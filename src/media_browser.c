/**
 * \file media_browser.c
 * \author Simon Gerber
 * \date 2007-12-09
 * This file contains the media browser functions
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
#include <stdlib.h>

#include "media_browser.h"

#include "playlist_gtk.h"
#include "support.h"

/************** local functions **************/
static gboolean read_directory_to_tree_store(char *dirname, gchar *parentdir);
static gboolean media_browser_window_new(gchar *folder_id);
static void media_browser_row_activated_cb(GtkTreeView *tv, GtkTreePath *p, GtkTreeViewColumn *tv_column, gpointer data);
static gboolean media_browser_button_pressed_cb(GtkWidget *widget, GdkEventButton *event_btn, gpointer data);
static void nav_button_cb(GtkWidget *widget, gpointer data);

/************** local variables **************/
/** callback id for "row-activated" */
gulong row_activated_cb_id;
/** stacks for directory change */
GQueue *dir_stack = NULL;
GQueue *parent_stack = NULL;
/* tree view widget */
GtkWidget *mb_TreeView;
/* navigation buttons */
GtkWidget *nav_ButtonBox;

/************** implementation  **************/

/**
 * reads directory `dirname' to media_database, adds entry
 * for parent directory if `parentdir' is not empty
 * @param dirname name of directory to read
 * @param parentdir name of parent directory
 * @return TRUE if no errors occured
 */
