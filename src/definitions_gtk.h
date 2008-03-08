/**
 * \file definitions_gtk.h
 * \author Simon Gerber
 * \date 2007-12-09
 * This file contains global definitions for the GTK+ interfaces
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

#ifndef __definitions_gtk_h
#define __definitions_gtk_h

/**\defgroup 00globals-gtk Global GTK+
 * Global structures and data for GTK+ interfaces.
 */
/*@{*/

#include "definitions.h"
#include <gtk/gtk.h>

/**
 * this variable is used for the statusbar text in the GTK+ interface
 */
char sb_text[BUF_LEN];

/**
 * Length of the geometry string in the #WinInfo struct
 */
#define GEOM_LEN 64

/**
 * window information struct which contains all that is
 * necessary to know about a window
 */
typedef struct {
    /// the window widget
    GtkWidget *win;
    /// variable which indicates the status of the window creation process
    /**
     * this variable is set to 0 when the creation of a interface main
     * window begins, and set to 1 when it finishes
     */
    int window_creation_finished;
    /// window size and position
    struct {
	/// X-geometry string of the form widthxheight+x+y
	gchar geom[GEOM_LEN];
	/// is the information in #geom valid?
	gboolean filled;
    } characteristics;
} WinInfo;

/**
 * enumeration for the playlist columns
 */
enum {
    /// filename
    COLUMN_FILE,
    /// playlist id
    COLUMN_PL_ID,
    /// is this song currently played
    COLUMN_PLAYING,
    /// title of the song
    COLUMN_TITLE,
    /// artist of the song
    COLUMN_ARTIST,
    /// album of the song
    COLUMN_ALBUM,
    /// total time of the song (seconds)
    COLUMN_TIME,
    /// total time of the song (human readable)
    COLUMN_HUMAN_TIME,
    /// number of columns
    N_COLUMNS
};


/*@}*/
#endif

/* vim:sts=4:shiftwidth=4
 */
