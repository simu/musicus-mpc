/**
 * \file gui.c
 * \author Simon Gerber
 * \date 2007-12-15
 * This file contains the procedures to build the main GUI
 */

 /**********************************************************************
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

#include "gui.h"
#include "playlist_gtk.h"
#include "media_browser.h"
#include "support.h"
#include "mpd.h"
#include "mpd_action.h"
#include "search.h"
#include "preferences.h"

/************** local functions **************/
static GtkWidget *create_menubar(GFunc *quit_proc);
static void open_about_dialog(GtkWidget *widget, gpointer data);

static void update_statusbar(gchar *message, GtkWidget *statusbar);
static void gui_add_mrl_win(GtkWidget *widget, gpointer data);

static gboolean titlebar_updater(gpointer data);
static gboolean auto_change_view(gpointer data);

static void page_switched(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer data);

static void applet_pl_cleanup_cb(GtkWidget *widget, gpointer data);
static void playlist_update_tree_cb(GtkWidget *widget, gpointer data);
static void mpd_connect_cb(GtkWidget *widget, gpointer data);
static void mpd_disconnect_cb(GtkWidget *widget, gpointer data);
static void menu_entries_update(int play_state);
static void mpd_mrl_add_cb(GtkWidget *widget, gpointer data);

/************** local variables **************/
/* to produce a message if not connected to mpd */
static GtkWidget *notConnectedMsg;
static GtkWidget *mpdElements;
static GList *menu_entries;
static gint sb_push_num = 0;
static GtkWidget *StatusBar;
/* for auto_change_view */
static int old_conn_state = -64;
/* update playlist tree? */
static gboolean update_pl_tree = FALSE;

/* labels in statusbar */
static GtkWidget *repeat_label;
static GtkWidget *random_label;
static GtkWidget *xfade_label;

/* database update status */
static gboolean is_updating_db = FALSE;

/************** implementation  **************/

/* create playlist window, run_updater = 1 will run the title updating
 * applet = 1 means that the playlist is called from the panel applet,
 * use 0 otherwise! */
