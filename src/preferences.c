/**
 * \file preferences.c
 * \author Simon Gerber
 * \date 2007-12-15
 * preferences dialog and loading/saving
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

/*************** includes ****************/

#include "definitions_gtk.h"

#include <pwd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifdef APPLET
#include <panel-applet.h>
#endif

#include "preferences.h"
#include "support.h"

/************ local functions ***********/
static void config_entry_new(GtkWidget *table, gchar *title, gint sep, gchar *value, GList *value_list);

/************ local variables ***********/

/* vertical position in table (prefs dialog) */
static gint v_pos = 0;

/************ IMPLEMENTATION ************/

void save_new_settings(GtkWidget *widget, gpointer data) {
	GList *args = (GList *)data;
	const gchar *_host, *_port, *_pw, *_interval, *_xfade,
#ifdef APPLET
		  *_tt_fmt, *_ap_fmt,
#endif
		  *_sb_fmt, *_tb_fmt;
#ifdef APPLET
	gboolean _ap_activate;
#endif
	gboolean _auto_connect;
	gint level;
	args = g_list_first(args); args = g_list_next(args);
	/* connect: yes/no */
	_auto_connect = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(args->data));
	args = g_list_next(args);
	/* mpd host */
	_host = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* mpd port */
	_port = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* mpd password */
	_pw = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* xfade secs */
	_xfade = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* update interval */
	_interval = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* log level */
	level = gtk_combo_box_get_active(GTK_COMBO_BOX(args->data));
	args = g_list_next(args);
#ifdef APPLET
	/* tooltip format */
	_tt_fmt = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* applet format */
	_ap_fmt = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	_ap_activate = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(args->data));
	args = g_list_next(args);
#endif
	/* titlebar format */
	_tb_fmt = gtk_entry_get_text(GTK_ENTRY(args->data));
	args = g_list_next(args);
	/* statusbar format */
	_sb_fmt = gtk_entry_get_text(GTK_ENTRY(args->data));


	if(verbose) {
		fprintf(log_file, "[%s:%3i] %s(): _cfg_conn = %i\n", __FILE__, __LINE__, __FUNCTION__, _auto_connect?1:0);
		fprintf(log_file, "[%s:%3i] %s(): _host     = %s\n", __FILE__, __LINE__, __FUNCTION__, _host);
		fprintf(log_file, "[%s:%3i] %s(): _port     = %s\n", __FILE__, __LINE__, __FUNCTION__, _port);
		fprintf(log_file, "[%s:%3i] %s(): _pw       = %s\n", __FILE__, __LINE__, __FUNCTION__, _pw);
		fprintf(log_file, "[%s:%3i] %s(): _xfade    = %s\n", __FILE__, __LINE__, __FUNCTION__, _xfade);
		fprintf(log_file, "[%s:%3i] %s(): _interval = %s\n", __FILE__, __LINE__, __FUNCTION__, _interval);
		fprintf(log_file, "[%s:%3i] %s(): level     = %d\n", __FILE__, __LINE__, __FUNCTION__, level);
#ifdef APPLET
		fprintf(log_file, "[%s:%3i] %s(): tooltip   = %s\n", __FILE__, __LINE__, __FUNCTION__, _tt_fmt);
		fprintf(log_file, "[%s:%3i] %s(): applet    = %s\n", __FILE__, __LINE__, __FUNCTION__, _ap_fmt);
		fprintf(log_file, "[%s:%3i] %s(): show (ap) = %i\n", __FILE__, __LINE__, __FUNCTION__, _ap_activate ? 1:0);
#endif
		fprintf(log_file, "[%s:%3i] %s(): titlebar  = %s\n", __FILE__, __LINE__, __FUNCTION__, _tb_fmt);
		fprintf(log_file, "[%s:%3i] %s(): statusbar = %s\n", __FILE__, __LINE__, __FUNCTION__, _sb_fmt);
		fflush(log_file);
	}

	g_string_free(mpd_info.msi.host, TRUE);
	mpd_info.msi.host = g_string_new(_host);

	mpd_info.msi.port = atoi(_port);

	g_string_free(mpd_info.msi.pw, TRUE);
	mpd_info.msi.pw = g_string_new(_pw);

	mpd_info.auto_connect = _auto_connect;

	mpd_info.update_interval = atoi(_interval);

	mpd_info.xfade_secs = atoi(_xfade);

	verbose = (level == 2) ? 1 : 0;
	debug = (level >= 1) ? 1 : 0;

#ifdef APPLET
	g_string_free(mpd_info.msf.tooltip_fmt, TRUE);
	mpd_info.msf.tooltip_fmt = g_string_new(_tt_fmt);

	g_string_free(mpd_info.msf.applet_fmt, TRUE);
	mpd_info.msf.applet_fmt = g_string_new(_ap_fmt);

	musicus_applet.show_text = _ap_activate;