static gboolean read_directory_to_tree_store(char *dirname, char *parentdir) {
	/*< require */
	g_return_val_if_fail (dirname != NULL, FALSE);
	g_return_val_if_fail (parentdir != NULL, FALSE);
	/*> */
	/* GtkTreeStore *store = gtk_tree_store_new(M_COLUMNS, G_TYPE_STRING, G_TYPE_STRING); */
	media_database = gtk_tree_store_new(M_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
	GtkTreeIter iter;
	char identifier[256] = { "" };
	MpdData *mpd_data_dir = mpd_database_get_directory(mpd_info.obj, dirname);
	MpdData *tmp = mpd_data_dir;
	/* add an upwards node if not root directory */
	if(parentdir[0]!=0) {
		gtk_tree_store_append(media_database,&iter, NULL);
		snprintf(identifier,256,"[d] ..");
		gtk_tree_store_set(media_database,&iter,M_COLUMN_IDENTIFIER,identifier,
												M_COLUMN_FILE,parentdir,-1);
	}
	for(tmp=mpd_data_dir;tmp!=NULL;tmp=mpd_data_get_next(tmp)) {
		gtk_tree_store_append(media_database, &iter, NULL);
		switch(tmp->type) {
			case MPD_DATA_TYPE_DIRECTORY:
				snprintf(identifier, 256, "[d] %s", tmp->directory);
				gtk_tree_store_set(media_database,&iter,M_COLUMN_IDENTIFIER, identifier,
											   M_COLUMN_FILE, tmp->directory, -1);
				break;
			case MPD_DATA_TYPE_PLAYLIST:
				snprintf(identifier, 256, "[p] %s", tmp->playlist);
				gtk_tree_store_set(media_database,&iter,M_COLUMN_IDENTIFIER, identifier,
											   M_COLUMN_FILE, tmp->playlist, -1);
				break;
			case MPD_DATA_TYPE_SONG:
				snprintf(identifier, 256, "[s] %s - %s", tmp->song->artist, tmp->song->title);
				gtk_tree_store_set(media_database,&iter,M_COLUMN_IDENTIFIER, identifier,
											   M_COLUMN_FILE, tmp->song->file, -1);
				break;
			default: fprintf(err_file, "%s(): something bad happened\n", __FUNCTION__);
					 g_assert_not_reached();
		}
	}
	if(mpd_check_error(mpd_info.obj)) {
		return FALSE;
	}
	else {
		return TRUE;
	}
}

/**
 * Creates the new media_browser window
 * @return what media_browser_window_new() returns
 */
gboolean media_browser_new() {
	gboolean ret = media_browser_window_new("/");
	return ret;
}


/**
 * Builds the media_browser_window
 * @param folder_id the name of the folder to display
 * @return TRUE if no errors
 */
static gboolean media_browser_window_new(gchar *folder_id) {
	/*< require */
	g_return_val_if_fail (folder_id != NULL,FALSE);
	/*> */
	gboolean ret = FALSE;
	GtkWidget *media_browser_widget = init_mb_widget(folder_id);
	if(media_browser_widget != NULL) {
		MbWin.win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_default_size(GTK_WINDOW(MbWin.win), 700,500);
		gtk_window_set_title(GTK_WINDOW(MbWin.win), folder_id);
		/* gtk_window_set_position(GTK_WINDOW(media_browser), GTK_WIN_POS_CENTER_ON_PARENT);
		gtk_window_set_transient_for(GTK_WINDOW(media_browser), GTK_WINDOW(PlWin)); */
		g_signal_connect(G_OBJECT(MbWin.win), "destroy",
						 G_CALLBACK(media_browser_window_hide), NULL);
		g_signal_connect(G_OBJECT(MbWin.win), "delete-event",
						 G_CALLBACK(media_browser_window_hide), NULL);
		gtk_container_add(GTK_CONTAINER(MbWin.win),media_browser_widget);
		gtk_widget_show_all(MbWin.win);
		gtk_widget_hide(MbWin.win);
		ret = TRUE;
	}
	else {
		fprintf(err_file, "[%s:%3i] %s(): error: media_browser_widget == NULL\n", __FILE__,__LINE__,__FUNCTION__);
		fflush(err_file);
	}

	return ret;
}

GtkWidget *init_mb_widget(gchar *folder_id) {
	/*< require */
	g_return_val_if_fail (folder_id != NULL,FALSE);
	/*> */
	GtkWidget *mb_widget = NULL;
	if(read_directory_to_tree_store(folder_id,"")) {
		dir_stack = g_queue_new();
		parent_stack = g_queue_new();
		g_queue_push_head(dir_stack, media_database);
		g_queue_push_head(parent_stack, "");
		GtkWidget *tv = gtk_tree_view_new_with_model(GTK_TREE_MODEL(media_database));
		mb_TreeView = tv;
		GtkTreeViewColumn *col;
		GtkCellRenderer *r;
		GtkObject *vadjustment = gtk_adjustment_new(0,0,20,1,20,20), *hadjustment = gtk_adjustment_new(0,0,4,1,4,4);
		GtkWidget *scrolled_window = gtk_scrolled_window_new(GTK_ADJUSTMENT(hadjustment), GTK_ADJUSTMENT(vadjustment));
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
		int i = 0;
		r = gtk_cell_renderer_text_new();
		for(i = 1; i < M_COLUMNS; i++) {
			if(i == M_COLUMN_IDENTIFIER)
				col = gtk_tree_view_column_new_with_attributes("Identifier", r, "text", i, NULL);
			gtk_tree_view_append_column(GTK_TREE_VIEW(tv), col);
		}
		gtk_tree_view_set_vadjustment(GTK_TREE_VIEW(tv), GTK_ADJUSTMENT(vadjustment));
		row_activated_cb_id = g_signal_connect(G_OBJECT(tv), "row-activated",
						 G_CALLBACK(media_browser_row_activated_cb), "/");
		g_signal_connect(G_OBJECT(tv), "button-press-event",
						 G_CALLBACK(media_browser_button_pressed_cb), NULL);
		gtk_container_add(GTK_CONTAINER(scrolled_window), tv);
		/* create vbox: */
		GtkWidget *box = gtk_vbox_new(FALSE,2);
		GtkWidget *button_box = gtk_hbox_new(FALSE, 2);
		nav_ButtonBox = button_box;
		GtkWidget *button = gtk_button_new_with_label("/");
		g_signal_connect(G_OBJECT(button), "clicked",
						 G_CALLBACK(nav_button_cb), "/");
		gtk_box_pack_start(GTK_BOX(button_box), button, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box), button_box, FALSE, FALSE, 2);
		gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 2);
		mb_widget = box;
	}
	else
		fprintf(err_file, "%s(): error: couldn't read media database\n", __FUNCTION__);

	return mb_widget;
}

