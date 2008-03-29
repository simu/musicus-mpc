/**
 * \file musicus_song.c
 * \author Simon Gerber
 * \date 2008-03-23
 * This file contains the implementation for a song GObject.
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

#include "musicus_song.h"

/**
 * Private structure for MusicusSong.
 */
struct _MusicusSongPrivate {
    /// name of the song
    GString *song_name;
    /// used to ensure that dispose doesn't run twice or that data is valid.
    gboolean dispose_has_run;
};

static GObjectClass *parent_class = NULL;

/* internal methods */
static void musicus_song_class_init (MusicusSongClass *klass, gpointer class_data);
static void musicus_song_init (GTypeInstance *instance, gpointer class);
static void musicus_song_dispose (GObject *object);
static void musicus_song_finalize (GObject *object);

MusicusSong *musicus_song_new(void) {
    return MUSICUS_SONG (g_object_new (MUSICUS_SONG_TYPE, NULL));
}

MusicusSong *musicus_song_new_with_name(const gchar *song_name) {
    MusicusSong *new = musicus_song_new();
    musicus_song_set_song_name(new, song_name);
    return new;
}

/* get song name for MusicusSong `song` */
const gchar *musicus_song_get_song_name(MusicusSong *song) {
    if(song->priv->dispose_has_run)
	return NULL;
    return song->priv->song_name->str;
}

/* set song name for MusicusSong `song` */
void musicus_song_set_song_name(MusicusSong *song, const gchar *song_name) {
    if(song->priv->dispose_has_run)
	return;
    g_string_printf(song->priv->song_name, "%s", song_name);
}

void musicus_song_free (MusicusSong *song, gpointer user_data) {
    musicus_song_dispose(G_OBJECT(song));
    return;
}

GType musicus_song_get_type(void) {
    static GType type = 0;
    if (type == 0) {
	static const GTypeInfo info = {
	    sizeof (MusicusSongClass),
	    NULL, /* base init */
	    NULL, /* base finalize */
	    (GClassInitFunc)musicus_song_class_init, /* class init */
	    NULL, /* class finalize */
	    NULL, /* class data */
	    sizeof (MusicusSong),
	    0,
	    (GInstanceInitFunc)musicus_song_init, /* instance init */
	};
	type = g_type_register_static (G_TYPE_OBJECT, "MusicusSongType",
				       &info, 0);
    }

    return type;
}

static void musicus_song_class_init (MusicusSongClass *klass, gpointer class_data) {

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = musicus_song_dispose;
    gobject_class->finalize = musicus_song_finalize;

    parent_class = g_type_class_peek_parent (klass);

    g_type_class_add_private (klass, sizeof(MusicusSongPrivate));
    return;
}

static void musicus_song_init (GTypeInstance *instance, gpointer class) {
    MusicusSong *self = MUSICUS_SONG (instance);
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MUSICUS_SONG_TYPE, MusicusSongPrivate);
    self->priv->song_name = g_string_new("");
    self->priv->dispose_has_run = FALSE;
    return;
}

static void musicus_song_dispose (GObject *object) {

    MusicusSong *self = MUSICUS_SONG (object);

    /* if dispose did run already, return */
    if(self->priv->dispose_has_run) {
	return;
    }

    /* Make sure dispose does not run twice */
    self->priv->dispose_has_run = TRUE;

    /* free song_name */
    g_string_free(self->priv->song_name, TRUE);

    G_OBJECT_CLASS (parent_class)->dispose (object);

    return;
}

static void musicus_song_finalize (GObject *object) {

    G_OBJECT_CLASS(parent_class)->finalize (object);

    return;
}

/* vim:sts=4:shiftwidth=4
 */
