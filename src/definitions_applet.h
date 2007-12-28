/**
 * \file definitions_applet.h
 * \author Simon Gerber
 * \date 2007-12-16
 * This file contains definitions for the GNOME applet
 */

/**********************************************************************
 * Copyright 2006, 2007 by Simon Gerber <simugerber@student.ethz.ch>
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

#ifndef __DEFINITIONS_APPLET_H
#define __DEFINITIONS_APPLET_H

/**\defgroup 80appletdef Applet definitions
 * Definitions and global objects for the GNOME applet
 */
/*@{*/

#include "definitions_gtk.h"
#include <panel-applet.h>

/**
 * buffer for the tooltip text
 */
gchar tt_text[BUF_LEN];

/**
 * buffer for the applet text
 */
gchar ap_text[BUF_LEN];

/**
 * Structure for the Applet
 */
typedef struct MusicusApplet_ {
	PanelApplet *applet;
	GtkWidget *label;
	GtkTooltips *tooltips;
	gboolean show_text;
} MusicusApplet;

MusicusApplet musicus_applet;

/*@}*/

#endif // __DEFINITIONS_APPLET_H