#endif

	g_string_free(mpd_info.msf.title_fmt, TRUE);
	mpd_info.msf.title_fmt = g_string_new(_tb_fmt);

	g_string_free(mpd_info.msf.statusbar_fmt, TRUE);
	mpd_info.msf.statusbar_fmt = g_string_new(_sb_fmt);


	config_save();

	return;
}

/* properties window is centered on `parent', parent == NULL => window centered on screen */
GtkWidget *mpd_new_properties_window(GtkWindow *parent) {
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	GtkWidget *win;
	GtkWidget *vbox;
	GtkWidget *frame;
	GtkWidget *table;
	GtkWidget *hbox;
	GtkWidget *button;
	GtkWidget *check_btn;
	GList *value_list = NULL;
	value_list = g_list_append(value_list, win);

	win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win), "Properties");
	gtk_window_set_default_size(GTK_WINDOW(win), 300, 200);
	if(parent != NULL) {
		gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ON_PARENT);
		gtk_window_set_transient_for(GTK_WINDOW(win), parent);
	}
	else
		gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ALWAYS);
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(win), vbox);

	/* mpd settings */
	frame = gtk_frame_new("MPD settings");
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 2);

	table = gtk_table_new(2, 5, TRUE);
	gtk_container_add(GTK_CONTAINER(frame), table);
	v_pos = 0;

	/* connect on startup */
	check_btn = gtk_check_button_new_with_label("Auto-Connect to MPD:");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_btn), mpd_info.auto_connect);
	gtk_table_attach(GTK_TABLE(table), check_btn, 0,5, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0,0);
	v_pos++;
	value_list = g_list_append(value_list, check_btn);

	/* mpd host */
	if(mpd_info.msi.host == NULL)
		mpd_info.msi.host = g_string_new("");
	config_entry_new(table, "MPD host:", 1, mpd_info.msi.host->str, value_list);

	/* mpd port */
	char port[6];
	snprintf(port, 6, "%i", mpd_info.msi.port);
	config_entry_new(table, "MPD port:", 1, port, value_list);

	/* mpd password */
	if(mpd_info.msi.pw == NULL)
		mpd_info.msi.pw = g_string_new("");
	config_entry_new(table, "MPD password:", 1, mpd_info.msi.pw->str, value_list);

	/* crossfade settings */
	char xfade[6];
	snprintf(xfade,6,"%i", mpd_info.xfade_secs);
	config_entry_new(table, "Crossfade timeout:", 1, xfade, value_list);

	/* playlist settings */
	frame = gtk_frame_new("General settings");
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 2);

	table = gtk_table_new(1,5,FALSE);
	gtk_container_add(GTK_CONTAINER(frame), table);
	v_pos = 0;

	/* update interval */
	gchar update_iv[5];
	snprintf(update_iv, 5, "%i", mpd_info.update_interval);
	config_entry_new(table, "Update interval", 2, update_iv, value_list);

	/* verbosity */
	v_pos ++;
	GtkWidget *label, *combo;
	gint now = (verbose == 1) ? 2 : debug;
	label = gtk_label_new("Verbosity:");
	gtk_table_attach(GTK_TABLE(table), label, 0, 2, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
	combo = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Level 0 (almost nothing)");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Level 1 (some debug information)");
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo), "Level 2 (quite verbose, not recommended!)");
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo), now);
	value_list = g_list_append(value_list, combo);
	gtk_table_attach(GTK_TABLE(table), combo, 2, 4, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);

	/* status display formats (tooltip, title-/statusbar) */
	frame = gtk_frame_new("Text formats");
	gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 2);
	table = gtk_table_new(3, 5, TRUE);
	gtk_container_add(GTK_CONTAINER(frame), table);
	v_pos = 0;

	/* tooltip */
#ifdef APPLET
	if(mpd_info.msf.tooltip_fmt == NULL)
		mpd_info.msf.tooltip_fmt = g_string_new("[\%s] \%a - \%t [\%z/\%Z]");
	config_entry_new(table, "Tooltip string", 2, mpd_info.msf.tooltip_fmt->str, value_list);
	if(mpd_info.msf.applet_fmt == NULL)
		mpd_info.msf.applet_fmt = g_string_new("MPD: \%a");
	config_entry_new(table, "Applet string", 2, mpd_info.msf.applet_fmt->str, value_list);
	check_btn = gtk_check_button_new_with_label("Activate Applet text");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_btn), musicus_applet.show_text);
	gtk_table_attach(GTK_TABLE(table), check_btn, 0,5, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0,0);
	v_pos++;
	value_list = g_list_append(value_list, check_btn);
