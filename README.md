# wlrobs
wlrobs is an obs-studio plugin that allows you to screen capture on wlroots based wayland compositors

[![builds.sr.ht status](https://builds.sr.ht/~scoopta/wlrobs.svg)](https://builds.sr.ht/~scoopta/wlrobs?)

This plugin only records wayland desktops, it does not make OBS run wayland native. If you're not using a fork of OBS with an EGL backend you need to set `QT_QPA_PLATFORM=xcb` or else OBS does not work.

## dmabuf backend
Please note that in order to use the dmabuf backend you have to use a fork of OBS which has an EGL backend such as https://github.com/GeorgesStavracas/obs-studio/tree/feaneron/egl-wayland

## Dependencies
	libwayland-dev
	libobs-dev
	pkg-config
	meson
The dmabuf backend additionally requires

	libdrm-dev
	libegl-dev
## Building
	hg clone https://hg.sr.ht/~scoopta/wlrobs
	cd wlrobs
	meson build
	ninja -C build
The screencopy backend can be disabled with the `-Duse_scpy=false` meson option, likewise dmabuf can be disabled with `-Duse_dmabuf=false`

## Installing
	mkdir -p ~/.config/obs-studio/plugins/wlrobs/bin/64bit
	cp build/libwlrobs.so ~/.config/obs-studio/plugins/wlrobs/bin/64bit
## Uninstalling
	rm -rf ~/.config/obs-studio/plugins/wlrobs
## Bug Reports
Please file bug reports at https://todo.sr.ht/~scoopta/wlrobs
## Contributing
Please submit patches to https://lists.sr.ht/~scoopta/wlrobs

You can find documentation here https://man.sr.ht/hg.sr.ht/email.md
