/**
 * \file support.c
 * \author Simon Gerber
 * \date 2007-12-09
 * This file implements the support routines
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

#include <stdlib.h>
#include <stdarg.h>
#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>

#include "definitions_gtk.h"
#include "mpd.h"
#include "support.h"
#include "playlist.h"

/************ local functions ***********/

/**
 *
 */
gboolean do_string(int fmt_num, char *state_string, int current_time, int song_time);
/**
 *
 */
void parse_fmt_string(char *fmt, char *out, char *state, int hrs, int c_hrs, int c_min, int c_sec, int s_hrs, int s_min, int s_sec);
/**
 * append tree store entry
 * \param tree_store the TreeStore to append to
 * \param iter the iter in the TreeStore (for the position)
 * \param song the song to place in the current position
 * \param current_song 1 if the song we want to append is currently playing, 0 otherwise
 */
void append_tree_store_entry(GtkTreeStore *tree_store, GtkTreeIter *iter, mpd_Song *song, int current_song);
/**
 * append list store entry
 * \param list the ListStore to append to
 * \param iter the iter in the ListStore (for the position)
 * \param song the song to place in the current position
 * \param playing TRUE if the song we want to append is currently playing, FALSE otherwise
 */
void append_list_store_entry(GtkListStore *list, GtkTreeIter *iter, mpd_Song *song, gboolean playing);


/************ local variables ***********/
gint old_interval = -1;

/************ implementation ************/

GtkTreeStore *pl_create_tree_store(MpdPlContainer *plc) {
    GtkTreeStore *tree_store = gtk_tree_store_new(N_COLUMNS,G_TYPE_STRING,G_TYPE_INT,G_TYPE_BOOLEAN,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING);
    GtkTreeIter iter;
    int i;

    for(i=0;i<plc->length;i++) {
	if(i == plc->current)
	    append_tree_store_entry(tree_store, &iter, plc->list[i], TRUE);
	else
	    append_tree_store_entry(tree_store, &iter, plc->list[i], FALSE);
    }

    return tree_store;
}

GtkTreeStore *pl_empty_tree_store(void) {
    GtkTreeStore *tree_store = gtk_tree_store_new(N_COLUMNS,G_TYPE_STRING,G_TYPE_INT,G_TYPE_BOOLEAN,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING);
    return tree_store;
}

GtkListStore *pl_create_list_store(MpdPlContainer *list) {
    GtkListStore *list_store = gtk_list_store_new(N_COLUMNS,G_TYPE_STRING,G_TYPE_INT,G_TYPE_BOOLEAN,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING);
    GtkTreeIter iter;
    int i;

    for(i=0;i<list->length;i++) {
	if(i == list->current)
	    append_list_store_entry(list_store, &iter, list->list[i], TRUE);
	else
	    append_list_store_entry(list_store, &iter, list->list[i], FALSE);
    }

    return list_store;
}

void append_tree_store_entry(GtkTreeStore *tree_store, GtkTreeIter *iter, mpd_Song *song, gboolean playing) {
    gchar human_time[20];
    int old_debug = debug;
    debug = 0;
    if(debug) {
	fprintf(log_file, "[%s:%3i] %s(): create_list_store\n", __FILE__, __LINE__, __FUNCTION__);
	fflush(log_file);
    }

    gtk_tree_store_append(tree_store, iter, NULL);
    if(song->time == -1)
    	snprintf(human_time, 20, "n/a");
    else
    	snprintf(human_time, 20, "%i:%02i", song->time / 60, song->time % 60);
    if(song->title)
    	gtk_tree_store_set (tree_store, iter,
			    COLUMN_FILE, song->file,
			    COLUMN_PL_ID, song->id,
			    COLUMN_PLAYING, playing,
			    COLUMN_TITLE, song->title,
			    COLUMN_ARTIST, song->artist,
			    COLUMN_ALBUM, song->album,
			    COLUMN_TIME, song->time,
			    COLUMN_HUMAN_TIME, human_time,
			    -1);
    else
    	gtk_tree_store_set (tree_store, iter,
			    COLUMN_FILE, song->file,
			    COLUMN_PL_ID, song->id,
			    COLUMN_PLAYING, playing,
			    COLUMN_TITLE, song->name,
			    COLUMN_ARTIST, song->artist,
			    COLUMN_ALBUM, song->album,
			    COLUMN_TIME, song->time,
			    COLUMN_HUMAN_TIME, human_time,
			    -1);

    debug = old_debug;
    return;
}