/**
 * Updates media browser widget
 * @param folder_id new folder to display
 * @param parent_id new parent folder
 * @param jumpout TRUE if we are going up in the directory tree
 * @return TRUE if no errors
 */
static gboolean media_browser_widget_update(gchar *folder_id, gchar *parent_id, gboolean jumpout) {
	gboolean ret = FALSE;
	gboolean up = FALSE;
	gchar *folder, *parent;
	if(jumpout) {
		up = TRUE;
		g_queue_pop_head(dir_stack);
		media_database = GTK_TREE_STORE(g_queue_peek_head(dir_stack));
		folder = (gchar*)g_queue_pop_head(parent_stack);
		if(strcmp(folder,"")==0) /* root dir */
			parent = "";
		else
			parent = (gchar*)g_queue_peek_head(parent_stack);
		GList *last = g_list_last(GTK_BOX(nav_ButtonBox)->children);
		GtkBoxChild *last_child = last->data;
		GtkWidget *nav_btn_to_rem = last_child->widget;
		gtk_container_remove(GTK_CONTAINER(nav_ButtonBox), nav_btn_to_rem);
		ret = TRUE;
	}
	else {
		if(read_directory_to_tree_store(folder_id,parent_id)) {
			g_queue_push_head(dir_stack,media_database);
			g_queue_push_head(parent_stack, parent_id);
			folder = folder_id;
			parent = parent_id;
			gchar *button_lbl = g_strrstr(folder_id, "/");
			if(strcmp(folder_id, "/") == 0)
				button_lbl = " /";
			GtkWidget *nav_btn;
			if(button_lbl != NULL)
				nav_btn = gtk_button_new_with_label(++button_lbl);
			else
				nav_btn = gtk_button_new_with_label(folder_id);
			g_signal_connect(G_OBJECT(nav_btn), "clicked",
							 G_CALLBACK(nav_button_cb), folder);
			gtk_box_pack_start(GTK_BOX(nav_ButtonBox), nav_btn, FALSE, FALSE, 0);
			gtk_widget_show(nav_btn);
			ret=TRUE;
		}
		else
			fprintf(err_file, "%s(): couldn't read media database\n", __FUNCTION__);
	}
	//gtk_window_set_title(GTK_WINDOW(media_browser), folder);
	gtk_tree_view_set_model(GTK_TREE_VIEW(mb_TreeView), GTK_TREE_MODEL(media_database));
	if(g_signal_handler_is_connected(G_OBJECT(mb_TreeView), row_activated_cb_id))
		g_signal_handler_disconnect(G_OBJECT(mb_TreeView), row_activated_cb_id);
	row_activated_cb_id = g_signal_connect(G_OBJECT(mb_TreeView), "row-activated",
										G_CALLBACK(media_browser_row_activated_cb), folder);


	return ret;
}

/**
 * shows media browser window and loads position
 */
void media_browser_window_show() {
	if(MbWin.win != NULL)
		gtk_widget_show(MbWin.win);
	else {
		media_browser_new();
		gtk_widget_show(MbWin.win);
	}
	if(MbWin.characteristics.filled) gtk_window_parse_geometry(GTK_WINDOW(MbWin.win), MbWin.characteristics.geom);
	return;
}

/**
 * hides media_browser_window and saves position
 */
void media_browser_window_hide() {
	gint x,y,width,height;

	gtk_window_get_position(GTK_WINDOW(MbWin.win), &x, &y);
	gtk_window_get_size(GTK_WINDOW(MbWin.win), &width, &height);
	snprintf(MbWin.characteristics.geom, 64, "%dx%d+%d+%d", width, height, x,y);
	MbWin.characteristics.filled = TRUE;

	gtk_widget_hide(MbWin.win);
	return;
}

