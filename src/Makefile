# src/Makefile

CC=gcc
CFLAGS=-Wall -ggdb

PKG_CONFIG_EXE=/usr/bin/pkg-config

LIBMPD_CFLAGS=`${PKG_CONFIG_EXE} --cflags libmpd`
LIBMPD_LIBS=`${PKG_CONFIG_EXE} --libs libmpd`

GLIB_CFLAGS=`${PKG_CONFIG_EXE} --cflags glib-2.0`
GLIB_LIBS=`${PKG_CONFIG_EXE} --libs glib-2.0`

GTK_CFLAGS=`${PKG_CONFIG_EXE} --cflags gtk+-2.0`
GTK_LIBS=`${PKG_CONFIG_EXE} --libs gtk+-2.0`

FULL_CFLAGS=$(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS)
FULL_LIBS=$(GTK_LIBS) $(LIBMPD_LIBS)
FULL_BINARY_OPTS=$(FULL_CFLAGS) $(FULL_LIBS)

BINARIES=cli media_browser musicus-mpc

HEADERS=definitions.h mpd.h colorcodes.h media_browser.h support.h playlist.h playlist_gtk.h gui.h search.h preferences.h mpd_action.h ../config.h
COMMON_OBJS=mpd.o media_browser.o support.o playlist.o playlist_gtk.o gui.o search.o preferences.o mpd_action.o
ALL_OBJS=$(COMMON_OBJS) cli.o musicus.o

all: $(BINARIES)

# do applet build
applet:
	$(MAKE) -f Makefile.applet

mpd.o: mpd.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GLIB_CFLAGS) $(LIBMPD_CFLAGS) -o mpd.o mpd.c

media_browser.o: media_browser.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o media_browser.o media_browser.c

support.o: support.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o support.o support.c

playlist.o: playlist.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GLIB_CFLAGS) $(LIBMPD_CFLAGS) -o playlist.o playlist.c

playlist_gtk.o: playlist_gtk.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o playlist_gtk.o playlist_gtk.c

search.o: search.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o search.o search.c

gui.o: gui.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o gui.o gui.c

preferences.o: preferences.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) -o preferences.o preferences.c

mpd_action.o: mpd_action.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o mpd_action.o mpd_action.c

cli.o: cli.c $(HEADERS)
	$(CC) -c $(FULL_CFLAGS) -o cli.o cli.c

musicus.o: musicus.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(GTK_CFLAGS) $(LIBMPD_CFLAGS) -o musicus.o musicus.c

musicus-mpc: musicus.o $(COMMON_OBJS)
	$(CC) $(FULL_LIBS) -o musicus-mpc musicus.o $(COMMON_OBJS)

cli: cli.o $(COMMON_OBJS)
	$(CC) $(FULL_LIBS) -o cli cli.o $(COMMON_OBJS)

media_browser_so.o: media_browser_standalone.c $(HEADERS)
	$(CC) -c $(FULL_CFLAGS) -o media_browser_so.o media_browser_standalone.c

media_browser: media_browser_so.o $(COMMON_OBJS)
	$(CC) $(FULL_LIBS) -o media_browser media_browser_so.o $(COMMON_OBJS)

clean:
	touch $(ALL_OBJS)
	rm -f $(ALL_OBJS)
	touch $(BINARIES)
	rm -f $(BINARIES)
	$(MAKE) -f Makefile.applet clean