GtkWidget *mpd_win_new(int run_updater, int applet) {

	int internal_debug = 0;
	guint mb_page_id = -1;

	GtkWidget *mpd_win, *hbox, *vbox, *menubar, *statusbar, *label, *frame;
	GtkWidget *panes, *page, *page_label;
	/* GtkWidget *lyrics_expander;
	GtkWidget *lyrics; */
	GFunc *quit_proc;
	gchar label_text[255];

	MpdWin.window_creation_finished = 0;

	/* create the window widget, set its properties */
	mpd_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(mpd_win), 700,500);
	gtk_window_set_title(GTK_WINDOW(mpd_win), "MPD playlist");
	/* get the actual size of the window */
	if(internal_debug) {
		g_signal_connect(G_OBJECT(mpd_win), "size-allocate",
						 G_CALLBACK(print_size), NULL);
	}

	if (debug) {
		fprintf(log_file, "[%s:%3i] %s(): before adding buttons\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	/* add some control buttons */
	vbox = gtk_vbox_new(FALSE,0);
	if(applet == 1)
		quit_proc = (GFunc *)applet_pl_cleanup_cb;
	else
		quit_proc = (GFunc *)quit_procedure;
	menubar = create_menubar(quit_proc);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE,FALSE, 0);

	/*
	 * add panes for playlist, media browser, add
	 */
	hbox = gtk_hbox_new(FALSE, 10);
	notConnectedMsg = gtk_label_new("Not connected to MPD daemon");
	panes = gtk_notebook_new();
	mpdElements = panes;
	/* Playlist: */
	page = init_pl_widget();
	page_label = gtk_label_new("Playlist");
	gtk_notebook_append_page(GTK_NOTEBOOK(panes), page, page_label);
	/* Media Browser */
	page = NULL;
	page = init_mb_widget("/");
	//page = gtk_label_new("Test");
	if(page != NULL) {
		page_label = gtk_label_new("Media Browser");
		mb_page_id = gtk_notebook_append_page(GTK_NOTEBOOK(panes), page, page_label);
	}
	else {
		fprintf(err_file, "[%s:%3i] %s(): error: page == NULL, error in init_mb_widget()\n", __FILE__,__LINE__,__FUNCTION__);
		fflush(err_file);
	}
	// connect signal for the media browser page
	g_signal_connect(G_OBJECT(panes), "switch-page",
					 G_CALLBACK(page_switched), (guint *)mb_page_id);

	/* Search dialog */
	page = NULL;
	page = init_search_widget(mpd_win);
	page_label = gtk_label_new("Search & Add");
	gtk_notebook_append_page(GTK_NOTEBOOK(panes), page, page_label);

	/* song lyrics
	 * TODO:
	 * find out how to prevent
	 * problems on program start,
	 * if server isn't found or similar
	 */
	/* page = lyrics_pane();
	page_label = gtk_label_new("Song lyrics");
	gtk_notebook_append_page(GTK_NOTEBOOK(panes),page,page_label);

	 */
#if 0
	if(!applet) {
		lyrics_expander = gtk_expander_new("Lyrics");
		lyrics = lyrics_pane(lyrics_buf);
		g_signal_connect (G_OBJECT(lyrics_expander), "notify::expanded",
						  G_CALLBACK(lyrics_expander_cb), lyrics);
		//gtk_expander_set_label_widget(GTK_EXPANDER(lyrics_expander), lyrics_pane());
		gtk_container_add(GTK_CONTAINER(lyrics_expander), lyrics);
		//gtk_widget_set_size_request(lyrics_expander, 300, -1);
		gtk_box_pack_start(GTK_BOX(hbox), lyrics_expander, FALSE, FALSE, 0);
	}
#endif

	/* add panes to window */
	gtk_box_pack_start(GTK_BOX(hbox), panes, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), notConnectedMsg, TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

	if (debug) {
		fprintf(log_file, "[%s:%3i] %s(): before statusbar\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	/* statusbar */
	statusbar = gtk_statusbar_new();
	/* repeat label */
	frame = gtk_frame_new(NULL);
	if(mpd_info.mps.repeat)
		label = gtk_label_new("repeat on");
	else
		label = gtk_label_new("repeat off");
	gtk_container_add(GTK_CONTAINER(frame), label);
	/* remember in file scope */
	repeat_label = label;
	gtk_box_set_homogeneous(GTK_BOX(statusbar), FALSE);
	gtk_box_set_spacing(GTK_BOX(statusbar), 0);
	gtk_box_pack_start(GTK_BOX(statusbar), frame, FALSE, FALSE, 1);

    /* random label */
	frame = gtk_frame_new(NULL);
	if(mpd_info.mps.random)
		label = gtk_label_new("random on");
	else
		label = gtk_label_new("random off");
	gtk_container_add(GTK_CONTAINER(frame), label);
	/* remember in file scope */
	random_label = label;
	gtk_box_set_homogeneous(GTK_BOX(statusbar), FALSE);
	gtk_box_set_spacing(GTK_BOX(statusbar), 0);
	gtk_box_pack_start(GTK_BOX(statusbar), frame, FALSE, FALSE, 1);

	/* crossfade label */
	frame = gtk_frame_new(NULL);
	snprintf(label_text, 255, "crossfade %is", mpd_info.mps.xfade);
	label = gtk_label_new(label_text);
	gtk_container_add(GTK_CONTAINER(frame), label);
	/* remember in file scope */
	xfade_label = label;
	gtk_box_set_homogeneous(GTK_BOX(statusbar), FALSE);
	gtk_box_set_spacing(GTK_BOX(statusbar), 0);
	gtk_box_pack_start(GTK_BOX(statusbar), frame, FALSE, FALSE, 1);

	gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);
	StatusBar = statusbar;

	/* finishing up */
	gtk_container_add(GTK_CONTAINER(mpd_win), vbox);

	MpdWin.win = mpd_win;
	/* running auto-update processes (title/statusbar and hide elements when not connected) */
	if(run_updater == 1) {
		if (debug) {
			fprintf(log_file, "[%s:%3i] %s(): before g_timeout_add()\n", __FILE__, __LINE__, __FUNCTION__);
			fflush(log_file);
		}
		mpd_info.update_routines = g_list_append(mpd_info.update_routines, titlebar_updater);
		mpd_info.update_routines = g_list_append(mpd_info.update_routines, auto_change_view);
		g_timeout_add(mpd_info.update_interval, titlebar_updater, (gpointer)mpd_info.update_interval);
		// hide mpd elements when appropriate
		g_timeout_add(mpd_info.update_interval, auto_change_view, (gpointer)mpd_info.update_interval);
	}

	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): before return\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}

	return mpd_win;
}

