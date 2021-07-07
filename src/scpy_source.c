/*
 *  Copyright (C) 2019-2020 Scoopta
 *  This file is part of wlrobs
 *  wlrobs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    wlrobs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wlrobs.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <scpy_source.h>

struct wlr_frame {
	uint32_t format;
	uint32_t width, height;
	uint32_t stride;
	uint32_t size;
	struct wl_shm_pool* pool;
	struct wl_buffer* buffer;
	int32_t fd;
};

struct wlr_source {
	struct wl_display* wl;
	struct wl_list outputs;
	struct output_node* current_output;
	struct wl_shm* shm;
	struct zxdg_output_manager_v1* output_manager;
	struct zwlr_screencopy_manager_v1* copy_manager;
	struct wlr_frame* frame;
	bool waiting;
	bool flip_rb;
	bool show_cursor;
	bool render;
	pthread_mutex_t mutex;
	pthread_cond_t _waiting;
	obs_property_t* obs_outputs;
	uint32_t x, y;
	uint32_t width, height;
};

struct output_node {
	struct wl_output* output;
	char* name;
	struct zxdg_output_v1_listener* listener;
	struct wl_list link;
};

static const char* get_name(void* data) {
	(void) data;
	return "Wayland output(scpy)";
}

static void add_interface(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
	struct wlr_source* this = data;
	if(strcmp(interface, wl_output_interface.name) == 0) {
		struct output_node* node = malloc(sizeof(struct output_node));
		node->output = wl_registry_bind(registry, name, &wl_output_interface, version);
		wl_list_insert(&this->outputs, &node->link);
	} else if(strcmp(interface, wl_shm_interface.name) == 0) {
		this->shm = wl_registry_bind(registry, name, &wl_shm_interface, version);
	} else if(strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
		this->output_manager = wl_registry_bind(registry, name, &zxdg_output_manager_v1_interface, version);
	} else if(strcmp(interface, zwlr_screencopy_manager_v1_interface.name) == 0) {
		this->copy_manager = wl_registry_bind(registry, name, &zwlr_screencopy_manager_v1_interface, version);
	}
}

static void nop() {}

static void get_xdg_name(void* data, struct zxdg_output_v1* output, const char* name) {
	(void) output;
	struct output_node* node = data;
	node->name = strdup(name);
}

static void destroy(void* data) {
	struct wlr_source* this = data;
	struct output_node* node, *safe_node;
	wl_list_for_each_safe(node, safe_node, &this->outputs, link) {
		wl_list_remove(&node->link);
		free(node->name);
		free(node->listener);
		node->name = NULL;
		free(node);
	}
	this->current_output = NULL;
	free(this->frame);
	this->frame = NULL;
	if(this->wl != NULL) {
		wl_display_disconnect(this->wl);
	}
}

static void destroy_complete(void* data) {
	destroy(data);
	free(data);
}

static void populate_outputs(struct wlr_source* this) {
	struct output_node* node;
	wl_list_for_each(node, &this->outputs, link) {
		obs_property_list_add_string(this->obs_outputs, node->name, node->name);
	}
}

static void setup_display(struct wlr_source* this, const char* display) {
	pthread_mutex_lock(&this->mutex);
	while(this->waiting) {
		pthread_cond_wait(&this->_waiting, &this->mutex);
	}
	pthread_mutex_unlock(&this->mutex);
	this->render = false;
	if(this->wl != NULL) {
		destroy(this);
	}
	wl_list_init(&this->outputs);
	if(strcmp(display, "") == 0) {
		display = NULL;
	}
	this->wl = wl_display_connect(display);
	if(this->wl == NULL) {
		return;
	}
	struct wl_registry* registry = wl_display_get_registry(this->wl);
	struct wl_registry_listener listener = {
		.global = add_interface,
		.global_remove = nop
	};
	wl_registry_add_listener(registry, &listener, this);
	wl_display_roundtrip(this->wl);
	struct output_node* node;
	wl_list_for_each(node, &this->outputs, link) {
		struct zxdg_output_v1* xdg_output = zxdg_output_manager_v1_get_xdg_output(this->output_manager, node->output);
		node->listener = malloc(sizeof(struct zxdg_output_v1_listener));
		node->listener->description = nop;
		node->listener->done = nop;
		node->listener->logical_position = nop;
		node->listener->logical_size = nop;
		node->listener->name = get_xdg_name;
		zxdg_output_v1_add_listener(xdg_output, node->listener, node);
	}
	wl_display_roundtrip(this->wl);
	this->render = true;
}

static void update(void* data, obs_data_t* settings) {
	struct wlr_source* this = data;
	if(this->render) {
		struct output_node* node;
		wl_list_for_each(node, &this->outputs, link) {
			if(strcmp(node->name, obs_data_get_string(settings, "output")) == 0) {
				this->current_output = node;
			}
		}
		this->flip_rb = obs_data_get_bool(settings, "flip_rb");
		this->show_cursor = obs_data_get_bool(settings, "show_cursor");
		this->x = obs_data_get_int(settings, "x");
		this->y = obs_data_get_int(settings, "y");
		this->width = obs_data_get_int(settings, "width");
		this->height = obs_data_get_int(settings, "height");
	}
}

static void* create(obs_data_t* settings, obs_source_t* source) {
	(void) source;
	struct wlr_source* this = calloc(1, sizeof(struct wlr_source));
	pthread_mutex_init(&this->mutex, NULL);
	pthread_cond_init(&this->_waiting, NULL);
	setup_display(this, obs_data_get_string(settings, "display"));
	update(this, settings);
	return this;
}

static void buffer(void* data, struct zwlr_screencopy_frame_v1* frame, uint32_t format, uint32_t width, uint32_t height, uint32_t stride) {
	struct wlr_source* this = data;
	if(this->frame != NULL) {
		free(this->frame);
	}
	this->frame = malloc(sizeof(struct wlr_frame));
	this->frame->format = format;
	this->frame->width = width;
	this->frame->height = height;
	this->frame->stride = stride;
	this->frame->size = stride * height;
	this->frame->fd = shm_open("/wlrobs", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	shm_unlink("/wlrobs");
	ftruncate(this->frame->fd, stride * height);
	this->frame->pool = wl_shm_create_pool(this->shm, this->frame->fd, stride * height);
	this->frame->buffer = wl_shm_pool_create_buffer(this->frame->pool, 0, width, height, stride, format);
	zwlr_screencopy_frame_v1_copy(frame, this->frame->buffer);
}

static void ready(void* data, struct zwlr_screencopy_frame_v1* frame, uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) {
	(void) tv_sec_hi;
	(void) tv_sec_lo;
	(void) tv_nsec;
	struct wlr_source* this = data;
	void* buffer = mmap(NULL, this->frame->size, PROT_READ, MAP_SHARED, this->frame->fd, 0);
	enum gs_color_format format;
	if(this->flip_rb) {
		format = GS_BGRA;
	} else {
		format = GS_RGBA;
	}
	gs_texture_t* texture = gs_texture_create(this->frame->width, this->frame->height, format, 1, (const uint8_t**) &buffer, 0);
	obs_source_draw(texture, 0, 0, 0, 0, true);
	gs_texture_destroy(texture);
	zwlr_screencopy_frame_v1_destroy(frame);
	munmap(buffer, this->frame->size);
	wl_buffer_destroy(this->frame->buffer);
	wl_shm_pool_destroy(this->frame->pool);
	close(this->frame->fd);
	this->waiting = false;
}

static void failed(void* data, struct zwlr_screencopy_frame_v1* frame) {
	struct wlr_source* this = data;
	zwlr_screencopy_frame_v1_destroy(frame);
	wl_buffer_destroy(this->frame->buffer);
	wl_shm_pool_destroy(this->frame->pool);
	close(this->frame->fd);
	shm_unlink("/wlrobs");
	this->waiting = false;
}

static void render(void* data, gs_effect_t* effect) {
	(void) effect;
	struct wlr_source* this = data;
	if(!this->render || this->current_output == NULL) {
		return;
	}
	this->waiting = true;

	struct zwlr_screencopy_frame_v1* frame;
	if(this->width == 0 || this->height == 0) {
		frame = zwlr_screencopy_manager_v1_capture_output(this->copy_manager, this->show_cursor, this->current_output->output);
	} else {
		frame = zwlr_screencopy_manager_v1_capture_output_region(this->copy_manager, this->show_cursor, this->current_output->output, this->x, this->y, this->width, this->height);
	}

	struct zwlr_screencopy_frame_v1_listener listener = {
		.buffer = buffer,
		.flags = nop,
		.ready = ready,
		.failed = failed,
		.damage = nop,
		.linux_dmabuf = nop,
		.buffer_done = nop,
	};
	zwlr_screencopy_frame_v1_add_listener(frame, &listener, this);
	while(this->waiting) {
		wl_display_roundtrip(this->wl);
	}
	pthread_mutex_lock(&this->mutex);
	pthread_cond_broadcast(&this->_waiting);
	pthread_mutex_unlock(&this->mutex);
}

static bool update_outputs(void* data, obs_properties_t* props, obs_property_t* property, obs_data_t* settings) {
	(void) props;
	(void) property;
	struct wlr_source* this = data;
	if(this->obs_outputs == NULL) {
		return false;
	}
	setup_display(this, obs_data_get_string(settings, "display"));
	obs_property_list_clear(this->obs_outputs);
	populate_outputs(this);
	update(data, settings);
	return true;
}

static obs_properties_t* get_properties(void* data) {
	struct wlr_source* this = data;
	obs_properties_t* props = obs_properties_create();
	obs_property_t* display = obs_properties_add_text(props, "display", "Wayland Display", OBS_TEXT_DEFAULT);
	obs_property_set_modified_callback2(display, update_outputs, this);
	this->obs_outputs = obs_properties_add_list(props, "output", "Output", OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	populate_outputs(this);
	obs_properties_add_bool(props, "flip_rb", "Flip red and blue");
	obs_properties_add_bool(props, "show_cursor", "Show mouse cursor");
	obs_properties_add_int(props, "x", "X", 0, UINT16_MAX, 1);
	obs_properties_add_int(props, "y", "Y", 0, UINT16_MAX, 1);
	obs_properties_add_int(props, "width", "Width", 0, UINT16_MAX, 1);
	obs_properties_add_int(props, "height", "Height", 0, UINT16_MAX, 1);
	return props;
}

static uint32_t get_width(void* data) {
	struct wlr_source* this = data;
	if(this->frame == NULL) {
		return 0;
	} else {
		return this->frame->width;
	}
}

static uint32_t get_height(void* data) {
	struct wlr_source* this = data;
	if(this->frame == NULL) {
		return 0;
	} else {
		return this->frame->height;
	}
}

struct obs_source_info scpy_source = {
	.id = "wlrobs-scpy",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = get_name,
	.create = create,
	.destroy = destroy_complete,
	.update = update,
	.video_render = render,
	.get_width = get_width,
	.get_height = get_height,
	.get_properties = get_properties
};
