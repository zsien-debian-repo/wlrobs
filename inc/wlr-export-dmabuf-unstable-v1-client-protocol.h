/* Generated by wayland-scanner 1.17.0 */

#ifndef WLR_EXPORT_DMABUF_UNSTABLE_V1_CLIENT_PROTOCOL_H
#define WLR_EXPORT_DMABUF_UNSTABLE_V1_CLIENT_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "wayland-client.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @page page_wlr_export_dmabuf_unstable_v1 The wlr_export_dmabuf_unstable_v1 protocol
 * a protocol for low overhead screen content capturing
 *
 * @section page_desc_wlr_export_dmabuf_unstable_v1 Description
 *
 * An interface to capture surfaces in an efficient way by exporting DMA-BUFs.
 *
 * Warning! The protocol described in this file is experimental and
 * backward incompatible changes may be made. Backward compatible changes
 * may be added together with the corresponding interface version bump.
 * Backward incompatible changes are done by bumping the version number in
 * the protocol and interface names and resetting the interface version.
 * Once the protocol is to be declared stable, the 'z' prefix and the
 * version number in the protocol and interface names are removed and the
 * interface version number is reset.
 *
 * @section page_ifaces_wlr_export_dmabuf_unstable_v1 Interfaces
 * - @subpage page_iface_zwlr_export_dmabuf_manager_v1 - manager to inform clients and begin capturing
 * - @subpage page_iface_zwlr_export_dmabuf_frame_v1 - a DMA-BUF frame
 * @section page_copyright_wlr_export_dmabuf_unstable_v1 Copyright
 * <pre>
 *
 * Copyright © 2018 Rostislav Pehlivanov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * </pre>
 */
struct wl_output;
struct zwlr_export_dmabuf_frame_v1;
struct zwlr_export_dmabuf_manager_v1;

/**
 * @page page_iface_zwlr_export_dmabuf_manager_v1 zwlr_export_dmabuf_manager_v1
 * @section page_iface_zwlr_export_dmabuf_manager_v1_desc Description
 *
 * This object is a manager with which to start capturing from sources.
 * @section page_iface_zwlr_export_dmabuf_manager_v1_api API
 * See @ref iface_zwlr_export_dmabuf_manager_v1.
 */
/**
 * @defgroup iface_zwlr_export_dmabuf_manager_v1 The zwlr_export_dmabuf_manager_v1 interface
 *
 * This object is a manager with which to start capturing from sources.
 */
extern const struct wl_interface zwlr_export_dmabuf_manager_v1_interface;
/**
 * @page page_iface_zwlr_export_dmabuf_frame_v1 zwlr_export_dmabuf_frame_v1
 * @section page_iface_zwlr_export_dmabuf_frame_v1_desc Description
 *
 * This object represents a single DMA-BUF frame.
 *
 * If the capture is successful, the compositor will first send a "frame"
 * event, followed by one or several "object". When the frame is available
 * for readout, the "ready" event is sent.
 *
 * If the capture failed, the "cancel" event is sent. This can happen anytime
 * before the "ready" event.
 *
 * Once either a "ready" or a "cancel" event is received, the client should
 * destroy the frame. Once an "object" event is received, the client is
 * responsible for closing the associated file descriptor.
 *
 * All frames are read-only and may not be written into or altered.
 * @section page_iface_zwlr_export_dmabuf_frame_v1_api API
 * See @ref iface_zwlr_export_dmabuf_frame_v1.
 */
/**
 * @defgroup iface_zwlr_export_dmabuf_frame_v1 The zwlr_export_dmabuf_frame_v1 interface
 *
 * This object represents a single DMA-BUF frame.
 *
 * If the capture is successful, the compositor will first send a "frame"
 * event, followed by one or several "object". When the frame is available
 * for readout, the "ready" event is sent.
 *
 * If the capture failed, the "cancel" event is sent. This can happen anytime
 * before the "ready" event.
 *
 * Once either a "ready" or a "cancel" event is received, the client should
 * destroy the frame. Once an "object" event is received, the client is
 * responsible for closing the associated file descriptor.
 *
 * All frames are read-only and may not be written into or altered.
 */
extern const struct wl_interface zwlr_export_dmabuf_frame_v1_interface;

#define ZWLR_EXPORT_DMABUF_MANAGER_V1_CAPTURE_OUTPUT 0
#define ZWLR_EXPORT_DMABUF_MANAGER_V1_DESTROY 1


/**
 * @ingroup iface_zwlr_export_dmabuf_manager_v1
 */
#define ZWLR_EXPORT_DMABUF_MANAGER_V1_CAPTURE_OUTPUT_SINCE_VERSION 1
/**
 * @ingroup iface_zwlr_export_dmabuf_manager_v1
 */
#define ZWLR_EXPORT_DMABUF_MANAGER_V1_DESTROY_SINCE_VERSION 1

