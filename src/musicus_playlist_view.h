/*
 * musicus_playlist.h - playlist widget for musicus mpc: header
 *
 * Copyright ...
 */

#ifndef MUSICUS_PLAYLIST_H
#define MUSICUS_PLAYLIST_H

#include <gtk/gtk.h>
#include "musicus_song.h"

#define MUSICUS_PLAYLIST_TYPE (musicus_playlist_get_type())
#define MUSICUS_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUSICUS_PLAYLIST_TYPE, MusicusPlaylist))
#define MUSICUS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), MUSICUS_PLAYLIST_TYPE, MusicusPlaylistClass))
#define MUSICUS_IS_PLAYLIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUSICUS_PLAYLIST_TYPE,))
#define MUSICUS_IS_PLAYLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MUSICUS_PLAYLIST_TYPE))
#define MUSICUS_PLAYLIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), MUSICUS_PLAYLIST_GET_TYPE, MusicusPlaylistClass))

typedef struct _MusicusPlaylist MusicusPlaylist;
typedef struct _MusicusPlaylistClass MusicusPlaylistClass;
typedef struct _MusicusPlaylistPrivate MusicusPlaylistPrivate;

struct _MusicusPlaylist {
    GtkTreeView parent;

    /*< private >*/
    MusicusPlaylistPrivate *priv;
};

struct _MusicusPlaylistClass {
    GtkTreeViewClass parent;
};

GType musicus_playlist_get_type(void);

MusicusPlaylist *musicus_playlist_new();

GList *musicus_playlist_get_songs(MusicusPlaylist *pl);
void musicus_playlist_set_songs(MusicusPlaylist *pl, GList *songs);

void musicus_playlist_build_view_from_songs(MusicusPlaylist *pl);

gint musicus_playlist_get_active_id(MusicusPlaylist *pl);
void musicus_playlist_set_active_id(MusicusPlaylist *pl, gint active_id);

#endif

/* vim:sts=4:shiftwidth=4
 */