void append_list_store_entry(GtkListStore *list, GtkTreeIter *iter, mpd_Song *song, gboolean playing) {
    gchar human_time[20];
    int old_debug = debug;
    debug = 0;
    if(debug) {
	fprintf(log_file, "[%s:%3i] %s(): create_list_store\n", __FILE__, __LINE__, __FUNCTION__);
	fflush(log_file);
    }

    gtk_list_store_append(list, iter);
    if(song->time == -1)
	snprintf(human_time, 20, "n/a");
    else
	snprintf(human_time, 20, "%i:%02i", song->time / 60, song->time % 60);
    if(song->title)
	gtk_list_store_set (list, iter,
			    COLUMN_FILE, song->file,
			    COLUMN_PL_ID, song->id,
			    COLUMN_PLAYING, playing,
			    COLUMN_TITLE, song->title,
			    COLUMN_ARTIST, song->artist,
			    COLUMN_ALBUM, song->album,
			    COLUMN_TIME, song->time,
			    COLUMN_HUMAN_TIME, human_time,
			    -1);
    else
	gtk_list_store_set (list, iter,
			    COLUMN_FILE, song->file,
			    COLUMN_PL_ID, song->id,
			    COLUMN_PLAYING, playing,
			    COLUMN_TITLE, song->name,
			    COLUMN_ARTIST, song->artist,
			    COLUMN_ALBUM, song->album,
			    COLUMN_TIME, song->time,
			    COLUMN_HUMAN_TIME, human_time,
			    -1);

    debug = old_debug;
    return;
}

/* create GtkTreeView with provided GtkListStore */
GtkWidget *pl_create_tree_view(GtkTreeModel *model) {
    GtkWidget *tree_view = gtk_tree_view_new_with_model(model);
    GtkCellRenderer *renderer, *renderer2;
    GtkTreeViewColumn *column;

    int i;
    for(i = 1; i < N_COLUMNS; i++) {
	renderer = gtk_cell_renderer_text_new();
	renderer2 = gtk_cell_renderer_toggle_new();
	gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer2), TRUE);
	switch(i) {
	    case COLUMN_TITLE:
		column = gtk_tree_view_column_new_with_attributes(
			    "Title",
			    renderer,
			    "text", i,
			    NULL
			 );
		break;
	    case COLUMN_PLAYING:
		column = gtk_tree_view_column_new_with_attributes(
			    "",
			    renderer2,
			    "active", i,
			    NULL
			 );
		break;
	   case COLUMN_ARTIST:
		column = gtk_tree_view_column_new_with_attributes(
			    "Artist",
			    renderer,
			    "text", i,
			    NULL
			 );
		break;
	    case COLUMN_ALBUM:
		column = gtk_tree_view_column_new_with_attributes(
			    "Album",
			    renderer,
			    "text", i,
			    NULL
			 );
		break;
	    case COLUMN_HUMAN_TIME:
		column = gtk_tree_view_column_new_with_attributes(
			    "Time",
			    renderer,
			    "text", i,
			    NULL
			 );
		break;
	    default:
		if(verbose) {
		    fprintf(log_file, "column %i not displayed\n", i);
		    fflush(log_file);
		}
		continue;
	}
	gtk_tree_view_column_set_resizable(GTK_TREE_VIEW_COLUMN(column), TRUE);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }


    return tree_view;
}

void delete_widget(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(widget);
    return;
}

void pl_win_quit(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(widget);
    return;
}

void dummy() {
    fprintf(log_file, "this is not implemented yet...\n");
    fflush(log_file);
    return;
}

void quit_procedure(GtkWidget *widget, gpointer data) {
    if (debug) {
	fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
	fflush(log_file);
    }
    gtk_main_quit();
    return;
}

