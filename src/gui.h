/**
 * \file gui.h
 * \author Simon Gerber
 * \date 2007-12-15
 * This file declares the globally used main GUI procedures
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

#ifndef __GUI_H
#define __GUI_H

/**\defgroup 70gui GUI
 * main GUI elements
 */
/*@{*/

#include "definitions_gtk.h"

/**
 * Struct to hold window information
 */
WinInfo MpdWin;

/**
 * This is the procedure which builds the main user interface
 * for the GTK+ client
 * \param updater set this to 1, if you want to have periodically
 * updating functions enabled
 * \param applet set this to 1, if you're building the main window
 * from the context of a GNOME applet
 * \return the newly created GUI window
 */
GtkWidget *mpd_win_new(int updater, int applet);


/**
 * This function hides the not-connected message
 * and shows the mpd control elements
 */
void mpd_gui_show_mpd_elements(void);
/**
 * This function hides the mpd control elements
 * and displays a not-connected message
 */
void mpd_gui_hide_mpd_elements(void);

/**
 * This function is used to get callbacks from mpd when something
 * happens, this is _much_ better than periodic polling!
 * \param obj The MpdObj from which the call originated
 * \param what What has changed?
 * \param data user data
 */
void gui_mpd_status_changed_cb(MpdObj *obj, ChangedStatusType what, gpointer data);

 /*@}*/

#endif // __GUI_H

/* vim:sts=4:shiftwidth=4
 */