/**
 * rereads the TreeStore
 */
void media_browser_reread() {

	gchar *folder;

	media_database = GTK_TREE_STORE(g_queue_peek_head(dir_stack));
	folder = (gchar *)g_queue_peek_head(parent_stack);

	gtk_tree_view_set_model(GTK_TREE_VIEW(mb_TreeView), GTK_TREE_MODEL(media_database));
	if(g_signal_handler_is_connected(G_OBJECT(mb_TreeView), row_activated_cb_id))
		g_signal_handler_disconnect(G_OBJECT(mb_TreeView), row_activated_cb_id);
	row_activated_cb_id = g_signal_connect(G_OBJECT(mb_TreeView), "row-activated",
										G_CALLBACK(media_browser_row_activated_cb), folder);

	return;
}

/* callbacks */

/**
 * This function is called if a row in the TreeView is double-clicked
 * @param tv the corresponding TreeView
 * @param p the GtkTreePath of the clicked row
 * @param tv_column the clicked GtkTreeViewColumn
 * @param data the new parent directory */
static void media_browser_row_activated_cb(GtkTreeView *tv, GtkTreePath *p, GtkTreeViewColumn *tv_column, gpointer data) {
	gchar *id_data, *f_data;
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(tv);
	gtk_tree_model_get_iter (model, &iter, p);
	gtk_tree_model_get (model, &iter,
						M_COLUMN_IDENTIFIER, &id_data,
						M_COLUMN_FILE, &f_data, -1);
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): identifier = %s | file = %s\n", __FILE__, __LINE__, __FUNCTION__, id_data, f_data);
		fflush(log_file);
	}
	if(id_data[1] == 'd') {
		if(strcmp(id_data, "[d] ..") == 0)
			media_browser_widget_update(f_data, (gchar*)data,TRUE);
		else
			media_browser_widget_update(f_data, (gchar*)data,FALSE);
	}
	else {
		if(verbose) {
			fprintf(log_file, "adding %s to playlist\n", f_data);
			fflush(log_file);
		}
		mpd_playlist_add(mpd_info.obj, f_data);
	}
	return;
}

/**
 * this function is called if a button is pressed in the tree view
 * @param widget the TreeView
 * @param event_btn which button was clicked, which position etc
 * @param data user data
 * @return FALSE, so the mouse click gets propagated to other signal handlers if appropriate
 */
static gboolean media_browser_button_pressed_cb(GtkWidget *widget, GdkEventButton *event_btn, gpointer data) {
	/* on right mouse button */
	if(event_btn->type == GDK_BUTTON_PRESS && event_btn->button == 3) {
		gchar *dirpath;

		/* get the row the user clicked on */
		GtkTreeSelection *selection;
		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));

		if(gtk_tree_selection_count_selected_rows(selection) <= 1) {
			GtkTreePath *path;
			/* get path for the row that was clicked */
			if(gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget),
											 (gint) event_btn->x,
											 (gint) event_btn->y,
											 &path, NULL, NULL, NULL))
			{
				gtk_tree_selection_unselect_all(selection);
				gtk_tree_selection_select_path(selection, path);
				gtk_tree_path_free(path);
			}
		}

		selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
		GtkTreeIter iter;
		GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
		gtk_tree_selection_get_selected(selection, &model, &iter);
		gtk_tree_model_get(model, &iter,
						   M_COLUMN_FILE, &dirpath,
						   -1);

		GtkWidget *menu;
		GtkWidget *menu_item;
		menu = gtk_menu_new();

		menu_item = gtk_menu_item_new_with_label("Add to playlist");
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
		g_signal_connect(G_OBJECT(menu_item), "activate",
						 G_CALLBACK(mpd_add_directory_to_playlist), dirpath);

		gtk_widget_show_all(menu);

		gtk_menu_popup(GTK_MENU(menu),NULL,NULL, NULL,NULL,event_btn->button,gdk_event_get_time((GdkEvent*)event_btn));


		return TRUE;
	}
	return FALSE;
}

