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
 * along with Musicus.  If not, see <http://www.gnu.param/licenses/>.
 **********************************************************************/

#ifndef MUSICUS_SONG_H
#define MUSICUS_SONG_H

#include <gtk/gtk.h>

/**\defgroup 201song MusicusSong
 * Song object tied into GObject type system.
 */

/*@{*/

/**
 * This gives the GType for MusicusSong.
 * \return GType for MusicusSong
 */
#define MUSICUS_SONG_TYPE (musicus_song_get_type())

/**
 * This tries to cast a pointer to type MusicusSong*
 * \param obj The pointer to convert.
 * \return the converted pointer.
 */
#define MUSICUS_SONG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUSICUS_SONG_TYPE, MusicusSong))

/**
 * This tries to cast a class pointer to MusicusSongClass.
 * \param klass the class to convert.
 * \return the converted pointer.
 */
#define MUSICUS_SONG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MUSICUS_SONG_TYPE, MusicusSongClass))

/**
 * This checks if `obj` is of type MusicusSong.
 * \param obj the pointer to check.
 * \return TRUE if `obj` is an instance of MusicusSong. FALSE and emits a warning otherwise. 
 */
#define MUSICUS_IS_SONG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUSICUS_SONG_TYPE,))

/**
 * This checks if `klass` is of type MusicusSongClass.
 * \param klass the pointer to check.
 * \return TRUE if `klass` is an instance of MusicusSongClass. FALSE and emits a warning otherwise. 
 */
#define MUSICUS_IS_SONG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MUSICUS_SONG_TYPE))

/**
 * This returns the class of `obj`.
 * \param obj the instance to get the class of.
 * \return a MusicusSongClass.
 */
#define MUSICUS_SONG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MUSICUS_SONG_GET_TYPE, MusicusSongClass))

typedef struct _MusicusSong MusicusSong;
typedef struct _MusicusSongClass MusicusSongClass;
typedef struct _MusicusSongPrivate MusicusSongPrivate;

/**
 * The main structure, used for client accessible fields.
 * Has none of those right now.
 */
struct _MusicusSong {
    /// reference to parent object.
    GObject parent;

    /// private fields.
    MusicusSongPrivate *priv;
};

/**
 * The class structure.
 */
struct _MusicusSongClass {
    /// parent's class.
    GObjectClass parent;
};

/**
 * Create a new MusicusSong object.
 * \return a new MusicusSong object.
 */
MusicusSong *musicus_song_new(void);

/**
 * Create a new MusicusSong object and supply a song name for it.
 * \param song_name the name for the new MusicusSong
 * \return a MusicusSong object with the supplied name.
 */
MusicusSong *musicus_song_new_with_name(const gchar *song_name);

/**
 * Get the name of a song.
 * \param song the MusicusSong of which you want the song name.
 * \return the song name.
 */
const gchar *musicus_song_get_song_name(MusicusSong *song);

/**
 * Set tha name of a song.
 * \param song the song for which to set the name.
 * \param song_name the song name.
 */
void musicus_song_set_song_name(MusicusSong *song, const gchar *song_name);

/**
 * Wrapper for musicus_song_dispose.
 * \param song the song to free.
 * \param user_data not used.
 */
void musicus_song_free(MusicusSong *song, gpointer user_data);

/**
 * Gets the GType for MusicusSong\n
 * <b>Do not call from user applications directly. Always use MUSICUS_SONG_TYPE.</b>
 * \return the GType for MusicusSong.
 */
GType musicus_song_get_type(void);

/*@}*/

#endif

/* vim:sts=4:shiftwidth=4
 */