/* updates title_text, sb_text and tt strings */
gboolean update_info(gpointer data) {

    char *state_string;
    int current_time, song_time;
    int play_state;

    if(mpd_info.msi.connected) { // connected
		play_state = mpd_play_state();
		current_time = mpd_get_current_play_time();
		song_time = mpd_get_current_song_time();
		if(!mpd_info.mps.update) {
			switch(play_state) {
			/* playing */
			case 1:
				state_string = "playing";
				do_string(FMT_TITLE, state_string, current_time, song_time);
				do_string(FMT_STATUSBAR, state_string, current_time, song_time);
#ifdef APPLET
				do_string(FMT_TOOLTIP, state_string, current_time, song_time);
				if(musicus_applet.show_text)
				do_string(FMT_APPLET, state_string, current_time, song_time);
				else
				snprintf(ap_text, 255, "[MPD]");
#endif
				break;
			case 2:
				state_string = "paused";
				do_string(FMT_TITLE, state_string, current_time, song_time);
				do_string(FMT_STATUSBAR, state_string, current_time, song_time);
#ifdef APPLET
				do_string(FMT_TOOLTIP, state_string, current_time, song_time);
				if(musicus_applet.show_text)
				do_string(FMT_APPLET, state_string, current_time, song_time);
				else
				snprintf(ap_text, 255, "[MPD]");
#endif
				break;
			default:
				snprintf(title_text, 255, "MPD [stopped]");
				snprintf(sb_text, 255, "[stopped]");
#ifdef APPLET
				snprintf(tt_text, 255, "MPD [stopped]");
				snprintf(ap_text, 255, "MPD");
#endif
			}
		}

		/* update MpdObj status */
		if(!mpd_check_connected(mpd_info.obj)) {
			if(mpd_connect(mpd_info.obj)!=MPD_OK) {
			    msi_clear(&mpd_info);
			    mpd_info.msi.connected = FALSE;
			    return FALSE;   
			}
			msi_fill(&mpd_info);
			mpd_info.msi.connected = TRUE;
		}
		mpd_status_update(mpd_info.obj);
		return ((mpd_info.update_interval != (int)data) ? FALSE : TRUE);
    }
#ifdef APPLET
    snprintf(ap_text, 255, "MPD");
#endif
    return FALSE;
}

/* formats the title, sb and tt_text strings
 * needs state, current_time and song_time
 */
gboolean do_string(int fmt_num, char *state_string, int current_time, int song_time) {
    int current_hrs, current_min, current_sec;
    int song_hrs, song_min, song_sec;
    current_min = current_time / 60;
    current_sec = current_time % 60;
    current_hrs = 0;
    if(current_min > 59) {
	current_hrs = current_min / 60;
	current_min = current_min % 60;
    }
    song_min = song_time / 60;
    song_sec = song_time % 60;
    song_hrs = 0;
    if(song_min > 59) {
	song_hrs = song_min / 60;
	song_min = song_min % 60;
    }
    if(song_hrs > 0 || current_hrs > 0) {
	if(current_hrs > 0) {
	    switch(fmt_num) {
		case FMT_TITLE:
		    parse_fmt_string(mpd_info.msf.title_fmt->str, title_text, state_string, 2, current_hrs, current_min, current_sec,
											       song_hrs, song_min, song_sec);
		    break;
		case FMT_STATUSBAR:
		    parse_fmt_string(mpd_info.msf.statusbar_fmt->str, sb_text, state_string, 2, current_hrs, current_min, current_sec,
												song_hrs, song_min, song_sec);
		    break;
#ifdef APPLET
		case FMT_TOOLTIP:
		    parse_fmt_string(mpd_info.msf.tooltip_fmt->str, tt_text, state_string, 2, current_hrs, current_min, current_sec,
											      song_hrs, song_min, song_sec);
		    break;
		case FMT_APPLET:
		    parse_fmt_string(mpd_info.msf.applet_fmt->str, ap_text, state_string, 2, current_hrs, current_min, current_sec,
											     song_hrs, song_min, song_sec);
		    break;
#endif
	    }
	}
	else {
	    switch(fmt_num) {
		case FMT_TITLE:
		    parse_fmt_string(mpd_info.msf.title_fmt->str, title_text, state_string, 1, current_hrs, current_min, current_sec,
											       song_hrs, song_min, song_sec);
		    break;
		case FMT_STATUSBAR:
		    parse_fmt_string(mpd_info.msf.statusbar_fmt->str, sb_text, state_string, 1, current_hrs, current_min, current_sec,
												song_hrs, song_min, song_sec);
		    break;
#ifdef APPLET
		case FMT_TOOLTIP:
		    parse_fmt_string(mpd_info.msf.tooltip_fmt->str, tt_text, state_string, 1, current_hrs, current_min, current_sec,
											      song_hrs, song_min, song_sec);
		    break;
		case FMT_APPLET:
		    parse_fmt_string(mpd_info.msf.applet_fmt->str, ap_text, state_string, 1, current_hrs, current_min, current_sec,
											     song_hrs, song_min, song_sec);
		    break;
#endif
	    }

	}
    }
    else {
	switch(fmt_num) {
	    case FMT_TITLE:
		parse_fmt_string(mpd_info.msf.title_fmt->str, title_text, state_string, 0, current_hrs, current_min, current_sec,
											   song_hrs, song_min, song_sec);
		break;
	    case FMT_STATUSBAR:
		parse_fmt_string(mpd_info.msf.statusbar_fmt->str, sb_text, state_string, 0, current_hrs, current_min, current_sec,
											    song_hrs, song_min, song_sec);
		break;
#ifdef APPLET
	    case FMT_TOOLTIP:
		parse_fmt_string(mpd_info.msf.tooltip_fmt->str, tt_text, state_string, 0, current_hrs, current_min, current_sec,
											  song_hrs, song_min, song_sec);
		break;
	    case FMT_APPLET:
		parse_fmt_string(mpd_info.msf.applet_fmt->str, ap_text, state_string, 0, current_hrs, current_min, current_sec,
											 song_hrs, song_min, song_sec);
		break;
#endif
	}
    }
    return TRUE;
}