static GtkWidget *create_menubar(GFunc *quit_proc) {
	GtkWidget *menubar;
	GtkWidget *menu,*menu_item;
	GtkWidget *menu_entry_play, *menu_entry_pause, *menu_entry_conn, *menu_entry_disconn;
/* 	GtkActionGroup *group;
	GtkUIManager *ui_manager; */

	menubar = gtk_menu_bar_new();

	/* action group whatever *
	group = gtk_action_group_new("MainActionGroup");
	gtk_action_group_add_actions(group,entries,NUM_ENTRIES,NULL);
	* UI Manager *
	ui_manager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(ui_manager, group, 0);
	* adjust path !!! *
	gtk_ui_manager_add_ui_from_file(ui_manager, "menu.ui", NULL);
	menubar = gtk_ui_manager_get_widget(ui_manager, "/MenuBar"); */

	/* item: Playlist */
	menu_item = gtk_menu_item_new_with_mnemonic ("_Playlist");
	gtk_container_add(GTK_CONTAINER(menubar), menu_item);

	/* create menubox for the item */
	menu = gtk_menu_new ();
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), menu);

	/* action: add url */
	menu_add_entry(GTK_CONTAINER(menu), "_Add Media Resource Locator", G_CALLBACK(gui_add_mrl_win), NULL);
	/* action: delete */
	menu_add_entry(GTK_CONTAINER(menu), "_Delete Songs", G_CALLBACK(pl_del_songs), NULL);
	/* action: get current song lyrics */
	// menu_add_entry(GTK_CONTAINER(menu), "_Get lyrics", G_CALLBACK(pl_get_current_song_lyrics), NULL);

	/* menu_entry = gtk_menu_item_new_with_mnemonic("Media _Browser");
	gtk_container_add(GTK_CONTAINER(menu), menu_entry);
	g_signal_connect(G_OBJECT(menu_entry), "activate",
					 G_CALLBACK(media_browser_window_show), NULL);
	g_signal_connect(G_OBJECT(menu_entry), "activate",
					 G_CALLBACK(media_database_read), NULL); */

	/* action: separator */
	menu_add_separator(GTK_CONTAINER(menu));
	/* action: reload playlist */
	menu_add_entry(GTK_CONTAINER(menu), "_Reload playlist", G_CALLBACK(playlist_update_tree_cb), NULL);

	/* action: separator */
	menu_add_separator(GTK_CONTAINER(menu));

	/* action: properties dialog */
	menu_add_entry(GTK_CONTAINER(menu), "_Properties", G_CALLBACK(mpd_new_properties_window_cb), (gpointer)MpdWin.win);

	/* action: quit */
#ifdef CLOSE
	menu_add_entry(GTK_CONTAINER(menu), "_Close", G_CALLBACK(quit_proc), MpdWin.win);
#else
	menu_add_entry(GTK_CONTAINER(menu), "_Quit", G_CALLBACK(quit_proc), MpdWin.win);
