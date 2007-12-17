# Makefile

INSTALL_DIR=/usr/bin
BONOBO_DIR=/usr/lib/bonobo/servers
DATA_DIR=/usr/share/pixmaps
SHARE_DIR=/usr/share/gnome-applets
PKG_CONFIG_EXE=/usr/bin/pkg-config

#don't edit below

VERSION=0.1a

.PHONY: clean install install_local dist doxy GNOME_MusicusApplet.server

#export all variables
export

# variables for compiler etc
CC=gcc
CFLAGS=-Wall -ggdb
BINARIES=cli media_browser musicus-mpc
SBINARIES=src/cli src/media_browser src/musicus-mpc
unexport SBINARIES
BBINARIES=bin/cli bin/media_browser bin/musicus-mpc
unexport BBINARIES
IBINARIES=src/musicus_applet src/musicus-mpc
unexport IBINARIES

all: code

code: applet musicus

applet: config.h
	cd src && $(MAKE) -e applet

musicus: config.h
	cd src && $(MAKE) -e

local_install: code
	mkdir -p bin
	cp $(SBINARIES) bin/

install: GNOME_MusicusApplet.server
	cp $(IBINARIES) $(INSTALL_DIR)
	cp GNOME_MusicusApplet.server $(BONOBO_DIR)
	cp data/musicus-applet.png $(DATA_DIR)
	mkdir -p $(SHARE_DIR)/musicus_applet
	cp data/GNOME_MusicusApplet.xml $(SHARE_DIR)/musicus_applet

GNOME_MusicusApplet.server: GNOME_MusicusApplet.server.in
	sed -e 's#?bindir#$(INSTALL_DIR)#g' GNOME_MusicusApplet.server.in > GNOME_MusicusApplet.server

config.h: config.h.in
	sed -e 's#?bindir#$(INSTALL_DIR)#g' -e 's#?datadir#$(DATA_DIR)#g' -e 's#?sharedir#$(SHARE_DIR)#g' -e 's#?bonobodir#$(BONOBO_DIR)#g' -e 's#?version#$(VERSION)#g' config.h.in > config.h
	
doxy:
	mkdir -p doc
	doxygen Doxyfile

dist:
	mkdir -p tmp/musicus-applet/data
	mkdir -p tmp/musicus-applet/src
	touch tmp/VERSION-$(VERSION)
	cp src/*.c src/*.h src/Makefile* tmp/musicus-applet/src
	cp Makefile Doxyfile TODO AUTHORS COPYING README GNOME_MusicusApplet.server.in config.h.in tmp/musicus-applet
	cp data/GNOME_MusicusApplet.xml tmp/musicus-applet/data
	cp data/musicus-applet.png tmp/musicus-applet/data
	(cd tmp && tar cjf ../musicus-applet-$(VERSION).tar.bz2 musicus-applet/* VERSION-$(VERSION))
	rm -r tmp/musicus-applet tmp/VERSION-$(VERSION)

clean:
	touch $(BBINARIES)
	cd src && $(MAKE) clean
	rm -f $(BBINARIES)
	rm -f config.h GNOME_MusicusApplet.server

