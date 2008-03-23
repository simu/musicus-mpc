/*
 * musicus_playlist.c - playlist widget for musicus mpc: implementation
 */

#include "musicus_playlist.h"

struct _MusicusPlaylistPrivate {
    GList *songs;
    gint active_id;
    /* for destruction purposes */
    gboolean dispose_has_run;
};

static GObjectClass *parent_class = NULL;

/* internal methods */
static void musicus_playlist_class_init (MusicusPlaylistClass *klass);
static void musicus_playlist_init (GTypeInstance *instance, gpointer class);
static void musicus_playlist_dispose (GObject *object);
static void musicus_playlist_finalize (GObject *object);
static void free_song (gpointer data, gpointer user_data);

/* Get the GType for musicus playlist objects */
GType musicus_playlist_get_type (void) {
    static GType type = 0;
    if (type == 0) {
	static const GTypeInfo info = {
	    sizeof (MusicusPlaylistClass),
	    NULL, /* base init */
	    NULL, /* base finalize */
	    NULL, /* class init */
	    NULL, /* class finalize */
	    NULL, /* class data */
	    sizeof (MusicusPlaylist),
	    0,
	    musicus_playlist_init, /* instance init */
	};
	type = g_type_register_static (G_TYPE_OBJECT, "MusicusPlaylistType",
				       &info, 0
				      );
    }
    return type;
}

/* Initialize MusicusPlaylistClass */
static void musicus_playlist_class_init (MusicusPlaylistClass *klass) {

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    
    gobject_class->dispose = musicus_playlist_dispose;
    gobject_class->finalize = musicus_playlist_finalize;

    parent_class = g_type_class_peek_parent (klass);


    /* this initializes space for the private structure */
    g_type_class_add_private (klass, sizeof (MusicusPlaylistPrivate));
    return;
}

/* Initialize MusicusPlaylist instances */
static void musicus_playlist_init (GTypeInstance *instance, gpointer class) {
    MusicusPlaylist *self = MUSICUS_PLAYLIST (instance);
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MUSICUS_TYPE_PLAYLIST, MusicusPlaylistPrivate);
    self->priv->songs = NULL;
    self->priv->active_id = -1;
    self->priv->dispose_has_run = FALSE;
    self->priv->dispose_has_run = FALSE;
    return;
}

/* Dispose of a MusicusPlaylist instance */
static void musicus_playlist_dispose (GObject *object) {

    MusicusPlaylist *self = MUSICUS_PLAYLIST(object);

    /* if dispose did run already, return */
    if(self->priv->dispose_has_run) {
	return;
    }

    /* Make sure dispose does not run twice */
    self->priv->dispose_has_run = TRUE;

    /* free song list */
    g_list_foreach(self->priv->songs, (GFunc)free_song, NULL);
    g_list_free(self->priv->songs);

    G_OBJECT_CLASS (parent_class)->dispose (object);

    return;
}

/* Finalize a MusicusPlaylist instance */
static void musicus_playlist_finalize (GObject *object) {

    MusicusPlaylist *self = MUSICUS_PLAYLIST(object);

    G_OBJECT_CLASS(parent_class)->finalize (object);
}

/* Free Playlist Song data */
static void free_song (gpointer data, gpointer user_data) {
    /* free song struct */
    return;
}

/* vim:sts=4:shiftwidth=4
 */
