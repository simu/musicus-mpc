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

#define MUSICUS_PLAYLIST_VIEW_TYPE (musicus_playlist_view_get_type())
#define MUSICUS_PLAYLIST_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUSICUS_PLAYLIST_VIEW_TYPE, MusicusPlaylistView))
#define MUSICUS_PLAYLIST_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MUSICUS_PLAYLIST_VIEW_TYPE, MusicusPlaylistViewClass))
#define MUSICUS_IS_PLAYLIST_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUSICUS_PLAYLIST_VIEW_TYPE,))
#define MUSICUS_IS_PLAYLIST_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MUSICUS_PLAYLIST_VIEW_TYPE))
#define MUSICUS_PLAYLIST_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MUSICUS_PLAYLIST_VIEW_GET_TYPE, MusicusPlaylistViewClass))

typedef struct _MusicusPlaylistView MusicusPlaylistView;
typedef struct _MusicusPlaylistViewClass MusicusPlaylistViewClass;
typedef struct _MusicusPlaylistViewPrivate MusicusPlaylistViewPrivate;

struct _MusicusPlaylistView {
    GtkTreeView parent;

    /*< private >*/
    MusicusPlaylistViewPrivate *priv;
};

struct _MusicusPlaylistViewClass {
    GtkTreeViewClass parent;
};

GType musicus_playlist_view_get_type(void);

MusicusPlaylistView *musicus_playlist_view_new();

void musicus_playlist_view_append_song(MusicusPlaylistView *pl, MusicusSong *song);

GList *musicus_playlist_view_get_songs(MusicusPlaylistView *pl);
void musicus_playlist_view_set_songs(MusicusPlaylistView *pl, GList *songs);

void musicus_playlist_view_build_view_from_songs(MusicusPlaylistView *pl);
void musicus_playlist_view_update_view(MusicusPlaylistView *pl);

gint musicus_playlist_view_get_active_id(MusicusPlaylistView *pl);
void musicus_playlist_view_set_active_id(MusicusPlaylistView *pl, gint active_id);

gboolean musicus_playlist_view_dump_data (MusicusPlaylistView *pl);

#endif

/* vim:sts=4:shiftwidth=4
 */
