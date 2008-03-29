/**
 * \file musicus_song.h
 * \author Simon Gerber
 * \date 2008-03-23
 * This file contains the definitions for a song GObject
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

#ifndef MUSICUS_SONG_H
#define MUSICUS_SONG_H

#include <gtk/gtk.h>

#define MUSICUS_SONG_TYPE (musicus_song_get_type())
#define MUSICUS_SONG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUSICUS_SONG_TYPE, MusicusSong))
#define MUSICUS_SONG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MUSICUS_SONG_TYPE, MusicusSongClass))
#define MUSICUS_IS_SONG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUSICUS_SONG_TYPE,))
#define MUSICUS_IS_SONG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MUSICUS_SONG_TYPE))
#define MUSICUS_SONG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MUSICUS_SONG_GET_TYPE, MusicusSongClass))

typedef struct _MusicusSong MusicusSong;
typedef struct _MusicusSongClass MusicusSongClass;
typedef struct _MusicusSongPrivate MusicusSongPrivate;

struct _MusicusSong {
    GObject parent;

    MusicusSongPrivate *priv;
};

struct _MusicusSongClass {
    GObjectClass parent;
};

MusicusSong *musicus_song_new(void);
MusicusSong *musicus_song_new_with_name(const gchar *song_name);

const gchar *musicus_song_get_song_name(MusicusSong *song);
void musicus_song_set_song_name(MusicusSong *song, const gchar *song_name);

void musicus_song_free(MusicusSong *song, gpointer user_data);

GType musicus_song_get_type(void);

#endif

/* vim:sts=4:shiftwidth=4
 */
