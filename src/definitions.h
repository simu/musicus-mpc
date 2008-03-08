/**
 * \file definitions.h
 * \author Simon Gerber
 * \date 2007-12-08
 * This file contains general global definitions
 */

/**********************************************************************
 * Copyright 2006 - 2008 by Simon Gerber <simugerber@student.ethz.ch>
 *
 * This file is part of Musicus.
 * Musicus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Musicus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Musicus.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

/**
 * \mainpage Musicus MPD client 
 *
 * Don't miss to check the project's main page at http://musicus-mpc.berlios.de
 *
 * Musicus MPD client (musicus-mpc) is a client for the 
 * music player daemon MPD (<a href="http://musicpd.org">MPD homepage</a>)
 * It contains two executables. <br>
 * The first is an applet for the GNOME panel. 
 * The other is a standalone version of the Playlist/Media Browser component
 * of the applet.
 *
 * The project is currently hosted on berliOS. Visit the project overview
 * page at http://developer.berlios.de/projects/musicus-mpc/
 */

#ifndef __definitions_h
#define __definitions_h

/**\defgroup 00globals Global
 * Global structures and data.
 */
/*@{*/

#include "../config.h"

#include <stdio.h>
#include <libmpd/libmpd.h>
#include <libmpd/libmpdclient.h>
#include <glib.h>

#define AUTHORS "Simon Gerber <simugerber@student.ethz.ch>"
#define DOCUMENTERS "Simon Gerber <simugerber@student.ethz.ch>"
#define LOGO "musicus-mpc"

/**
 * if this variable is set to 1, debug output is generated
 * this output goes to the file specified by log_file
 */
int debug;
/**
 * if this variable is et to 1, more verbose debug output is generated
 * this output also goes to the file specified by log_file
 */
int verbose;

/**
 * the file handle which is used as destination for error messages
 */
FILE *err_file;
/**
 * the file handle which is used as destination for log messages
 */
FILE *log_file;

#define BUF_LEN 256
/**
 * this variable is used for an automatic title update routine
 * the update routine runs all mpd_info.update_interval milliseconds
 */
char title_text[BUF_LEN];

/* mpd related */

/**
 * The MpdPlayState struct holds information about playback settings in mpd
 */
typedef struct {
    /// True if random playback enabled
    gboolean random;
    /// True if repeating playback enabled
    gboolean repeat;
    /// 0 if crossfade disabled, otherwise crossfade time in seconds
    gint xfade;
    /// is database updating
    gboolean update;
} MpdPlayState;

/**
 * MpdStateInfo struct holds information about connection settings
 */
typedef struct {
    /// true if connected to mpd daemon specified by host:port
    gboolean connected;
    /// the host of the machine where the mpd daemon runs
    GString *host;
    /// the port of the mpd daemon
    gint port;
    /// timeout for the connection
    gint timeout;
    /// password for the mpd daemon
    GString *pw;
} MpdStateInfo;

/**
 * The MpdStatusFmt struct holds all format strings
 */
typedef struct {
    /// the general title format
    GString *title_fmt;
    /// the statusbar format
    GString *statusbar_fmt;
    /// the tooltip format (only used in applet context)
    GString *tooltip_fmt;
    /// the applet string format (only used in applet context)
    GString *applet_fmt;
} MpdStatusFmt;

/**
 * Format string types enumeration
 */
enum {
    /// tooltip format
    FMT_TOOLTIP,
    /// title format
    FMT_TITLE,
    /// statusbar format
    FMT_STATUSBAR,
    /// applet format
    FMT_APPLET
};

/**
 * Format string contents enumeration
 */
enum {
    /// no variable contents
    HAS_NOTHING = 0,
    /// state (playing / paused)
    HAS_STATE = 1 << 0,
    /// current time
    HAS_CURRENT_TIME = 1 << 1,
    /// song time
    HAS_SONG_TIME = 1 << 2,
    /// all of the above three
    HAS_ALL = HAS_STATE|HAS_CURRENT_TIME|HAS_SONG_TIME
};

/**
 * The MpdInfo struct holds all MPD-related information
 */
typedef struct {
    /// the absolute path for the config file
    GString *config_file;
    /// true if program should triy to connect to mpd on startup
    gboolean auto_connect;
    /// A MpdObj to actually connect to MPD
    MpdObj *obj;
    /// The StateInfo of the mpd daemon
    MpdStateInfo msi;
    /// The playback state of the mpd daemon
    MpdPlayState mps;
    /// The format strings for the different status texts
    MpdStatusFmt msf;
    /// the update interval for the title updater
    gint update_interval;
    /// the length of the crossfade effect
    gint xfade_secs;
    /// the routines which are called all update_interval milliseconds
    GList *update_routines;
    /// a second set of routines which are called all 500ms regardless of
    /// the value of update_interval
    GList *period_funcs;
} MpdInfo;

/**
 *  global MpdInfo struct
 */
MpdInfo mpd_info;

/*
 * convenience macros for mpd_info fields, read-only
 */
/// convenience macro for mpd host
#define HOST mpd_info.msi.host->str
/// convenience macro for mpd port
#define PORT mpd_info.msi.port
/// convenience macro for connection timeout
#define TIMEOUT mpd_info.msi.timeout
/// convenience macro for mpd password
#define PASSWORD mpd_info.msi.pw->str


/*@}*/
#endif

/* vim:softtabstop=4:shiftwidth=4
 */
