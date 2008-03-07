/**
 * \file playlist_gtk.c
 * \author Simon Gerber
 * \date 2007-12-15
 * This file provides the GTK+ part for the playlist.
 */

/**********************************************************************
 * Copyright 2006, 2007 by Simon Gerber <simugerber@student.ethz.ch>
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

#include <gdk/gdkkeysyms.h>

#include "playlist_gtk.h"
#include "support.h"
#include "mpd.h"

/************** local functions **************/
static void pl_row_activated(GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data);
static void pl_key_pressed(GtkWidget *widget, GdkEventKey *key, gpointer data);

/************** local variables **************/
static GtkWidget *PlTreeView;

/************** implementation  **************/

/* initalize playlist widget */
GtkWidget *init_pl_widget(void) {

	GtkWidget *pl_widget, *pl_tree_view, *scrolled_window;
	GtkTreeStore *pl_tree_store;
	GtkObject *h_adjustment, *v_adjustment;
    GtkTreeModel *tree_model;
    GtkTreeSelection *tree_selection;

	MpdPlContainer *plc;

	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): before mpd_get_pl()\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}

	plc = mpd_get_pl();

	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): before pl_new()\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	/* create the tree view with the mpd playlist and set appropriate global references */
	if(plc == NULL) pl_tree_store = pl_empty_tree_store();
	else {
	    pl_tree_store = pl_create_tree_store(plc);
	    mpd_pl_container_free(plc);
	}
	PlTreeStore = pl_tree_store;
	pl_tree_view = pl_create_tree_view(GTK_TREE_MODEL(pl_tree_store));
	PlTreeView = pl_tree_view;
	tree_model = gtk_tree_view_get_model(GTK_TREE_VIEW(pl_tree_view));
	tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(pl_tree_view));
	/* set up for double-click -> plays selected song */
	gtk_tree_selection_set_mode(tree_selection, GTK_SELECTION_MULTIPLE);
	g_signal_connect(G_OBJECT(pl_tree_view), "row-activated",
					 G_CALLBACK(pl_row_activated), GTK_TREE_SELECTION(tree_selection));
	g_signal_connect(G_OBJECT(pl_tree_view), "key-press-event",
					 G_CALLBACK(pl_key_pressed), (gpointer)tree_selection);

	if (debug) {
		fprintf(log_file, "[%s:%3i] %s(): before adding scrolled window\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	/* put a scrolled window in between the treeview and the window... */
	/* I just don't know how this works exactly, but the result is acceptable, so why bother */
	h_adjustment = gtk_adjustment_new(0, 0, 4, 1, 4, 4);
	v_adjustment = gtk_adjustment_new(0, 0, 20, 1, 20, 20);

	scrolled_window = gtk_scrolled_window_new(GTK_ADJUSTMENT(h_adjustment), GTK_ADJUSTMENT(v_adjustment));
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
	/* display scrollbars only when needed */
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	gtk_container_add(GTK_CONTAINER(scrolled_window), pl_tree_view);
	pl_widget = scrolled_window;

	return pl_widget;
}

/* update playlist tree view */
void playlist_update_tree(void) {

	int old_debug;

	MpdPlContainer *plc;

	/* no local debug */
   	old_debug = debug; debug = 0;
	if (debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	/* get new playlist */
	plc = mpd_get_pl();

	/* only do this if a playlist window exists... */
	if((PlTreeStore != NULL) && (PlTreeView != NULL)) {
		/* reset and refill PlTreeStore and PlTreeView */
		gtk_tree_store_clear(PlTreeStore);
		if(plc == NULL) PlTreeStore = pl_empty_tree_store();
		else {
		    PlTreeStore = pl_create_tree_store(plc);
		    mpd_pl_container_free(plc);
		}

		gtk_tree_view_set_model(GTK_TREE_VIEW(PlTreeView), GTK_TREE_MODEL(PlTreeStore));
	}

	/* revert debug state */
	debug = old_debug;
	return;
}

/* delete songs from playlist */
void pl_del_songs(GtkWidget *widget, gpointer data) {
	GtkTreeIter iter;
	int i, selection_length;
	gint *pl_id;
	GList *selection;
	GtkTreeModel *tree_model;

	pl_id = (gint *)-32;

	selection = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(data), &tree_model);
	selection_length = g_list_length(selection);
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): selection length = %i\n", __FILE__, __LINE__, __FUNCTION__, selection_length);
		fflush(log_file);
	}

    if(mpd_info.msi.connected) {
        if(!mpd_check_connected(mpd_info.obj))
            if(mpd_connect(mpd_info.obj) != MPD_OK) {
		msi_clear(&mpd_info);
		mpd_info.msi.connected = FALSE;
		return;
	    }
	    msi_fill(&mpd_info);
	    mpd_info.msi.connected = TRUE;
    }
	for(i=0; i<selection_length; i++) {
		gtk_tree_model_get_iter(tree_model, &iter, selection->data);
		gtk_tree_model_get(tree_model, &iter, COLUMN_PL_ID, &pl_id, -1);
		if(debug) {
			fprintf(log_file, "[%s:%3i] %s(): song id = %i\n", __FILE__, __LINE__, __FUNCTION__, (int)pl_id);
			fflush(log_file);
		}
		mpd_playlist_queue_delete_id(mpd_info.obj, (int)pl_id);
		selection = g_list_next(selection);
	}
	mpd_playlist_queue_commit(mpd_info.obj);
	fprintf(log_file, "[%s:%3i] %s(): finished deleting songs\n", __FILE__, __LINE__, __FUNCTION__);
	g_list_foreach (selection, (GFunc)gtk_tree_path_free, NULL);
	g_list_free (selection);
	return;
}