#endif

	/* item: MPD */
	menu_item = gtk_menu_item_new_with_mnemonic ("_MPD");
	gtk_container_add(GTK_CONTAINER(menubar), menu_item);

	/* create menubox for the item */
	menu = gtk_menu_new ();
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), menu);

	/* action: connect/disconnect */
	menu_entry_disconn = menu_add_entry_stock_icon(GTK_CONTAINER(menu), "_Disconnect", GTK_STOCK_DISCONNECT, G_CALLBACK(mpd_disconnect_cb), NULL);
	menu_entry_conn = menu_add_entry_stock_icon(GTK_CONTAINER(menu), "_Connect", GTK_STOCK_CONNECT, G_CALLBACK(mpd_connect_cb), NULL);

	/* separator */
	menu_add_separator(GTK_CONTAINER(menu));

	/* action: Previous */
	menu_add_entry_stock_icon(GTK_CONTAINER(menu), "P_revious", GTK_STOCK_MEDIA_PREVIOUS, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_PREVIOUS));

	/* action: Play */
	menu_entry_play = menu_add_entry_stock_icon(GTK_CONTAINER(menu), "_Play", GTK_STOCK_MEDIA_PLAY, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_PLAY_PAUSE));

	/* action: Pause */
	menu_entry_pause = menu_add_entry_stock_icon(GTK_CONTAINER(menu), "_Pause", GTK_STOCK_MEDIA_PAUSE, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_PLAY_PAUSE));


	/* action: Stop */
	menu_add_entry_stock_icon(GTK_CONTAINER(menu), "_Stop", GTK_STOCK_MEDIA_STOP, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_STOP));

	/* action: Next */
	menu_add_entry_stock_icon(GTK_CONTAINER(menu), "_Next", GTK_STOCK_MEDIA_NEXT, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_NEXT));

	/* action: separator */
	menu_add_separator(GTK_CONTAINER(menu));

	/* action: Update DB */
	menu_add_entry(GTK_CONTAINER(menu), "_Update DB", G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_UPDATE_DB));

	/* action: Shuffle Playlist */
	menu_add_entry(GTK_CONTAINER(menu), "R_andom", G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_RANDOM));

	/* action: Repeat */
	menu_add_entry(GTK_CONTAINER(menu), "R_epeat", G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_REPEAT));

	/* action: Xfade */
	menu_add_entry(GTK_CONTAINER(menu), "_Xfade", G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_XFADE));

	/* item: Help */
	menu_item = gtk_menu_item_new_with_mnemonic ("_Help");
	gtk_container_add(GTK_CONTAINER(menubar), menu_item);

	/* create menubox for the item */
	menu = gtk_menu_new ();
  	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), menu);

	/* action: log window */
//	menu_add_entry(GTK_CONTAINER(menu), "_Log window", G_CALLBACK(open_log_window), NULL);

	/* action: separator */
	menu_add_separator(GTK_CONTAINER(menu));

	/* action: About */
	menu_add_entry(GTK_CONTAINER(menu), "_About", G_CALLBACK(open_about_dialog), NULL);


	gtk_widget_show_all(menubar);

	menu_entries = NULL;
	menu_entries = g_list_append(menu_entries, menu_entry_play);
	menu_entries = g_list_append(menu_entries, menu_entry_pause);
	menu_entries = g_list_append(menu_entries, menu_entry_conn);
	menu_entries = g_list_append(menu_entries, menu_entry_disconn);

	return menubar;
}

/*display an about dialog */
void open_about_dialog(GtkWidget *widget, gpointer data) {
	gchar *authors[] = {
		AUTHORS,
		NULL
	};
	gchar *documenters[] = {
		DOCUMENTERS,
		NULL
	};

	/* gchar dir[512],*img;
	get_data_dir(dir, 512, FALSE);
	snprintf(img, strlen(dir)+22, "%s/data/gnome-mpd-applet", dir); */
	const gchar *img_c = "gnome-mpd-applet";

	gtk_show_about_dialog (NULL,
#ifdef APPLET
		"name",		"Musicus Applet",
#else
		"name",		"Musicus MPD client",
#endif
		"version",	VERSION,
		"comments",	"Musicus is a client for the Music Player Daemon (http://www.musicpd.org).\n" \
		"Visit http://musicus-mpc." \
		"compiled on " __DATE__ " " __TIME__,
		"copyright",	"\xC2\xA9 2006, 2007 S. Gerber",
		"authors",	authors,
		"documenters",	documenters,
		"translator-credits",	"translator-credits",
		"logo-icon-name",	img_c,
		NULL);
	return;
}

void gui_add_mrl_win(GtkWidget *widget, gpointer data) {

	GtkWidget *window;
	GtkWidget *hbox;
	GtkWidget *label,*entry;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 500, -1);
	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(MpdWin.win));
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_title(GTK_WINDOW(window), "Add MRL");
	g_signal_connect(G_OBJECT(window), "destroy",
					 G_CALLBACK(gtk_widget_destroy), NULL);

	hbox = gtk_hbox_new(FALSE,2);

	label = gtk_label_new("MRL:");
	gtk_box_pack_start_defaults(GTK_BOX(hbox), label);

	entry = gtk_entry_new();
	gtk_box_pack_start_defaults(GTK_BOX(hbox), entry);
	g_signal_connect(G_OBJECT(entry), "activate",
					 G_CALLBACK(mpd_mrl_add_cb),window);

	gtk_container_add(GTK_CONTAINER(window), hbox);

	gtk_widget_show_all(window);

	return;
}

/* updates statusbar with new message */
static void update_statusbar(gchar *message, GtkWidget *statusbar) {
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), sb_push_num, message);
	sb_push_num++;

	return;
}

