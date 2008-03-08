/**
 * \file media_browser.h
 * \author Simon Gerber
 * \date 2007-12-09
 * This file contains function headers and global structs
 * for the media browser component
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

#ifndef __media_browser_h
#define __media_browser_h

/**\defgroup 20mediabrowser Media Browser
 * Media browser window and widget
 */
/*@{*/

#include "definitions_gtk.h"

#include "colorcodes.h"

/**
 * WinInfo struct for the media browser window
 */
WinInfo MbWin;

/**
 * TreeStore for the mpd database image for the media browser
 */
GtkTreeStore *media_database;

/**
 * enumeration of the columns for the media_database TreeStore
 */
enum {
	/// media file name
	M_COLUMN_FILE,
	/// media file identifier
	M_COLUMN_IDENTIFIER,
	/// number of columns
	M_COLUMNS
};

/**
 * build a new media browser window
 * \return TRUE if success, FALSE if not
 */
gboolean media_browser_new();

/**
 * build a new media browser widget
 * \param folder_id the name of the folder which should be opened
 * on construction
 * \return a pointer to the newly created widget
 */
GtkWidget *init_mb_widget(gchar *folder_id);

/**
 * hide the media browser window
 */
void media_browser_window_show();

/**
 * show the media browser window
 */
void media_browser_window_hide();

/**
 * Reread the TreeStore
 */
void media_browser_reread();

/*@}*/
#endif

/* vim:sts=4:shiftwidth=4
 */

