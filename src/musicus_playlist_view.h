/**
 * \file musicus_playlist_view.h
 * \author Simon Gerber
 * \date 2008-03-23
 * This file contains the definitions for a PlaylistView widget.
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

#ifndef MUSICUS_PLAYLIST_VIEW_H
#define MUSICUS_PLAYLIST_VIEW_H

#include <gtk/gtk.h>
#include "musicus_song.h"

/**\defgroup 202playlistview MusicusPlaylistView
 * A widget for displaying MPD playlists.
 */

/*@{*/

/**
 * This gives the GType for MusicusPlaylistView.
 * \return GType for MusicusPlaylistView
 */
#define MUSICUS_PLAYLIST_VIEW_TYPE (musicus_playlist_view_get_type())

/**
 * This tries to cast a pointer to type MusicusPlaylistView*
 * \param obj The pointer to convert.
 * \return the converted pointer.
 */
#define MUSICUS_PLAYLIST_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUSICUS_PLAYLIST_VIEW_TYPE, MusicusPlaylistView))

/**
 * This tries to cast a class pointer to MusicusPlaylistViewClass.
 * \param klass the class to convert.
 * \return the converted pointer.
 */
#define MUSICUS_PLAYLIST_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MUSICUS_PLAYLIST_VIEW_TYPE, MusicusPlaylistViewClass))

/**
 * This checks if `obj` is of type MusicusPlaylistView.
 * \param obj the pointer to check.
 * \return TRUE if `obj` is an instance of MusicusPlaylistView. FALSE and emits a warning otherwise.
 */
#define MUSICUS_IS_PLAYLIST_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUSICUS_PLAYLIST_VIEW_TYPE,))

/**
 * This checks if `klass` is of type MusicusPlaylistViewClass.
 * \param klass the pointer to check.
 * \return TRUE if `klass` is an instance of MusicusPlaylistViewClass. FALSE and emits a warning otherwise.
 */
#define MUSICUS_IS_PLAYLIST_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MUSICUS_PLAYLIST_VIEW_TYPE))

/**
 * This returns the class of `obj`.
 * \param obj the instance to get the class of.
 * \return a MusicusPlaylistViewClass.
 */
#define MUSICUS_PLAYLIST_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MUSICUS_PLAYLIST_VIEW_GET_TYPE, MusicusPlaylistViewClass))

typedef struct _MusicusPlaylistView MusicusPlaylistView;
typedef struct _MusicusPlaylistViewClass MusicusPlaylistViewClass;
typedef struct _MusicusPlaylistViewPrivate MusicusPlaylistViewPrivate;

/**
 * The main structure, used for client accessible fields.
 * Has none of those right now.
 */
struct _MusicusPlaylistView {
    /// reference to parent object.
    GtkTreeView parent;

    /// private fields
    MusicusPlaylistViewPrivate *priv;
};

/**
 * The class structure.
 */
struct _MusicusPlaylistViewClass {
    /// parent class
    GtkTreeViewClass parent;
};

/**
 * Gets the GType for MusicusSong\n
 * <b>Do not call from user applications directly. Always use MUSICUS_SONG_TYPE.</b>
 * \return the GType for MusicusSong.
 */
GType musicus_playlist_view_get_type(void);

/**
 * Create a new MusicusPlaylistView object.
 * \return a new MusicusPlaylistView object.
 */
MusicusPlaylistView *musicus_playlist_view_new();

/**
 * Appends a MusicusSong to the PlaylistView `pl`
 * \param pl the PlaylistView to append the song to.
 * \param song the MusicusSong to append.
 */
void musicus_playlist_view_append_song(MusicusPlaylistView *pl, MusicusSong *song);

/**
 * Get the song list for the PlaylistView `pl`.
 * \param pl the PlaylistView to get the List from.
 * \return a pointer to the songs GList.
 */
GList *musicus_playlist_view_get_songs(MusicusPlaylistView *pl);

/**
 * Set the song list for the PlaylistView `pl`.
 * \param pl the PlaylistView to set the list.
 * \param songs the new song list.
 */
void musicus_playlist_view_set_songs(MusicusPlaylistView *pl, GList *songs);

/**
 * Initializes the ListView component
 * \param pl the PlaylistView to initialize.
 */
void musicus_playlist_view_build_view_from_songs(MusicusPlaylistView *pl);
/**
 * Updates the ListView component.
 * \param pl the PlaylistView to update.
 */
void musicus_playlist_view_update_view(MusicusPlaylistView *pl);

/**
 * Get the id of the currently active playlist element.
 * \param pl the PlaylistView to get the id of.
 * \return the id.
 */
gint musicus_playlist_view_get_active_id(MusicusPlaylistView *pl);

/**
 * Set the id of the currently active playlist element.
 * \param pl the PlaylistView to set the id.
 * \param active_id the new id to set.
 */
void musicus_playlist_view_set_active_id(MusicusPlaylistView *pl, gint active_id);

/**
 * Debug routine dumps all fields of `pl`.\n
 * This function can be used as GSourceFunc.
 * \param pl the PlaylistView to dump the data of
 */
gboolean musicus_playlist_view_dump_data (MusicusPlaylistView *pl);

/*@}*/

#endif

/* vim:sts=4:shiftwidth=4
 */
