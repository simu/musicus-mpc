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
static void musicus_playlist_class_init (MusicusPlaylistClass *klass, gpointer class_data);
static void musicus_playlist_init (GTypeInstance *instance, gpointer class);
static void musicus_playlist_dispose (GObject *object);
static void musicus_playlist_finalize (GObject *object);

/* get a new MusicusPlaylist instance for the user */
MusicusPlaylist *musicus_playlist_new() {
    return MUSICUS_PLAYLIST (g_object_new (MUSICUS_PLAYLIST_TYPE, NULL));
}

/* append song */
void musicus_playlist_append_song(MusicusPlaylist *pl, MusicusSong *song) {
    if(pl->priv->dispose_has_run)
	return;
    pl->priv->songs = g_list_append (pl->priv->songs, song);
    return;
}

/* get song list */
GList *musicus_playlist_get_songs (MusicusPlaylist *pl) {
    if(pl->priv->dispose_has_run)
	return NULL;
    return pl->priv->songs;
}

/* set song list */
void musicus_playlist_set_songs (MusicusPlaylist *pl, GList *songs) {
    if(pl->priv->dispose_has_run)
	return;
    pl->priv->songs = songs;
    return;
}

/* build tree view from songs */
void musicus_playlist_build_view_from_songs(MusicusPlaylist *pl) {

    GtkTreeViewColumn *c;
    gchar *col[] = { "A", "ID", "Song" };
    gint i;

    /* return if object is no longer valid */
    if(pl->priv->dispose_has_run)
	return;

    musicus_playlist_update_view(pl);

    for(i=0;i<3;++i) {
	if(i == 0)
	    c = gtk_tree_view_column_new_with_attributes(
		    col[i],
		    gtk_cell_renderer_toggle_new(),
		    "active", i,
		    NULL);
	else
	    c = gtk_tree_view_column_new_with_attributes(
		    col[i],
		    gtk_cell_renderer_text_new(),
		    "text", i,
		    NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(pl), c);
    }

    return;
}

/* update tree view */
void musicus_playlist_update_view(MusicusPlaylist *pl) {

    GtkListStore *store;
    GtkTreeIter iter;
    GList *tmp;
    gint i = 0;
    gint active_id;
    gchar id[5];
    gboolean is_active;

    /* return if object no longer valid */
    if(pl->priv->dispose_has_run)
	return;

    store = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW(pl)));
    if(store == NULL)
	store = gtk_list_store_new(3, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING);
    tmp = pl->priv->songs;
    active_id = pl->priv->active_id;

    /* hacky but I don't see a more elegant solution right now
     * TODO: research for gtk_list_store_append or something
     */
    gtk_list_store_clear (store);

    while(tmp != NULL) {
	gtk_list_store_append(store, &iter);
	is_active = (i == active_id) ? TRUE : FALSE;
	snprintf(id,5,"%02d", i++);
	gtk_list_store_set(store, &iter, 0, is_active,
					 1, id,
					 2, musicus_song_get_song_name(MUSICUS_SONG(tmp->data)),
					 -1
			  );
	tmp = tmp->next;
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(pl), GTK_TREE_MODEL(store));

    return;
}

/* get active id */
gint musicus_playlist_get_active_id(MusicusPlaylist *pl) {
    if(pl->priv->dispose_has_run)
	return -2;
    return pl->priv->active_id;
}

/* set active id */
void musicus_playlist_set_active_id(MusicusPlaylist *pl, gint active_id) {
    if(pl->priv->dispose_has_run)
	return;
    pl->priv->active_id = active_id;
    /* update view */
    musicus_playlist_update_view(pl);
    return;
}

/* Get the GType for musicus playlist objects */
GType musicus_playlist_get_type (void) {
    static GType type = 0;
    if (type == 0) {
	static const GTypeInfo info = {
	    sizeof (MusicusPlaylistClass),
	    NULL, /* base init */
	    NULL, /* base finalize */
	    (GClassInitFunc)musicus_playlist_class_init, /* class init */
	    NULL, /* class finalize */
	    NULL, /* class data */
	    sizeof (MusicusPlaylist),
	    0,
	    (GInstanceInitFunc)musicus_playlist_init, /* instance init */
	};
	type = g_type_register_static (GTK_TYPE_TREE_VIEW, "MusicusPlaylistType",
				       &info, 0
				      );
    }
    return type;
}

/* Initialize MusicusPlaylistClass */
static void musicus_playlist_class_init (MusicusPlaylistClass *klass, gpointer class_data) {

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
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MUSICUS_PLAYLIST_TYPE, MusicusPlaylistPrivate);
    self->priv->songs = NULL;
    self->priv->active_id = -1;
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
    g_list_foreach(self->priv->songs, (GFunc)musicus_song_free, NULL);
    g_list_free(self->priv->songs);

    G_OBJECT_CLASS (parent_class)->dispose (object);

    return;
}

/* Finalize a MusicusPlaylist instance */
static void musicus_playlist_finalize (GObject *object) {

    G_OBJECT_CLASS(parent_class)->finalize (object);
}

/* vim:sts=4:shiftwidth=4
 */
