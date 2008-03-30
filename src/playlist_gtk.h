/**
 * \file playlist_gtk.h
 * \author Simon Gerber
 * \date 2007-12-15
 * This file contains methods to  build a GTK+ Interface for the playlist
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

#ifndef __PLAYLIST_GTK_H
#define __PLAYLIST_GTK_H

#include "definitions_gtk.h"
#include "playlist.h"

 /**\defgroup 51gtkpl GTK+ Playlist
  * GTK+ related playlist setup procedures
  */

/*@{*/

/**
 * PlTreeStore contains the TreeStore for the playlist
 */
GtkListStore *PlListStore;

/**
 * Initializes a GtkWidget to display the playlist
 * \return a pointer to the newly created playlist widget
 */
GtkWidget *init_pl_widget(void);

/**
 * update playlist tree on playlist change / song change
 */
void playlist_update_tree(void);

/**
 * Delete selected songs from playlist
 * \param widget the widget from which the function was called
 * \param data user data
 */
void pl_del_songs(GtkWidget *widget, gpointer data);

/**
 * Add media resource locator to playlist (e.g. web stream)
 * \param obj the MpdObj to add to.
 * \param mrl the media resource locator.
 */
void mpd_mrl_add(MpdObj *obj, const gchar *mrl);

/**
 * Adds a song list to the playlist
 * \param *widget the GtkWidget from which the function was called
 * \param data the list of songs to add (cast to GList*)
 */
void mpd_add_song_list(GtkWidget *widget, gpointer data);

/**
 * Adds a directory to the playlist
 * \param widget the widget from which the function was called
 * \param data the file name of the directory (cast to gchar*)
 */
void mpd_add_directory_to_playlist(GtkWidget *widget, gpointer data);

/*@}*/

#endif // __PLAYLIST_GTK_H

/* vim:sts=4:shiftwidth=4
 */