/** @ingroup iface_zwlr_export_dmabuf_manager_v1 */
static inline void
zwlr_export_dmabuf_manager_v1_set_user_data(struct zwlr_export_dmabuf_manager_v1 *zwlr_export_dmabuf_manager_v1, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) zwlr_export_dmabuf_manager_v1, user_data);
}

/** @ingroup iface_zwlr_export_dmabuf_manager_v1 */
static inline void *
zwlr_export_dmabuf_manager_v1_get_user_data(struct zwlr_export_dmabuf_manager_v1 *zwlr_export_dmabuf_manager_v1)
{
	return wl_proxy_get_user_data((struct wl_proxy *) zwlr_export_dmabuf_manager_v1);
}

static inline uint32_t
zwlr_export_dmabuf_manager_v1_get_version(struct zwlr_export_dmabuf_manager_v1 *zwlr_export_dmabuf_manager_v1)
{
	return wl_proxy_get_version((struct wl_proxy *) zwlr_export_dmabuf_manager_v1);
}

/**
 * @ingroup iface_zwlr_export_dmabuf_manager_v1
 *
 * Capture the next frame of a an entire output.
 */
static inline struct zwlr_export_dmabuf_frame_v1 *
zwlr_export_dmabuf_manager_v1_capture_output(struct zwlr_export_dmabuf_manager_v1 *zwlr_export_dmabuf_manager_v1, int32_t overlay_cursor, struct wl_output *output)
{
	struct wl_proxy *frame;

	frame = wl_proxy_marshal_constructor((struct wl_proxy *) zwlr_export_dmabuf_manager_v1,
			 ZWLR_EXPORT_DMABUF_MANAGER_V1_CAPTURE_OUTPUT, &zwlr_export_dmabuf_frame_v1_interface, NULL, overlay_cursor, output);

	return (struct zwlr_export_dmabuf_frame_v1 *) frame;
}

/**
 * @ingroup iface_zwlr_export_dmabuf_manager_v1
 *
 * All objects created by the manager will still remain valid, until their
 * appropriate destroy request has been called.
 */
static inline void
zwlr_export_dmabuf_manager_v1_destroy(struct zwlr_export_dmabuf_manager_v1 *zwlr_export_dmabuf_manager_v1)
{
	wl_proxy_marshal((struct wl_proxy *) zwlr_export_dmabuf_manager_v1,
			 ZWLR_EXPORT_DMABUF_MANAGER_V1_DESTROY);

	wl_proxy_destroy((struct wl_proxy *) zwlr_export_dmabuf_manager_v1);
}

#ifndef ZWLR_EXPORT_DMABUF_FRAME_V1_FLAGS_ENUM
#define ZWLR_EXPORT_DMABUF_FRAME_V1_FLAGS_ENUM
/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 * frame flags
 *
 * Special flags that should be respected by the client.
 */
enum zwlr_export_dmabuf_frame_v1_flags {
	/**
	 * clients should copy frame before processing
	 */
	ZWLR_EXPORT_DMABUF_FRAME_V1_FLAGS_TRANSIENT = 0x1,
};
#endif /* ZWLR_EXPORT_DMABUF_FRAME_V1_FLAGS_ENUM */

#ifndef ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_REASON_ENUM
#define ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_REASON_ENUM
/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 * cancel reason
 *
 * Indicates reason for cancelling the frame.
 */
enum zwlr_export_dmabuf_frame_v1_cancel_reason {
	/**
	 * temporary error, source will produce more frames
	 */
	ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_REASON_TEMPORARY = 0,
	/**
	 * fatal error, source will not produce frames
	 */
	ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_REASON_PERMANENT = 1,
	/**
	 * temporary error, source will produce more frames
	 */
	ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_REASON_RESIZING = 2,
};
#endif /* ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_REASON_ENUM */

/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 * @struct zwlr_export_dmabuf_frame_v1_listener
 */