/* parses the fmt string
 * - hrs as current_hrs > 1 => 2, song_hrs > 1 => 1 else 0 */
void parse_fmt_string(char *fmt, char *out, char *state, int hrs, int c_hrs, int c_min, int c_sec, int s_hrs, int s_min, int s_sec) {
    int i = 0, config = 0, pos = -1;
    char part[100], title[100], artist[100];
    for(i = 0; i < strlen(out); i++) out[i] = '\0';
    for(i = 0; i < strlen(fmt); i++) {
	if((config == 0) && (fmt[i] == '%')) {
	    config = 1;
	}
	else {
	    if(config) {
		switch(fmt[i]) {
		    case 't':
			mpd_get_current_title_string(title, 100);
			out[++pos] = '\0';
			strcat(out, title); pos = pos + strlen(title)-1;
			break;
		    case 'a':
			mpd_get_current_artist_string(artist, 100);
			out[++pos] = '\0';
			strcat(out, artist); pos = pos + strlen(artist) -1;
			break;
		    case 's':
			out[++pos] = '\0';
			snprintf(part, 100, "%s", state);
			strcat(out, part); pos = pos + strlen(part) - 1;
			break;
		    case 'Z':
		    case 'z':
			if(hrs == 2) {
			    if(fmt[i] == 'z') snprintf(part, 100, "%i:%02i:%02i", c_hrs, c_min, c_sec);
			    else snprintf(part, 100, "%i:%02i:%02i", s_hrs, s_min, s_sec);
			}
			else if(hrs == 1) {
			    if(fmt[i] == 'z') snprintf(part, 100, "%i:%02i", c_min, c_sec);
			    else snprintf(part, 100, "%i:%02i:%02i", s_hrs, s_min, s_sec);
			}
			else {
			    if(fmt[i] == 'z') snprintf(part, 100, "%i:%02i", c_min, c_sec);
			    else snprintf(part, 100, "%i:%02i", s_min, s_sec);
			}
			out[++pos] = '\0';
			strcat(out, part); pos = pos + strlen(part)-1;
			break;
		    default : out[++pos] = fmt[i];
		}
		config = 0;
	    }
	    else {
		out[++pos] = fmt[i];
	    }
	}
    }
    out[++pos]='\0';
    return;
}

/* check if update_interval has changed and restart periodic functions */
gboolean check_update_interval(gpointer data) {
    GList *u_r = mpd_info.update_routines;

    if(mpd_info.msi.connected) {
	if(old_interval == -1) old_interval = (gint)data;
	    if(old_interval != mpd_info.update_interval) {
			if(verbose) {
				fprintf(log_file, "[%s:%3i] %s(): update interval has changed!\n", __FILE__, __LINE__, __FUNCTION__);
				fflush(log_file);
				fprintf(log_file, "[%s:%3i] %s(): ",__FILE__, __LINE__+4, __FUNCTION__);
			}
			u_r = g_list_first(u_r);
			while(u_r != NULL) {
				if(verbose)
				fprintf(log_file, "g_timeout_add... ");
				g_timeout_add(mpd_info.update_interval, u_r->data, (gpointer)mpd_info.update_interval);
				u_r = g_list_next(u_r);
			}
			old_interval = mpd_info.update_interval;
			if(verbose) {
				fprintf(log_file, "all\n");
				fflush(log_file);
			}
	    }
	return TRUE;
    }
    return FALSE;
}

void debug_fprintf(FILE *out, char *format, ...) {
    va_list vargs;
    va_start(vargs, format);
    vfprintf(out, format, vargs);
    fflush(out);
    va_end(vargs);
    return;
}

