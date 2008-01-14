/**
 * \file search.c
 * \author Simon Gerber
 * \date 2007-12-15
 * This file contains the implementation of the search widget
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

#include <string.h>

#include "mpd.h"
#include "support.h"
#include "playlist.h"
#include "playlist_gtk.h"
#include "search.h"

/************** local functions **************/
static void mark_type(GtkWidget *widget, gpointer data);
static void mpd_search(GtkWidget *widget, gpointer data);

/************** local variables **************/
static int search_type;

/************** implementation  **************/

GtkWidget *init_search_widget(GtkWidget *parent_window) {

    GtkWidget *widget;
	GtkWidget *entry, *button, *hbox, *radio_group[4];
	gint i;
	const gchar *type[4] = { "Artist", "Album", "Title", "Filename" };
	GList *argument_list = NULL;

	widget = gtk_vbox_new(FALSE, 2);
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	gtk_box_pack_start(GTK_BOX(widget), entry,FALSE,FALSE,0);

	hbox = gtk_hbox_new(TRUE, 2);

	for(i = 0; i < 3; i++) {
		if(i == 0) {
			radio_group[i] = gtk_radio_button_new_with_label(
								NULL,
								type[i]
					);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_group[i]), TRUE);
		}
		else {
			radio_group[i] = gtk_radio_button_new_with_label_from_widget(
								GTK_RADIO_BUTTON(radio_group[0]),
								type[i]
					);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_group[i]), FALSE);
		}
		g_signal_connect(G_OBJECT(radio_group[i]), "toggled",
						 G_CALLBACK(mark_type), (gpointer)i);
		gtk_box_pack_start_defaults(GTK_BOX(hbox), radio_group[i]);
	}
	gtk_box_pack_start(GTK_BOX(widget), hbox,FALSE,FALSE, 0);

	button = gtk_button_new_with_label("Search");

	/* pack relevant widgets (box and entry) for mpd_search()*/
	argument_list = g_list_append(argument_list, entry);
	argument_list = g_list_append(argument_list, widget);
	argument_list = g_list_append(argument_list, parent_window);

	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(mpd_search), argument_list);
	g_signal_connect(G_OBJECT(entry), "activate",
					 G_CALLBACK(mpd_search), argument_list);
	gtk_box_pack_start(GTK_BOX(widget), button,FALSE,FALSE,0);

	return widget;
}

/* marks type of the search */
static void mark_type(GtkWidget *widget, gpointer data) {
	search_type = (int)data;
	return;
}