/**
 * Add media resource locator to playlist (e.g. web stream
 * \param widget the widget from which the function was called
 * \param data user data
 */
void mpd_mrl_add(MpdObj *obj, const gchar *mrl) {
    mpd_playlist_add(obj, (gchar *)mrl);
    return;
}

/* adds found songs to the playlist */
void mpd_add_song_list(GtkWidget *widget, gpointer data) {

    GList *argument_list = (GList *)data;
    GList *song_list = NULL;
    GtkTreeIter iter;
    int list_length = 0, i;
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GList *selected_rows;
    gchar *file_name;

    if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}

	selection = g_list_nth_data(argument_list, 1);
	selected_rows = gtk_tree_selection_get_selected_rows(selection, &model);

	/* anything selected ? */
	if(g_list_length(selected_rows) > 0) {

		if(debug) {
			fprintf(log_file, "[%s:%3i] %s(): %i songs selected\n", __FILE__, __LINE__, __FUNCTION__, g_list_length(selected_rows));
			fflush(log_file);
		}
		selected_rows = g_list_first(selected_rows);
		while(selected_rows != NULL) {
			gtk_tree_model_get_iter(model, &iter, selected_rows->data);

			gtk_tree_model_get(model, &iter, COLUMN_FILE, &file_name, -1);
			if(debug) {
				fprintf(log_file, "[%s:%3i] %s(): file_name = %s\n", __FILE__, __LINE__, __FUNCTION__, file_name);
				fflush(log_file);
			}
			song_list = g_list_append(song_list, file_name);
			/* g_free(file_name); */
			selected_rows = g_list_next(selected_rows);
		}
		list_length = g_list_length(song_list);
		/* free selected rows list */
		g_list_foreach (selected_rows, (GFunc)gtk_tree_path_free, NULL);
		g_list_free(selected_rows);
	}
	/* nothing selected, add all found songs; maybe not the best way, but never mind */
	else {
		song_list = g_list_nth_data(argument_list, 0);
		list_length = g_list_length(song_list);
		if(debug) {
			fprintf(log_file, "[%s:%3i] %s(): list_length = %i\n", __FILE__, __LINE__, __FUNCTION__, list_length);
			fflush(log_file);
		}
	}

	/* add songs */

	for(i = 0; i < list_length; i++) {
		if(debug) {
			fprintf(log_file, "[%s:%3i] %s(): song to add = %s\n", __FILE__, __LINE__, __FUNCTION__, (char *)song_list->data);
			fflush(log_file);
		}
		mpd_playlist_queue_add(mpd_info.obj, song_list->data);
		song_list = g_list_next(song_list);
	}

	mpd_playlist_queue_commit(mpd_info.obj);


	/* free song list *
	g_list_foreach(song_list, (GFunc)g_free, NULL);
	g_list_free(song_list); */

	return;
}

/* add directory (gchar*)data to playlist */
void mpd_add_directory_to_playlist(GtkWidget *widget, gpointer data) {
	mpd_playlist_add(mpd_info.obj, (char*)data);
	return;
}

/*********** local functions **********/

/* callback if a row of the playlist is double-clicked */
static void pl_row_activated(GtkTreeView *tv, GtkTreePath *path, GtkTreeViewColumn *column, gpointer data) {

	GtkTreeIter iter;
	GtkTreeModel *model;
	gint pl_id;

	pl_id = -32;

	model = gtk_tree_view_get_model(tv);
	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, COLUMN_PL_ID, &pl_id, -1);

	if(pl_id > -32) {
	    // if auto connect is set, try to reconnect
	    if(mpd_info.msi.connected) {
	        if(!mpd_check_connected(mpd_info.obj)) {
		    if(mpd_connect(mpd_info.obj) != MPD_OK) {
			msi_clear(&mpd_info);
			mpd_info.msi.connected = FALSE;
			return;
		    }
		    msi_fill(&mpd_info);
		    mpd_info.msi.connected = TRUE;
		}
            mpd_player_play_id(mpd_info.obj, pl_id);
	    }
	}
	fprintf(log_file, "[%s:%3i] %s(): pl_id = %i\n", __FILE__, __LINE__, __FUNCTION__, pl_id);
	fflush(log_file);

	return;
}

/* handle key press on playlist */
static void pl_key_pressed(GtkWidget *widget, GdkEventKey *key, gpointer data) {
	if(key->keyval == GDK_Delete)
		pl_del_songs(NULL, data);
	return;
}

/* vim:sts=4:shiftwidth=4
 */
