project('wlrobs', 'c', default_options : ['buildtype=release', 'warning_level=2'])

inc = include_directories('inc')
wayland = dependency('wayland-client')
obs = dependency('libobs')

sources = ['src/main.c', 'proto/xdg-output-unstable-v1-protocol.c']

deps = [wayland, obs]

if get_option('use_scpy')
	sources += ['src/scpy_source.c', 'proto/wlr-screencopy-unstable-v1-protocol.c']
	add_project_arguments('-DENABLE_SCPY', language : 'c')
endif

if get_option('use_dmabuf')
	egl = dependency('egl', required : false)
	drm = dependency('libdrm', required : false)
	if egl.found() and drm.found()
		deps += [egl, drm]
		sources += ['src/dmabuf_source.c',
					'glad/glad.c',
					'proto/wlr-export-dmabuf-unstable-v1-protocol.c']
		add_project_arguments('-DENABLE_DMABUF', language : 'c')
	else
		warning('dmabuf was disabled due to missing dependencies')
	endif
endif

shared_library(
	meson.project_name(),
	sources,
	include_directories : inc,
	dependencies : deps,
	install : true,
	install_dir : get_option('libdir') / 'obs-plugins'
)