/* searches in the mpd database for a given string and outputs the result (data contains a GtkEntry and a GtkWindow)*/
static void mpd_search(GtkWidget *widget, gpointer data) {

	GList *args,*vbox_content,*argument_list;
	gpointer list_data;
	GtkWidget *vbox, *entry, *label, *result_tree_view, *add_button, *scrolled_window, *window;
   	GtkBox *box;
	GtkBoxChild *child;
	const gchar *entry_string;
	gchar *search_string;
	GtkListStore *list_store;
	GtkTreeSelection *tree_selection;
	GtkObject *h_adjustment, *v_adjustment;

	MpdData *result, *res_ptr;
	int result_count = 0;
	int i = 0;

	GList *song_handle_list;
	MpdPlContainer *result_list;

   	args = (GList *)data;


	/* remove old results, if any */
	vbox = g_list_nth_data(args, 1);
	box = GTK_BOX(vbox);
	vbox_content = box->children;
	list_data = g_list_nth_data(vbox_content, 4);
	if(list_data != NULL) {
		child = list_data;
		gtk_widget_destroy(child->widget);
	}
	list_data = g_list_nth_data(vbox_content, 3);
	if(list_data != NULL) {
		child = list_data;
		gtk_widget_destroy(child->widget);
	}

	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): Type = %i\n", __FILE__, __LINE__, __FUNCTION__, search_type);
		fflush(log_file);
	}

	/* search for the typed string */
	entry = g_list_nth_data(args, 0);
	entry_string = gtk_entry_get_text(GTK_ENTRY(entry));
	search_string = g_strdup(entry_string);
	/* table ^= search_type */
	if(!mpd_check_connected(mpd_info.obj)) {
		if(mpd_connect(mpd_info.obj)!=MPD_OK)  {
		    msi_clear(&mpd_info);
		    mpd_info.msi.connected = FALSE;
		    return;
		}
		msi_fill(&mpd_info);
		mpd_info.msi.connected = TRUE;
	}
	/*
	 * Last argument: TRUE -> only exact matches 
	 *		  FALSE -> more matches 
	 */
	result = mpd_database_find(mpd_info.obj, search_type, search_string, FALSE);
	/* Iterate through the found songs, using
	 * the function mpd_data_get_next_keep(md), which
	 * calls mpd_data_get_next_real(md,FALSE) which
	 * prevents the list being killed when reaching
	 * the end.
	 */
	for(
		res_ptr =  mpd_data_get_first(result);
		res_ptr != NULL;
		res_ptr =  mpd_data_get_next_keep(res_ptr)
	   )
	{
		/* count only songs ... */
		if(res_ptr->type == MPD_DATA_TYPE_SONG)
			++result_count;
	}

	/* no matches */
	if(result_count == 0) {
		label = gtk_label_new("No songs found!");
		gtk_box_pack_end_defaults(GTK_BOX(vbox), label);
		gtk_widget_show(label);
	}
	/* output found songs */
	else {
		song_handle_list = NULL;
		result_list = mpd_pl_container_new_alloc(result_count);
		//mpd_Song *result_list[result_count];

		/* as above: the song info is still needed, as
		 * the mpd_Song objects are *not* copied into the
		 * result list
		 */

		for(
			res_ptr =  mpd_data_get_first(result);
			res_ptr != NULL;
			res_ptr =  mpd_data_get_next_keep(res_ptr)
		   )
		{
			/* only songs */
			if(res_ptr->type == MPD_DATA_TYPE_SONG) {
				result_list->list[i++] = mpd_songDup(res_ptr->song);
				song_handle_list = g_list_append(song_handle_list, strdup(res_ptr->song->file));
			}
		}


		list_store = pl_create_list_store(result_list);
		mpd_pl_container_free(result_list);
		result_tree_view = pl_create_tree_view(GTK_TREE_MODEL(list_store));
		/* create a facility to select only some of the found songs for adding...*/
		tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(result_tree_view));
		gtk_tree_selection_set_mode(tree_selection, GTK_SELECTION_MULTIPLE);

		/**********/
		argument_list = NULL;
		argument_list = g_list_append(argument_list, song_handle_list);
		argument_list = g_list_append(argument_list, tree_selection);
		add_button = gtk_button_new_with_label("add selected songs");
		g_signal_connect(GTK_BUTTON(add_button), "clicked",
						 G_CALLBACK(mpd_add_song_list), argument_list);
		/* g_signal_connect_swapped(GTK_BUTTON(add_button), "clicked",
						 G_CALLBACK(gtk_widget_destroy), g_list_nth_data(data, 2)); */
		gtk_box_pack_end(GTK_BOX(vbox), add_button,FALSE,FALSE,0);

		/* put a scrolled window in between the treeview and the window... */
		/* I just don't know how this works exactly, but the result is acceptable, so why bother */
		h_adjustment = gtk_adjustment_new(0, 0, 4, 1, 4, 4);
		v_adjustment = gtk_adjustment_new(0, 0, 20, 1, 20, 20);

		scrolled_window = gtk_scrolled_window_new(GTK_ADJUSTMENT(h_adjustment), GTK_ADJUSTMENT(v_adjustment));
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
		/* display scrollbars only when needed */
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

		gtk_container_add(GTK_CONTAINER(scrolled_window), result_tree_view);

		gtk_box_pack_end_defaults(GTK_BOX(vbox), scrolled_window);
		gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);

		gtk_widget_show_all(scrolled_window);
		gtk_widget_show_all(add_button);
		window = g_list_nth_data(args, 2);
		gtk_window_resize(GTK_WINDOW(window), 700, 500);
	}

	/* delete search result, have to do this ourselves
	 * because we needed the song data until now
	 */
	mpd_data_free(result);

	return;
}


/* vim:sts=4:shiftwidth=4
 */
