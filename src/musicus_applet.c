/**
 * \file musicus_applet.c
 * \author Simon Gerber
 * \date 2007-12-16
 * This file is the main file for the GNOME applet
 * client
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

#include "definitions_applet.h"
#include "preferences.h"
#include "support.h"
#include "mpd.h"
#include "gui.h"
#include "applet_support.h"

#include <string.h>

/************** local functions **************/
static void musicus_applet_new(MusicusApplet *ma);
static void musicus_applet_init(MusicusApplet *ma);
static gboolean update_tooltip(gpointer data);
static gboolean tooltip_cb(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data);
static void about_cb (BonoboUIComponent *uic, gpointer data, const gchar *verbname);
static void properties_cb(BonoboUIComponent *uic, gpointer data, const gchar *verbname);
static void applet_text_cb(BonoboUIComponent *uic, gpointer data, const gchar *verbname);

/************** local variables **************/
static const BonoboUIVerb musicus_applet_menu_verbs[] = {
	BONOBO_UI_UNSAFE_VERB ("Properties", properties_cb),
	BONOBO_UI_UNSAFE_VERB ("AppletText", applet_text_cb),
	BONOBO_UI_UNSAFE_VERB ("About", about_cb),
	BONOBO_UI_VERB_END
};
static PanelAppletClass *parent_class = NULL;

/************** implementation  **************/

static void musicus_applet_new(MusicusApplet *ma) {
	ma->label = gtk_label_new("MPD");
	return;
}

static void musicus_applet_init(MusicusApplet *ma) {

	parent_class = g_type_class_ref(PANEL_TYPE_APPLET);

	create_applet_menu();

	mpd_play_state_update(mpd_info.obj);

	return;
}

static gboolean update_tooltip(gpointer data) {
	//gtk_tooltips_set_tip(musicus_applet.tooltips, GTK_WIDGET(musicus_applet.applet), tt_text, NULL);
	/* now using new GtkTooltip instead of GtkTooltips */
	gtk_widget_set_tooltip_text(GTK_WIDGET(musicus_applet.applet), tt_text);
	/* update applet text */
	gtk_label_set_text(GTK_LABEL(musicus_applet.label), ap_text);
	return ((mpd_info.update_interval != (int)data) ? FALSE : TRUE);
}

#if 0
static gboolean tooltip_cb(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode, GtkTooltip *tooltip, gpointer user_data) {
	gtk_label_set_text(GTK_LABEL(musicus_applet.tt_label), tt_text);

	return TRUE;
}
#endif

static void about_cb(BonoboUIComponent *uic, gpointer data, const gchar *verbname) {
	static const gchar *authors[] = {
		AUTHORS,
		NULL
	};
	static const gchar *documenters[] = {
		DOCUMENTERS,
		NULL
	};

	gtk_show_about_dialog(NULL,
		"name",	"Musicus MPD client",
		"version", VERSION,
		"comments", "Musicus is a MPD client for the GNOME Panel.",
		"copyright", "\xC2\xA9 2006 - 2008 S. Gerber",
		"authors", authors,
		"documenters", documenters,
		"translator-credits", "translator-credits",
		"logo-icon-name", LOGO,
		NULL
	);
	return;
}

