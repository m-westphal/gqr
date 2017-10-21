
LIBGQR_PATH=../../_build_/default/gqr

example: example.c
	gcc -o example example.c `pkg-config --libs gobject-2.0` `pkg-config  --cflags gobject-2.0` -I ../ -lgqr -L${LIBGQR_PATH}