#endif

	if(mpd_info.msf.title_fmt == NULL)
		mpd_info.msf.title_fmt = g_string_new("\%a - \%t [\%s]");
	config_entry_new(table, "Titlebar string", 2, mpd_info.msf.title_fmt->str, value_list);

	if(mpd_info.msf.statusbar_fmt == NULL)
		mpd_info.msf.statusbar_fmt = g_string_new("[\%s] \%z/\%Z");
	config_entry_new(table, "Statusbar string", 2, mpd_info.msf.statusbar_fmt->str, value_list);

	/* explanation */
	label = gtk_label_new("\%s = mpd state (playing | paused)");
	gtk_table_attach(GTK_TABLE(table), label, 0, 5, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
	v_pos++;
	label = gtk_label_new("%t = song title \%a = artist name");
	gtk_table_attach(GTK_TABLE(table), label, 0, 5, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
	v_pos++;
	label = gtk_label_new("%z = current time %Z = song duration");
	gtk_table_attach(GTK_TABLE(table), label, 0, 5, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
	v_pos++;

	/* buttons */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

	button = gtk_button_new_with_label("Cancel");
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
					 		 G_CALLBACK(delete_widget), win);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	button = gtk_button_new_with_label("OK");
	g_signal_connect(G_OBJECT(button), "clicked",
					 G_CALLBACK(save_new_settings), value_list);
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
							 G_CALLBACK(delete_widget), win);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	return win;
}

void mpd_new_properties_window_cb(GtkWidget *widget, gpointer data) {
		GtkWidget *win = mpd_new_properties_window(GTK_WINDOW(data));
		gtk_widget_show_all(win);
		return;
}

static void config_entry_new(GtkWidget *table, gchar *title, gint sep, gchar *value, GList *value_list) {
	GtkWidget *label, *entry;
	label = gtk_label_new(title);
	gtk_table_attach(GTK_TABLE(table), label, 0, sep, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), value);
	value_list = g_list_append(value_list, entry);
	gtk_table_attach(GTK_TABLE(table), entry, sep, 5, v_pos, v_pos+1, GTK_EXPAND|GTK_FILL, GTK_EXPAND|GTK_FILL, 0, 0);
	v_pos++;
	return;
}

gboolean config_new() {
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	/* standard values */
	mpd_info.auto_connect = TRUE;
	mpd_info.msi.host = g_string_new("localhost");
	mpd_info.msi.port = 6600;
	mpd_info.msi.pw = g_string_new("");
	mpd_info.update_interval = 500;
	/* standard settings for verbosity */
	verbose = 0; debug = 0;
#ifdef APPLET
	mpd_info.msf.tooltip_fmt = g_string_new("[\%s] \%a - \%t [\%z/\%Z]");
	mpd_info.msf.applet_fmt = g_string_new("MPD: \%a");
	musicus_applet.show_text = TRUE;
#endif
	mpd_info.msf.title_fmt = g_string_new("\%a - \%t [\%s]");
	mpd_info.msf.statusbar_fmt = g_string_new("[\%s] \%z/\%Z");
	GtkWidget *config = mpd_new_properties_window(NULL);
	gtk_widget_show_all(config);
	config_save();
	return FALSE;
}

gboolean config_save() {
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	FILE *f = fopen(mpd_info.config_file->str, "w");
	fprintf(f, "auto_connect=%i\n",mpd_info.auto_connect?1:0);
	fprintf(f, "host=%s\n", mpd_info.msi.host->str);
	fprintf(f, "port=%d\n", mpd_info.msi.port);
	fprintf(f, "pw=%s\n", mpd_info.msi.pw->str);
	fprintf(f, "xfade=%i\n", mpd_info.xfade_secs);
	fprintf(f, "interval=%d\n", mpd_info.update_interval);
	int level = (verbose == 1) ? 2 : debug;
	fprintf(f, "log_level=%d\n", level);
#ifdef APPLET
	fprintf(f, "tt_fmt=%s\n", mpd_info.msf.tooltip_fmt->str);
	fprintf(f, "ap_fmt=%s\n", mpd_info.msf.applet_fmt->str);
	fprintf(f, "ap_show=%i\n", musicus_applet.show_text?1:0);
#endif
	fprintf(f, "tb_fmt=%s\n", mpd_info.msf.title_fmt->str);
	fprintf(f, "sb_fmt=%s\n", mpd_info.msf.statusbar_fmt->str);
	fclose(f);
/* #ifdef APPLET
	GConfValue *value = gconf_value_new(GCONF_VALUE_STRING);
	gconf_value_set_string (value, mpd_info.msi.host->str);
	panel_applet_gconf_set_value(PANEL_APPLET(mpd_applet->applet),
								 "mpd-host", value, NULL);
	gconf_value_free(value);
#endif */
	return TRUE;
}