static void properties_cb(BonoboUIComponent *uic, gpointer data, const gchar *verbname) {
	GtkWidget *prop_win = mpd_new_properties_window(NULL);
	g_signal_connect_swapped(G_OBJECT(prop_win), "delete_event",
							 G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect_swapped(G_OBJECT(prop_win), "destroy",
							 G_CALLBACK(gtk_widget_destroy), NULL);
	gtk_widget_show_all(prop_win);
	return;
}

static void applet_text_cb(BonoboUIComponent *uic, gpointer data, const gchar *verbname) {
	musicus_applet.show_text = !musicus_applet.show_text;
	config_save();
	return;
}

static gboolean musicus_applet_fill (PanelApplet *applet, const gchar *iid, gpointer data) {

	GtkWidget *event_box;
	/* GtkWidget *tt_win;
	GtkWidget *tt_label; */

	gchar file_path[100];
	gchar datadir[512], data_dir[512];

	/* enable debug output */
	debug = 1;
	log_file = stdout;
	err_file = stderr;

	get_data_dir(datadir,512,TRUE);
	
	config_get_config_file(TRUE);
	if(!config_load())
		config_new();	

	snprintf(data_dir, 512, "%s/data", datadir);

	/* open log file */
	snprintf(file_path, 100, "%s/log.txt", data_dir);
	log_file = fopen(file_path, "a+");
	fprintf(log_file, "------- new execution --------\n");
	fflush(log_file);

	/* open error file */
	snprintf(file_path, 100, "%s/err.txt", data_dir);
	err_file = fopen(file_path, "a+");
	fprintf(err_file, "---------new execution--------\n");
	fflush(err_file);


	mpd_info.obj = mpd_new(HOST,PORT,PASSWORD);
	msi_fill(&mpd_info);

	if(mpd_info.auto_connect) {
		if(mpd_connect(mpd_info.obj) == MPD_OK)
			mpd_info.msi.connected = TRUE;
		else
			mpd_info.msi.connected = FALSE;
	}

	musicus_applet_new(&musicus_applet);
	musicus_applet_init(&musicus_applet);

	musicus_applet.applet = applet;

	if(strcmp(iid, "OAFIID:MusicusApplet") != 0)
		return FALSE;

	event_box = gtk_event_box_new();
	g_signal_connect(G_OBJECT(event_box), "button-press-event",
					 G_CALLBACK(applet_button_pressed_cb), NULL);

	gtk_container_add(GTK_CONTAINER(applet), event_box);
	gtk_container_add(GTK_CONTAINER(event_box), musicus_applet.label);

	/* setup menu */
	panel_applet_setup_menu_from_file(applet, data_dir, SHARE_DIR "/musicus_applet/GNOME_MusicusApplet.xml", NULL, musicus_applet_menu_verbs, NULL);

	/* tooltips */
	gtk_widget_set_tooltip_text(GTK_WIDGET(applet), "Musicus MPD Client");
	/* gtk_widget_set_has_tooltip(GTK_WIDGET(applet), TRUE);
	g_signal_connect(G_OBJECT(applet), "query-tooltip",
			 G_CALLBACK(tooltip_cb), NULL);
	tt_label = gtk_label_new("Musicus MPD client");
	tt_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(tt_win), -1, -1);
	gtk_container_add(GTK_CONTAINER(tt_win), tt_label);
	gtk_widget_set_tooltip_window(GTK_WIDGET(applet), GTK_WINDOW(tt_win));
	musicus_applet.tt_label = tt_label; */
	
	/* periodic funcs */
	mpd_info.update_routines = NULL;
	mpd_info.period_funcs = NULL;
	mpd_info.update_routines = g_list_append(mpd_info.update_routines, update_info);
	mpd_info.update_routines = g_list_append(mpd_info.update_routines, update_tooltip);
	mpd_info.period_funcs = g_list_append(mpd_info.period_funcs, check_update_interval);
	g_timeout_add(mpd_info.update_interval, update_info, (gpointer)mpd_info.update_interval);
	g_timeout_add(mpd_info.update_interval, update_tooltip, (gpointer)mpd_info.update_interval);

	g_timeout_add(500, check_update_interval, (gpointer)mpd_info.update_interval);

	mpd_signal_connect_status_changed(mpd_info.obj, (StatusChangedCallback)gui_mpd_status_changed_cb, NULL);

	gtk_widget_show_all(GTK_WIDGET(applet));

	return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:MusicusApplet_Factory",
							 PANEL_TYPE_APPLET,
							 "Musicus MPD client",
							 "0",
							 musicus_applet_fill,
							 NULL
							);