/* callback for updating titlebar, statusbar and playlist on song change event */
static gboolean titlebar_updater(gpointer data) {
	if(MpdWin.window_creation_finished == 1) {
		gtk_window_set_title(GTK_WINDOW(MpdWin.win), title_text);
		if(mpd_info.mps.update) {
		    is_updating_db = TRUE;
            gtk_statusbar_push(GTK_STATUSBAR(StatusBar), sb_push_num++, "mpd: updating database");
		}
		else {
		    if(is_updating_db) {
                is_updating_db = FALSE;
                gtk_statusbar_pop(GTK_STATUSBAR(StatusBar), --sb_push_num);
		    }
            update_statusbar(sb_text, StatusBar);
        }
	}
	if(update_pl_tree == FALSE) {
		update_pl_tree = TRUE;
	}
	if(MpdWin.window_creation_finished == 1) {
		if(mpd_has_current_song_changed())
			playlist_update_tree();
	}
	if((int)data != mpd_info.update_interval) return FALSE;
	return TRUE;
}

/* auto change view if connected/disconnected */
static gboolean auto_change_view(gpointer data) {
	if(old_conn_state == -64)
		old_conn_state = mpd_info.msi.connected;

	if(old_conn_state != mpd_info.msi.connected) {
		old_conn_state = mpd_info.msi.connected;
		switch(mpd_info.msi.connected) {
			case 0: // not connected
				mpd_gui_hide_mpd_elements();
				break;
			case 1: // connected
				mpd_gui_show_mpd_elements();
				break;
			default:
				fprintf(log_file,"this shouldn't be reached");
				break;
		}
	}
	return ((int)data == mpd_info.update_interval)?TRUE:FALSE;
}

/* show playlist etc and hide notConnectedMsg */
void mpd_gui_show_mpd_elements(void) {
	gtk_widget_hide(notConnectedMsg);
	gtk_widget_show(mpdElements);
	/* hide the appropriate menu entries */
	menu_entries_update(mpd_play_state());
	return;
}

/* hide playlist and show notConnectedMsg */
void mpd_gui_hide_mpd_elements(void) {
	gtk_widget_hide(mpdElements);
	gtk_widget_show(notConnectedMsg);
	return;
}

/* keep media browser consistent */
static void page_switched(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer data) {

	/* if new current page is media browser */
	if(page_num == (guint)data) {
		media_browser_reread();
	}

	return;
}


/* cleanup applet playlist gui */
static void applet_pl_cleanup_cb(GtkWidget *widget, gpointer data) {

	gint x,y,width,height;

	if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}

	gtk_window_get_position(GTK_WINDOW(MpdWin.win), &x, &y);
	gtk_window_get_size(GTK_WINDOW(MpdWin.win), &width, &height);
	snprintf(MpdWin.characteristics.geom, 64, "%dx%d+%d+%d", width, height, x,y);
	MpdWin.characteristics.filled = TRUE;

	gtk_widget_hide(MpdWin.win);
	/* MpdWin = NULL; */
	return;
}

static void playlist_update_tree_cb(GtkWidget *widget, gpointer data) {
    playlist_update_tree();
    return;
}

static void mpd_connect_cb(GtkWidget *widget, gpointer data) {
    if(mpd_connect(mpd_info.obj)) {
        msi_fill(&mpd_info);
        mpd_info.msi.connected = TRUE;
    }
    return;
}

static void mpd_disconnect_cb(GtkWidget *widget, gpointer data) {
    mpd_disconnect(mpd_info.obj);
    msi_clear(&mpd_info);
    mpd_info.msi.connected = FALSE;
    return;
}

static void menu_entries_update(int play_state) {

    GList *list;
    GtkWidget *menu_entry_play, *menu_entry_pause;
    GtkWidget *menu_entry_conn, *menu_entry_disconn;

	/* only do this, if the playlist window exists!!! */
	if( (MpdWin.window_creation_finished == TRUE) && (menu_entries != NULL)) {
		list = menu_entries;
		list = g_list_first(list);
		menu_entry_play = (GtkWidget *)list->data;
		list = g_list_next(list);
		menu_entry_pause = (GtkWidget *)list->data;
		list = g_list_next(list);
		menu_entry_conn = (GtkWidget *)list->data;
		list = g_list_next(list);
		menu_entry_disconn = (GtkWidget *)list->data;

		if(mpd_info.msi.connected) {
			gtk_widget_hide(menu_entry_conn);
			gtk_widget_show(menu_entry_disconn);
			if(play_state == 1) {
				gtk_widget_hide(menu_entry_play);
				gtk_widget_show(menu_entry_pause);
			}
			else {
				gtk_widget_hide(menu_entry_pause);
				gtk_widget_show(menu_entry_play);
			}
		}
		else {
			gtk_widget_hide(menu_entry_disconn);
			gtk_widget_show(menu_entry_conn);
		}

	}

    return;
}

