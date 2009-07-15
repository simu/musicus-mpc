/**
 * \file musicus_playlist_view.c
 * \author Simon Gerber
 * \date 2008-03-23
 * This file contains the implementation for a PlaylistView widget.
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

#include "musicus_playlist_view.h"
#include <stdlib.h>
#include <glib/gprintf.h>

struct _MusicusPlaylistViewPrivate {
    /// list for the songs
    GList *songs;
    /// id of the active element
    gint active_id;
    /// used to ensure that dispose doesn't run twice or that data is valid.
    gboolean dispose_has_run;
};

static GObjectClass *parent_class = NULL;

/* internal methods */
static void musicus_playlist_view_class_init (MusicusPlaylistViewClass *klass, gpointer class_data);
static void musicus_playlist_view_init (GTypeInstance *instance, gpointer class);
static void musicus_playlist_view_dispose (GObject *object);
static void musicus_playlist_view_finalize (GObject *object);
void row_activated_cb (GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *c, gpointer data);
void row_inserted_cb (GtkTreeView *tv, GtkTreePath *path, GtkTreeIter *iter, gpointer data);

/* get a new MusicusPlaylistView instance for the user */
MusicusPlaylistView *musicus_playlist_view_new() {
    return MUSICUS_PLAYLIST_VIEW (g_object_new (MUSICUS_PLAYLIST_VIEW_TYPE, NULL));
}

/* append song */
void musicus_playlist_view_append_song(MusicusPlaylistView *pl, MusicusSong *song) {
    if(pl->priv->dispose_has_run)
	return;
    pl->priv->songs = g_list_append (pl->priv->songs, song);
    return;
}

/* get song list */
GList *musicus_playlist_view_get_songs (MusicusPlaylistView *pl) {
    if(pl->priv->dispose_has_run)
	return NULL;
    return pl->priv->songs;
}

/* set song list */
void musicus_playlist_view_set_songs (MusicusPlaylistView *pl, GList *songs) {
    if(pl->priv->dispose_has_run)
	return;
    pl->priv->songs = songs;
    return;
}

/* build tree view from songs */
void musicus_playlist_view_build_view_from_songs(MusicusPlaylistView *pl) {

    GtkTreeViewColumn *c;
    gchar *col[] = { "A", "ID", "Song" };
    gint i;

    /* return if object is no longer valid */
    if(pl->priv->dispose_has_run)
	return;

    musicus_playlist_view_update_view(pl);

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
void musicus_playlist_view_update_view(MusicusPlaylistView *pl) {

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
gint musicus_playlist_view_get_active_id(MusicusPlaylistView *pl) {
    if(pl->priv->dispose_has_run)
	return -2;
    return pl->priv->active_id;
}

/* set active id */
void musicus_playlist_view_set_active_id(MusicusPlaylistView *pl, gint active_id) {
    if(pl->priv->dispose_has_run)
	return;
    pl->priv->active_id = active_id;
    /* update view */
    musicus_playlist_view_update_view(pl);
    return;
}

/* Get the GType for musicus playlist_view objects */
GType musicus_playlist_view_get_type (void) {
    static GType type = 0;
    if (type == 0) {
	static const GTypeInfo info = {
	    sizeof (MusicusPlaylistViewClass),
	    NULL, /* base init */
	    NULL, /* base finalize */
	    (GClassInitFunc)musicus_playlist_view_class_init, /* class init */
	    NULL, /* class finalize */
	    NULL, /* class data */
	    sizeof (MusicusPlaylistView),
	    0,
	    (GInstanceInitFunc)musicus_playlist_view_init, /* instance init */
	};
	type = g_type_register_static (GTK_TYPE_TREE_VIEW, "MusicusPlaylistViewType",
				       &info, 0
				      );
    }
    return type;
}

/* Initialize MusicusPlaylistViewClass */
static void musicus_playlist_view_class_init (MusicusPlaylistViewClass *klass, gpointer class_data) {

    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    
    gobject_class->dispose = musicus_playlist_view_dispose;
    gobject_class->finalize = musicus_playlist_view_finalize;

    parent_class = g_type_class_peek_parent (klass);

    /* this initializes space for the private structure */
    g_type_class_add_private (klass, sizeof (MusicusPlaylistViewPrivate));

    GType *param_types = malloc(sizeof(GType));

    /* initialize signals for MusicusPlaylistView */
    param_types[0] = G_TYPE_UINT;
    g_signal_new("active-changed", MUSICUS_PLAYLIST_VIEW_TYPE,
		 G_SIGNAL_RUN_FIRST, 0, NULL, NULL, NULL, G_TYPE_NONE,
		 1, param_types);
    g_signal_new("order-changed", MUSICUS_PLAYLIST_VIEW_TYPE,
		 G_SIGNAL_RUN_FIRST, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);

    return;
}

/* Initialize MusicusPlaylistView instances */
static void musicus_playlist_view_init (GTypeInstance *instance, gpointer class) {
    GtkListStore *store;
    GtkTreeSelection *tree_selection;
    MusicusPlaylistView *self = MUSICUS_PLAYLIST_VIEW (instance);
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, MUSICUS_PLAYLIST_VIEW_TYPE, MusicusPlaylistViewPrivate);
    self->priv->songs = NULL;
    self->priv->active_id = -1;
    self->priv->dispose_has_run = FALSE;

    store = GTK_LIST_STORE(gtk_tree_view_get_model (GTK_TREE_VIEW(self)));
    /* set-up for drag-n-drop reordering */
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(self), TRUE);
    g_signal_connect(G_OBJECT(store), "row-inserted",
		     G_CALLBACK(row_inserted_cb), NULL);
    tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self));
    g_signal_connect(G_OBJECT(self), "row-activated",
		     G_CALLBACK(row_activated_cb), GTK_TREE_SELECTION(tree_selection));
//    g_signal_connect(G_OBJECT(self), "key-press-event",
//		     G_CALLBACK(self_key_pressed), (gpointer)tree_selection);

    return;
}

/* Dispose of a MusicusPlaylistView instance */
static void musicus_playlist_view_dispose (GObject *object) {

    MusicusPlaylistView *self = MUSICUS_PLAYLIST_VIEW(object);

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

/* Finalize a MusicusPlaylistView instance */
static void musicus_playlist_view_finalize (GObject *object) {

    G_OBJECT_CLASS(parent_class)->finalize (object);
}

gboolean musicus_playlist_view_dump_data (MusicusPlaylistView *pl) {
    g_printf("dispose_has_run = %s\n", pl->priv->dispose_has_run?"TRUE":"FALSE");
    g_printf("active_id = %d\n", pl->priv->active_id);
    g_printf("songs = %p\n", pl->priv->songs);
    g_print("==================\n");
    return TRUE;
}

void row_activated_cb (GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *c, gpointer data) {

    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *id;
    gint activated;

    model = gtk_tree_view_get_model(tv);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 1, &id, -1);

    activated = atoi(id);

    if(musicus_playlist_view_get_active_id(MUSICUS_PLAYLIST_VIEW(tv)) != activated)
	g_signal_emit_by_name(MUSICUS_PLAYLIST_VIEW(tv), "musicus-playlist-view-active-changed", activated);

    musicus_playlist_view_set_active_id(MUSICUS_PLAYLIST_VIEW(tv), activated);

    return;
}

void row_inserted_cb (GtkTreeView *tv, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
}
/* vim:sts=4:shiftwidth=4
 */
