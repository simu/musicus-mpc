/*
 * musicus_playlist.h - playlist widget for musicus mpc: header
 *
 * Copyright ...
 */

#ifndef MUSICUS_PLAYLIST_H
#define MUSICUS_PLAYLIST_H

#include <gtk/gtk.h>

#define MUSICUS_PLAYLIST(w) ((MusicusPlaylist*)w)

#define MUSICUS_TYPE_PLAYLIST (musicus_playlist_get_type())

typedef struct _MusicusPlaylist MusicusPlaylist;
typedef struct _MusicusPlaylistClass MusicusPlaylistClass;
typedef struct _MusicusPlaylistPrivate MusicusPlaylistPrivate;

struct _MusicusPlaylist {
    GObject parent;

    /*< private >*/
    MusicusPlaylistPrivate *priv;
};

struct _MusicusPlaylistClass {
    GObjectClass parent;
};

GType musicus_playlist_get_type(void);

GList *musicus_playlist_get_songs(GtkWidget *pl);
void musicus_playlist_set_songs(GtkWidget *pl, GList *songs);

gint musicus_playlist_get_active(GtkWidget *pl);
void musicus_playlist_set_active(GtkWidget *pl, gint active);

#endif

/* vim:sts=4:shiftwidth=4
 */
