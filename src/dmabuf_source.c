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

#include <dmabuf_source.h>

static bool gl_init = false;

struct wlr_frame {
	uint32_t format;
	uint32_t width, height;
	uint32_t obj_count;
	uint32_t strides[4];
	uint32_t sizes[4];
	int32_t fds[4];
	uint32_t offsets[4];
	uint32_t plane_indices[4];
	gs_texture_t* texture;
	EGLImage img;
	struct zwlr_export_dmabuf_frame_v1* frame;
};

struct wlr_source {
	struct wl_display* wl;
	struct wl_list outputs;
	struct output_node* current_output;
	struct zxdg_output_manager_v1* output_manager;
	struct zwlr_export_dmabuf_manager_v1* dmabuf_manager;
	struct wlr_frame* current_frame, *next_frame;
	bool waiting;
	bool show_cursor;
	bool render;
	pthread_mutex_t mutex;
	pthread_cond_t _waiting;
	obs_property_t* obs_outputs;
};

struct output_node {
	struct wl_output* output;
	char* name;
	struct zxdg_output_v1_listener* listener;
	struct wl_list link;
};

static const char* get_name(void* data) {
	(void) data;
	return "Wayland output(dmabuf)";
}

static void add_interface(void* data, struct wl_registry* registry, uint32_t name, const char* interface, uint32_t version) {
	struct wlr_source* this = data;
	if(strcmp(interface, wl_output_interface.name) == 0) {
		struct output_node* node = malloc(sizeof(struct output_node));
		node->output = wl_registry_bind(registry, name, &wl_output_interface, version);
		wl_list_insert(&this->outputs, &node->link);
	} else if(strcmp(interface, zxdg_output_manager_v1_interface.name) == 0) {
		this->output_manager = wl_registry_bind(registry, name, &zxdg_output_manager_v1_interface, version);
	} else if(strcmp(interface, zwlr_export_dmabuf_manager_v1_interface.name) == 0) {
		this->dmabuf_manager = wl_registry_bind(registry, name, &zwlr_export_dmabuf_manager_v1_interface, version);
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

	if(this->current_frame != NULL) {
		gs_texture_destroy(this->current_frame->texture);
		eglDestroyImage(eglGetCurrentDisplay(), this->current_frame->img);
		free(this->current_frame);
		this->current_frame = NULL;
	}

	if(this->next_frame != NULL) {
		gs_texture_destroy(this->next_frame->texture);
		eglDestroyImage(eglGetCurrentDisplay(), this->next_frame->img);
		free(this->next_frame);
		this->next_frame = NULL;
	}

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
		this->show_cursor = obs_data_get_bool(settings, "show_cursor");
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

static void _frame(void* data, struct zwlr_export_dmabuf_frame_v1* frame, uint32_t width, uint32_t height, uint32_t x, uint32_t y, uint32_t buffer_flags, uint32_t flags, uint32_t format, uint32_t high, uint32_t low, uint32_t obj_count) {
	(void) frame;
	(void) x;
	(void) y;
	(void) buffer_flags;
	(void) flags;
	(void) high;
	(void) low;
	struct wlr_source* this = data;
	this->next_frame = calloc(1, sizeof(struct wlr_frame));
	this->next_frame->format = format;
	this->next_frame->width = width;
	this->next_frame->height = height;
	this->next_frame->obj_count = obj_count;
}

static void object(void* data, struct zwlr_export_dmabuf_frame_v1* frame, uint32_t index, int32_t fd, uint32_t size, uint32_t offset, uint32_t stride, uint32_t plane_index) {
	(void) frame;
	struct wlr_source* this = data;
	this->next_frame->fds[index] = fd;
	this->next_frame->sizes[index] = size;
	this->next_frame->strides[index] = stride;
	this->next_frame->offsets[index] = offset;
	this->next_frame->plane_indices[index] = plane_index;
}

static void ready(void* data, struct zwlr_export_dmabuf_frame_v1* frame, uint32_t tv_sec_hi, uint32_t tv_sec_lo, uint32_t tv_nsec) {
	(void) frame;
	(void) tv_sec_hi;
	(void) tv_sec_lo;
	(void) tv_nsec;
	struct wlr_source* this = data;
	EGLint fd, offset, stride;
	switch(this->next_frame->plane_indices[0]) {
	case 0:
		fd = EGL_DMA_BUF_PLANE0_FD_EXT;
		offset = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
		stride = EGL_DMA_BUF_PLANE0_PITCH_EXT;
		break;
	case 1:
		fd = EGL_DMA_BUF_PLANE1_FD_EXT;
		offset = EGL_DMA_BUF_PLANE1_OFFSET_EXT;
		stride = EGL_DMA_BUF_PLANE1_PITCH_EXT;
		break;
	case 2:
		fd = EGL_DMA_BUF_PLANE2_FD_EXT;
		offset = EGL_DMA_BUF_PLANE2_OFFSET_EXT;
		stride = EGL_DMA_BUF_PLANE2_PITCH_EXT;
		break;
	default:
		fd = EGL_DMA_BUF_PLANE0_FD_EXT;
		offset = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
		stride = EGL_DMA_BUF_PLANE0_PITCH_EXT;
		break;
	}

	const EGLAttrib attr[] = {
		EGL_WIDTH, this->next_frame->width,
		EGL_HEIGHT, this->next_frame->height,
		EGL_LINUX_DRM_FOURCC_EXT, this->next_frame->format,
		fd, this->next_frame->fds[0],
		offset, this->next_frame->offsets[0],
		stride, this->next_frame->strides[0],
		EGL_NONE
	};

	this->next_frame->img = eglCreateImage(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, NULL, attr);
	this->next_frame->texture = gs_texture_create(this->next_frame->width, this->next_frame->height, GS_BGRA, 1, NULL, GS_GL_DUMMYTEX);
	glBindTexture(GL_TEXTURE_2D, *(GLuint*) gs_texture_get_obj(this->next_frame->texture));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, this->next_frame->img);

	glBindTexture(GL_TEXTURE_2D, 0);

	if(this->current_frame != NULL) {
		if(this->current_frame->img != NULL) {
			gs_texture_destroy(this->current_frame->texture);
			eglDestroyImage(eglGetCurrentDisplay(), this->current_frame->img);
		}

		if(this->current_frame->frame != NULL) {
			zwlr_export_dmabuf_frame_v1_destroy(this->current_frame->frame);
		}

		for(uint32_t count = 0; count < this->current_frame->obj_count; ++count) {
			close(this->current_frame->fds[count]);
		}
		free(this->current_frame);
	}

	this->current_frame = this->next_frame;
	this->next_frame = NULL;
	this->waiting = false;
}

static void cancel(void* data, struct zwlr_export_dmabuf_frame_v1* frame, enum zwlr_export_dmabuf_frame_v1_cancel_reason reason) {
	(void) reason;
	struct wlr_source* this = data;
	zwlr_export_dmabuf_frame_v1_destroy(frame);
	for(uint32_t count = 0; count < this->next_frame->obj_count; ++count) {
		close(this->next_frame->fds[count]);
	}
	this->waiting = false;
}

static void render(void* data, gs_effect_t* effect) {
	(void) effect;
	struct wlr_source* this = data;
	static struct zwlr_export_dmabuf_frame_v1_listener dmabuf_listener;

	if(!gl_init) {
		dmabuf_listener.frame = _frame;
		dmabuf_listener.object = object;
		dmabuf_listener.ready = ready;
		dmabuf_listener.cancel = cancel;

		gl_init = gladLoadGLES2Loader((GLADloadproc) eglGetProcAddress);


	}
	if(!this->render || this->current_output == NULL) {
		return;
	}
	if(!this->waiting) {
		this->waiting = true;
		struct zwlr_export_dmabuf_frame_v1* frame = zwlr_export_dmabuf_manager_v1_capture_output(this->dmabuf_manager, this->show_cursor, this->current_output->output);
		zwlr_export_dmabuf_frame_v1_add_listener(frame, &dmabuf_listener, this);
	}
	wl_display_roundtrip(this->wl);

	if(this->current_frame != NULL) {
		obs_source_draw(this->current_frame->texture, 0, 0, 0, 0, false);
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
	obs_properties_add_bool(props, "show_cursor", "Show mouse cursor");
	return props;
}

static uint32_t get_width(void* data) {
	struct wlr_source* this = data;
	if(this->current_frame == NULL) {
		return 0;
	} else {
		return this->current_frame->width;
	}
}

static uint32_t get_height(void* data) {
	struct wlr_source* this = data;
	if(this->current_frame == NULL) {
		return 0;
	} else {
		return this->current_frame->height;
	}
}

struct obs_source_info dmabuf_source = {
	.id = "wlrobs-dmabuf",
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