gboolean get_data_dir(gchar *dir, int len, gboolean is_applet) {
    char *_path;
    FILE *p;
    char error_msg[4096];
    char data_path[1024];
    struct passwd *pw;

    pw = getpwuid(getuid());

    int length = strlen(pw->pw_dir);

    if(is_applet) {
		length += strlen("/.gnome2/musicus-applet") + 1;
		_path = (char *)malloc(length*sizeof(char));
		snprintf(_path, length, "%s/.gnome2/musicus-applet", pw->pw_dir);
    }
    else {
		length += strlen("/.musicus-mpc") + 1;
		_path = (char *)malloc(length*sizeof(char));
		snprintf(_path, length, "%s/.musicus-mpc", pw->pw_dir);
    }
    /* directory doesn't exist yet */
    if((p=fopen(_path, "r"))==NULL) {
		errno = 0;
		if(mkdir(_path, 0755)==-1) {
			snprintf(error_msg, 4096, "An error occurred while creating the directory %s.\n (mkdir: %s)", _path, strerror(errno));
			fprintf(stderr, error_msg);
			fflush(stderr);
			return FALSE;
		}
		snprintf(data_path,1024,"%s/data", _path);
		if(mkdir(data_path, 0755)==-1) {
			snprintf(error_msg, 4096, "An error occurred while creating the directory %s.\n (mkdir: %s)", _path, strerror(errno));
			fprintf(stderr, error_msg);
		}
    }
    else { fclose(p); }

    gchar *_dir = (gchar*)malloc(length *sizeof(gchar));

    if(is_applet)
		snprintf(_dir, length, "%s/.gnome2/musicus-applet", pw->pw_dir);
    else
		snprintf(_dir, length, "%s/.musicus-mpc", pw->pw_dir);

    strncpy(dir, _dir, len);

    free(_dir);

    return TRUE;
}

void menu_add_entry(GtkContainer *menu, gchar *label, GCallback callback, gpointer data) {

    GtkWidget *entry;

    entry = gtk_menu_item_new_with_mnemonic(label);
    gtk_container_add(menu, entry);
    g_signal_connect(G_OBJECT(entry), "activate", callback, data);

    return;
}

GtkWidget *menu_add_entry_stock_icon(GtkContainer *menu, gchar *label, const gchar *stock_id, GCallback callback, gpointer data) {

    GtkWidget *icon, *entry, *hbox, *m_label;

    entry = gtk_menu_item_new();

    hbox = gtk_hbox_new(FALSE, 0);
    icon = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
    m_label = gtk_label_new_with_mnemonic(label);
    gtk_box_pack_start(GTK_BOX(hbox), m_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(entry), hbox);
    gtk_container_add (menu, entry);
    g_signal_connect(G_OBJECT(entry), "activate", callback, data);

    return entry;
}

void menu_add_separator(GtkContainer *menu) {

    GtkWidget *menu_entry;

    menu_entry = gtk_separator_menu_item_new ();
    gtk_container_add (menu, menu_entry);
    gtk_widget_set_sensitive (menu_entry, FALSE);

    return;
}

void menu_shell_add_entry(GtkMenuShell *menu, gchar *label, GCallback callback, gpointer data) {

    GtkWidget *entry;

    entry = gtk_menu_item_new_with_mnemonic(label);
    gtk_menu_shell_append (menu, entry);
    g_signal_connect(G_OBJECT(entry), "activate", callback, data);

    return;
}

GtkWidget *menu_shell_add_entry_stock_icon(GtkMenuShell *menu, gchar *label, const gchar *stock_id, GCallback callback, gpointer data) {

    GtkWidget *icon, *entry, *hbox, *m_label;

    entry = gtk_menu_item_new();

    hbox = gtk_hbox_new(FALSE, 0);
    icon = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
    m_label = gtk_label_new_with_mnemonic(label);
    gtk_box_pack_start(GTK_BOX(hbox), m_label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(entry), hbox);
    gtk_menu_shell_append (menu, entry);
    g_signal_connect(G_OBJECT(entry), "activate", callback, data);

    return entry;
}



void menu_shell_add_separator(GtkMenuShell *menu) {

    GtkWidget *menu_entry;

    menu_entry = gtk_separator_menu_item_new ();
    gtk_menu_shell_append (menu, menu_entry);
    gtk_widget_set_sensitive (menu_entry, FALSE);

    return;
}

/* print window size */
void print_size(GtkWidget *widget, GtkAllocation *allocation, gpointer data) {
	fprintf(log_file, "window size:\n\tx =\t%i\n\ty =\t%i\n\twidth =\t%i\n\theight =\t%i\n",
			allocation->x, allocation->y, allocation->width, allocation->height);
	fflush(log_file);
	return;
}


/* vim:syntax=c:sts=4:shiftwidth=4
 */