struct zwlr_export_dmabuf_frame_v1_listener {
	/**
	 * a frame description
	 *
	 * Main event supplying the client with information about the
	 * frame. If the capture didn't fail, this event is always emitted
	 * first before any other events.
	 *
	 * This event is followed by a number of "object" as specified by
	 * the "num_objects" argument.
	 * @param width frame width in pixels
	 * @param height frame height in pixels
	 * @param offset_x crop offset for the x axis
	 * @param offset_y crop offset for the y axis
	 * @param buffer_flags flags which indicate properties (invert, interlacing),                     has the same values as zwp_linux_buffer_params_v1:flags
	 * @param flags indicates special frame features
	 * @param format format of the frame (DRM_FORMAT_*)
	 * @param mod_high drm format modifier, high
	 * @param mod_low drm format modifier, low
	 * @param num_objects indicates how many objects (FDs) the frame has (max 4)
	 */
	void (*frame)(void *data,
		      struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1,
		      uint32_t width,
		      uint32_t height,
		      uint32_t offset_x,
		      uint32_t offset_y,
		      uint32_t buffer_flags,
		      uint32_t flags,
		      uint32_t format,
		      uint32_t mod_high,
		      uint32_t mod_low,
		      uint32_t num_objects);
	/**
	 * an object description
	 *
	 * Event which serves to supply the client with the file
	 * descriptors containing the data for each object.
	 *
	 * After receiving this event, the client must always close the
	 * file descriptor as soon as they're done with it and even if the
	 * frame fails.
	 * @param index index of the current object
	 * @param fd fd of the current object
	 * @param size size in bytes for the current object
	 * @param offset starting point for the data in the object's fd
	 * @param stride line size in bytes
	 * @param plane_index index of the the plane the data in the object applies to
	 */
	void (*object)(void *data,
		       struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1,
		       uint32_t index,
		       int32_t fd,
		       uint32_t size,
		       uint32_t offset,
		       uint32_t stride,
		       uint32_t plane_index);
	/**
	 * indicates frame is available for reading
	 *
	 * This event is sent as soon as the frame is presented,
	 * indicating it is available for reading. This event includes the
	 * time at which presentation happened at.
	 *
	 * The timestamp is expressed as tv_sec_hi, tv_sec_lo, tv_nsec
	 * triples, each component being an unsigned 32-bit value. Whole
	 * seconds are in tv_sec which is a 64-bit value combined from
	 * tv_sec_hi and tv_sec_lo, and the additional fractional part in
	 * tv_nsec as nanoseconds. Hence, for valid timestamps tv_nsec must
	 * be in [0, 999999999]. The seconds part may have an arbitrary
	 * offset at start.
	 *
	 * After receiving this event, the client should destroy this
	 * object.
	 * @param tv_sec_hi high 32 bits of the seconds part of the timestamp
	 * @param tv_sec_lo low 32 bits of the seconds part of the timestamp
	 * @param tv_nsec nanoseconds part of the timestamp
	 */
	void (*ready)(void *data,
		      struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1,
		      uint32_t tv_sec_hi,
		      uint32_t tv_sec_lo,
		      uint32_t tv_nsec);
	/**
	 * indicates the frame is no longer valid
	 *
	 * If the capture failed or if the frame is no longer valid after
	 * the "frame" event has been emitted, this event will be used to
	 * inform the client to scrap the frame.
	 *
	 * If the failure is temporary, the client may capture again the
	 * same source. If the failure is permanent, any further attempts
	 * to capture the same source will fail again.
	 *
	 * After receiving this event, the client should destroy this
	 * object.
	 * @param reason indicates a reason for cancelling this frame capture
	 */
	void (*cancel)(void *data,
		       struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1,
		       uint32_t reason);
};

/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 */
static inline int
zwlr_export_dmabuf_frame_v1_add_listener(struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1,
					 const struct zwlr_export_dmabuf_frame_v1_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) zwlr_export_dmabuf_frame_v1,
				     (void (**)(void)) listener, data);
}

#define ZWLR_EXPORT_DMABUF_FRAME_V1_DESTROY 0

/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 */
#define ZWLR_EXPORT_DMABUF_FRAME_V1_FRAME_SINCE_VERSION 1
/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 */
#define ZWLR_EXPORT_DMABUF_FRAME_V1_OBJECT_SINCE_VERSION 1
/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 */
#define ZWLR_EXPORT_DMABUF_FRAME_V1_READY_SINCE_VERSION 1
/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 */
#define ZWLR_EXPORT_DMABUF_FRAME_V1_CANCEL_SINCE_VERSION 1

/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 */
#define ZWLR_EXPORT_DMABUF_FRAME_V1_DESTROY_SINCE_VERSION 1

/** @ingroup iface_zwlr_export_dmabuf_frame_v1 */
static inline void
zwlr_export_dmabuf_frame_v1_set_user_data(struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) zwlr_export_dmabuf_frame_v1, user_data);
}

/** @ingroup iface_zwlr_export_dmabuf_frame_v1 */
static inline void *
zwlr_export_dmabuf_frame_v1_get_user_data(struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1)
{
	return wl_proxy_get_user_data((struct wl_proxy *) zwlr_export_dmabuf_frame_v1);
}

static inline uint32_t
zwlr_export_dmabuf_frame_v1_get_version(struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1)
{
	return wl_proxy_get_version((struct wl_proxy *) zwlr_export_dmabuf_frame_v1);
}

/**
 * @ingroup iface_zwlr_export_dmabuf_frame_v1
 *
 * Unreferences the frame. This request must be called as soon as its no
 * longer used.
 *
 * It can be called at any time by the client. The client will still have
 * to close any FDs it has been given.
 */
static inline void
zwlr_export_dmabuf_frame_v1_destroy(struct zwlr_export_dmabuf_frame_v1 *zwlr_export_dmabuf_frame_v1)
{
	wl_proxy_marshal((struct wl_proxy *) zwlr_export_dmabuf_frame_v1,
			 ZWLR_EXPORT_DMABUF_FRAME_V1_DESTROY);

	wl_proxy_destroy((struct wl_proxy *) zwlr_export_dmabuf_frame_v1);
}

#ifdef  __cplusplus
}
#endif

#endif