/* callback wrapper for mpd_mrl_add */
static void mpd_mrl_add_cb(GtkWidget *widget, gpointer data) {
    GtkEntry *e = GTK_ENTRY(widget);
    const gchar *etext = gtk_entry_get_text(e);
    if(mpd_info.auto_connect) {
            if(!mpd_check_connected(mpd_info.obj))
                mpd_connect(mpd_info.obj);
            mpd_mrl_add(mpd_info.obj, etext);
    }
    gtk_widget_destroy(GTK_WIDGET(data));
}

/* allows to react dynamically to mpd status changes
 * instead of periodic polling ...
 */
void gui_mpd_status_changed_cb(MpdObj *obj, ChangedStatusType what, gpointer data) {

    gchar buf[255];

    /* update mpd state */
    mpd_play_state_update(obj);
    /* do appropritate thing for action */
    if (what&MPD_CST_PLAYLIST) { /* playlist has changed */
        fprintf(log_file, "mpd: playlist changed\n");
        fflush(log_file);
        playlist_update_tree();
    }
    if (what&MPD_CST_SONGPOS) { /* song positon of playing song has changed */
        fprintf(log_file, "mpd: position of playing song has changed\n");
        fflush(log_file);
        playlist_update_tree();
    }
    if (what&MPD_CST_SONGID) { /* playing song has changed */
        fprintf(log_file, "mpd: playing song has changed\n");
        fflush(log_file);
        playlist_update_tree();
    }

    if (what&MPD_CST_DATABASE) { /* database has changed */
        fprintf(log_file, "mpd: database has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_UPDATING) { /* database updating status has changed -- seems not to work correctly ...*/
        fprintf(log_file, "mpd: database updating status has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_VOLUME) { /* volume has changed */
        fprintf(log_file, "mpd: volume changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_TOTAL_TIME) { /* total time of playing song has changed */
        fprintf(log_file, "mpd: total time of playing song has changed\n");
        fflush(log_file);
    }
if (what&MPD_CST_CROSSFADE) { /* crossfade status has changed */
        fprintf(log_file, "mpd: crossfade status has changed\n");
        fflush(log_file);
        snprintf(buf, 255, "crossfade %is", mpd_info.mps.xfade);
        gtk_label_set_text(GTK_LABEL(xfade_label), buf);
    }
    if (what&MPD_CST_RANDOM) { /* random status has changed */
        fprintf(log_file, "mpd: random status changed\n");
        fflush(log_file);
        if(mpd_info.mps.random)
            gtk_label_set_text(GTK_LABEL(random_label), "random on");
        else
            gtk_label_set_text(GTK_LABEL(random_label), "random off");
    }
    if (what&MPD_CST_REPEAT) { /* repeat status has changed */
        fprintf(log_file, "mpd: repeat status has changed\n");
        fflush(log_file);
        /* repeat active */
        if(mpd_info.mps.repeat)
            gtk_label_set_text(GTK_LABEL(repeat_label), "repeat on");
        else
            gtk_label_set_text(GTK_LABEL(repeat_label), "repeat off");
    }
    if (what&MPD_CST_STATE) { /* mpd state (playing/pause/...) has changed */
        fprintf(log_file, "mpd: mpd state has changed\n");
        fflush(log_file);
        playlist_update_tree();
        // do thingy with the menu entries
        menu_entries_update(mpd_play_state());
    }
    /* not checked:
     * MPD_CST_ELAPSED_TIME: elapsed time of current song has changed
     * MPD_CST_BITRATE: bitrate of current song has changed
     * MPD_CST_PERMISSION: permissions of client have changed
     * MPD_CST_AUDIOFORMAT: codec of current song has changed
     */
    return;
}

/* vim:sts=4:shiftwidth=4
 */
