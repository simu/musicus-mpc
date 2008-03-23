/*
 * musicus_song.h - a song object
 */

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

const gchar *musicus_song_get_song_name(MusicusSong *song);

GType musicus_song_get_type(void);

#endif

/* vim:sts=4:shiftwidth=4
 */