gboolean config_load() {
	fprintf(log_file, "configuration file read...\n");
	fflush(log_file);
	FILE *f = fopen(mpd_info.config_file->str, "r");
	if(f == NULL) {
		printf("no config file\n");
		return FALSE;
	}
	int i = 0, value_flag = 0, key_len = 0, line_len = 0;
	char line[512];
	char key[512], value[512];
	while(fgets(line, sizeof(line), f) != 0) {

		line_len = strlen(line);
		key_len = 0; value_flag = 0;

		for(i=0; i < line_len; i++) {
			if(line[i] == '=') {
				value_flag = 1;
				key_len = i + 1;
				key[i] = '\0';
				continue;
			}
			else if(line[i] == '\n') {
				value[i-key_len] = '\0';
				continue;
			}
			if(value_flag)
				value[i-key_len] = line[i];
			else
				key[i] = line[i];

		}

		if(verbose) {
			fprintf(log_file, "[%s:%3i] %s(): %s = %s\n", __FILE__, __LINE__, __FUNCTION__, key, value);
			fflush(log_file);
		}

		if(strcmp(key, "auto_connect") == 0)
			mpd_info.auto_connect = (atoi(value))?TRUE:FALSE;
		if(strcmp(key, "host") == 0) {
			if(mpd_info.msi.host != NULL) g_string_free(mpd_info.msi.host, TRUE);
			mpd_info.msi.host = g_string_new(value);
		}
		else if(strcmp(key, "port") == 0) {
			mpd_info.msi.port = atoi(value);
		}
		else if(strcmp(key, "pw") == 0) {
			if(mpd_info.msi.pw != NULL) g_string_free(mpd_info.msi.pw, TRUE);
			mpd_info.msi.pw = g_string_new(value);
		}
		else if(strcmp(key, "interval") == 0) {
			mpd_info.update_interval = atoi(value);
		}
		else if(strcmp(key, "log_level") == 0) {
			int level = atoi(value);
			verbose = (level == 2) ? 1 : 0;
			debug = (level >= 1) ? 1 : 0;
		}
#ifdef APPLET
		else if(strcmp(key, "tt_fmt") == 0) {
			if(mpd_info.msf.tooltip_fmt != NULL) g_string_free(mpd_info.msf.tooltip_fmt, TRUE);
			mpd_info.msf.tooltip_fmt = g_string_new(value);
		}
		else if(strcmp(key, "ap_fmt") == 0) {
			if(mpd_info.msf.applet_fmt != NULL) g_string_free(mpd_info.msf.applet_fmt, TRUE);
			mpd_info.msf.applet_fmt = g_string_new(value);
		}
		else if(strcmp(key, "ap_show")==0) {
			musicus_applet.show_text = (atoi(value) == 1) ? TRUE:FALSE;
		}
#endif
		else if(strcmp(key, "tb_fmt") == 0) {
			if(mpd_info.msf.title_fmt != NULL) g_string_free(mpd_info.msf.title_fmt, TRUE);
			mpd_info.msf.title_fmt = g_string_new(value);
		}
		else if(strcmp(key, "sb_fmt") == 0) {
			if(mpd_info.msf.statusbar_fmt != NULL) g_string_free(mpd_info.msf.statusbar_fmt, TRUE);
			mpd_info.msf.statusbar_fmt = g_string_new(value);
		}
		else if(strcmp(key, "xfade") == 0) {
			mpd_info.xfade_secs = atoi(value);
		}

		key[0] = '\0';
		value[0] = '\0';
	}
	fclose(f);

	fprintf(log_file, "searching mpd at %s:%i\n", mpd_info.msi.host->str, mpd_info.msi.port);
	fflush(log_file);

	return TRUE;
}

gboolean config_get_config_file(gboolean is_applet) {
	gchar dir[512],*path;
	int length;
	if(get_data_dir(dir,512,is_applet)) {
		length = strlen(dir)+8;
		path = (gchar *)malloc(length*sizeof(gchar));
		if(is_applet)
			snprintf(path,length, "%s/config", dir);
		else
			snprintf(path,length, "%s/config", dir);
		mpd_info.config_file = g_string_new(path);
	}
	else return FALSE;

	return TRUE;
}

void create_info_dialog(GtkWidget *window, gchar *title, gchar *info) {
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (GTK_WINDOW(window),
									 GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_OK,
									 title);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), info);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	return;
}

/* vim:syntax=c:ts=4:shiftwidth=4
 */