/**
 * is called if a navigation button is clicked
 * removes all obsolete nav buttons
 * cleans up dir/parent_stack
 * calls media_browser_widget_update with appropriate
 * folder_id & parent_id
 * TODO:
 * performance in removing buttons is possibly bad ...
 */
static void nav_button_cb(GtkWidget *widget, gpointer data) {
	gchar *folder_id = (gchar*)data;
	gchar *parent_id = g_strdup(folder_id);
	parent_id = g_strrstr(parent_id, "/");
	if(parent_id == NULL)
		parent_id = "/";
	else if(strcmp(parent_id,"/") == 0)
		parent_id = "";
	if(debug) {
		debug_fprintf(log_file, "[%s:%3i] %s(): folder = %s | parent = %s\n", __FILE__,__LINE__,__FUNCTION__, folder_id, parent_id);
	}
	/* determine which buttons to keep */
	GList *nav_buttons_list = GTK_BOX(nav_ButtonBox)->children;
	guint button_count = g_list_length(nav_buttons_list);
	guint j =0;
	GtkBoxChild *bc;
	for(j=0;j<button_count;j++) {
		bc = g_list_nth_data(nav_buttons_list, j);
		GtkButton *btn = GTK_BUTTON(bc->widget);
		printf("%s\n", gtk_button_get_label(btn));
	}
	gchar **dir_list = g_strsplit(folder_id, "/", 0);
	guint dir_count = g_strv_length(dir_list);
	if(debug) {
		debug_fprintf(log_file, "[%s:%3i] %s(): button_count = %i, dir_count = %i\n",__FILE__,__LINE__,__FUNCTION__, button_count, dir_count);
	}
	/* remove buttons that don't matter anymore */
	guint rem_count = 0;
	guint i = 0;
	GtkBoxChild *current  =NULL;
	if(strcmp(folder_id, "/") == 0) {
		/* empty ButtonBox */
		for(i = 0; i < button_count; i++) {
			if(debug)
				debug_fprintf(log_file, "[%s:%3i] %s(): %i th child: ",__FILE__,__LINE__,__FUNCTION__, i);

			current = (g_list_first(nav_buttons_list))->data;
			if(debug)
				debug_fprintf(log_file, "[%s:%3i] %s(): %s\n", __FILE__, __LINE__, __FUNCTION__,  gtk_button_get_label(GTK_BUTTON(current->widget)));

			gtk_container_remove(GTK_CONTAINER(nav_ButtonBox), current->widget);
			nav_buttons_list = GTK_BOX(nav_ButtonBox)->children;
			rem_count ++;
		}
	}
	else {
		for(i = dir_count; i < button_count; i++) {
			if(debug)
				debug_fprintf(log_file, "[%s:%3i] %s(): %i th child: ",__FILE__,__LINE__,__FUNCTION__, i);

			current = g_list_nth_data(nav_buttons_list, dir_count);
			/* if(debug) {
				fprintf(log_file, "[%s:%3i] %s(): %s\n", __FILE__, __LINE__, __FUNCTION__,  gtk_button_get_label(GTK_BUTTON(current->widget)));
				fflush(log_file);
			}*/
			if(debug)
				debug_fprintf(log_file, "[%s:%3i] %s(): %s\n", __FILE__, __LINE__, __FUNCTION__,  gtk_button_get_label(GTK_BUTTON(current->widget)));

			gtk_container_remove(GTK_CONTAINER(nav_ButtonBox), current->widget);
			nav_buttons_list = GTK_BOX(nav_ButtonBox)->children;
			rem_count ++;
			g_queue_pop_head(dir_stack); g_queue_pop_head(parent_stack);
		}
	}
	if(debug)
		debug_fprintf(log_file, "[%s:%3i] %s(): rem_count = %i\n",__FILE__,__LINE__,__FUNCTION__, rem_count);

	media_browser_widget_update(folder_id, parent_id, FALSE);
	return;
}

/* vim:shiftwidth=4:sts=4
 */
