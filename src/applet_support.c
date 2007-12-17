/**
 * \file applet_support.c
 * \author Simon Gerber
 * \date 2007-12-16
 * This file implements support functions for the applet
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

 #include "applet_support.h"
 #include "support.h"
 #include "mpd.h"
 #include "mpd_action.h"
 #include "gui.h"

/************** local functions **************/
static void mpd_menu_popup(guint button);
/* static void mpd_menu_popdown(void); */
static gboolean show_musicus_window(void);
static gboolean hide_musicus_win(void);
static void menu_position(GtkMenu *menu, int *x, int *y, gboolean *push_in, gpointer data);
static gboolean menu_item_pf(gpointer data);

/************** local variables **************/
static gboolean menu_visible = FALSE;
static GtkWidget *menu;
static GtkWidget *menu_item_play;
static GtkWidget *menu_item_pause;

/************** implementation ***************/

gboolean applet_button_pressed_cb(GtkWidget *widget, GdkEventButton *event_btn, gpointer data) {
	if(event_btn->button == 1) {
		if(event_btn->type == GDK_BUTTON_PRESS) {
			mpd_menu_popup(event_btn->button);
			return TRUE;
		}
	}
	return FALSE;
}

void create_applet_menu(void) {
	GtkWidget *menu_item;

	menu = gtk_menu_new();

	/* title */
	menu_item = gtk_menu_item_new_with_label("MPD controls");
	gtk_widget_set_sensitive(GTK_WIDGET(menu_item), FALSE);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	/* separator */
	menu_shell_add_separator(GTK_MENU_SHELL(menu));

	/* action: previous */
	menu_shell_add_entry_stock_icon(GTK_MENU_SHELL(menu), "Previous", GTK_STOCK_MEDIA_PREVIOUS, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_PREVIOUS));

	/* action: play */
	menu_item_play = menu_shell_add_entry_stock_icon(GTK_MENU_SHELL(menu), "Play", GTK_STOCK_MEDIA_PLAY, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_PLAY_PAUSE));

	/* action: pause */
	menu_item_pause = menu_shell_add_entry_stock_icon(GTK_MENU_SHELL(menu), "Pause", GTK_STOCK_MEDIA_PAUSE, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_PLAY_PAUSE));

	/* action: stop */
	menu_shell_add_entry_stock_icon(GTK_MENU_SHELL(menu), "Stop", GTK_STOCK_MEDIA_STOP, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_STOP));

	/* action: next */
	menu_shell_add_entry_stock_icon(GTK_MENU_SHELL(menu), "Next", GTK_STOCK_MEDIA_NEXT, G_CALLBACK(action), ACTION_TYPE_TO_GPOINTER(ACTION_NEXT));

	/* action: separator */
	menu_shell_add_separator(GTK_MENU_SHELL(menu));

	/* player window */
	menu_shell_add_entry(GTK_MENU_SHELL(menu), "Player Window", G_CALLBACK(show_musicus_window), ACTION_TYPE_TO_GPOINTER(ACTION_PLAYLIST));

	gtk_widget_show_all(menu);

	mpd_info.update_routines = g_list_append(mpd_info.update_routines, menu_item_pf);
	g_timeout_add(mpd_info.update_interval, menu_item_pf, (gpointer)mpd_info.update_interval);

	menu_visible = FALSE;

	return;
}

static void mpd_menu_popup(guint button) {
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
				   menu_position, NULL, button, gtk_get_current_event_time());
	menu_visible = TRUE;
	return;
}

#if 0
/* These functions are not needed right now */
static void mpd_menu_popdown(void) {
	gtk_menu_popdown(GTK_MENU(menu));
	return;
}
#endif

static gboolean show_musicus_window(void) {
	/* first time to open the musicus window */
	if(MpdWin.win == NULL) {
		MpdWin.win = mpd_win_new(1,1);
		g_signal_connect(G_OBJECT(MpdWin.win), "destroy",
						 G_CALLBACK(hide_musicus_win), NULL);
		g_signal_connect(G_OBJECT(MpdWin.win), "delete_event",
						 G_CALLBACK(hide_musicus_win), NULL);
		gtk_widget_show_all(MpdWin.win);
	}
	/* restore the window */
	else {
		if(MpdWin.characteristics.filled)
			gtk_window_parse_geometry(GTK_WINDOW(MpdWin.win), MpdWin.characteristics.geom);
		gtk_widget_show(MpdWin.win);
	}

	if(mpd_info.msi.connected)
		mpd_gui_show_mpd_elements();
	else
		mpd_gui_hide_mpd_elements();

	MpdWin.window_creation_finished = 1;

	return TRUE;
}

static gboolean hide_musicus_win(void) {
	gint x,y,width,height;

	MpdWin.window_creation_finished = 0;

	gtk_window_get_position(GTK_WINDOW(MpdWin.win), &x, &y);
	gtk_window_get_size(GTK_WINDOW(MpdWin.win), &width, &height);
	snprintf(MpdWin.characteristics.geom, GEOM_LEN, "%dx%d+%d+%d", width, height, x,y);

	MpdWin.characteristics.filled = TRUE;
	gtk_widget_hide(MpdWin.win);

	return TRUE;
}

static void menu_position(GtkMenu *menu, int *x, int *y, gboolean *push_in, gpointer data) {
	GtkWidget *pa = GTK_WIDGET(musicus_applet.applet);
	GtkRequisition rq;
	gint menu_xpos;
	gint menu_ypos;

	gtk_widget_size_request(GTK_WIDGET(menu), &rq);

	gdk_window_get_origin(pa->window, &menu_xpos, &menu_ypos);

	menu_xpos += pa->allocation.x;
	menu_ypos += pa->allocation.y;

	switch(panel_applet_get_orient(musicus_applet.applet)) {
		case PANEL_APPLET_ORIENT_DOWN:
		case PANEL_APPLET_ORIENT_UP:
			if(menu_ypos > gdk_screen_get_height (gtk_widget_get_screen(pa)))
				menu_ypos -= rq.height;
			else
				menu_ypos += pa->allocation.height;
			break;
		case PANEL_APPLET_ORIENT_RIGHT:
		case PANEL_APPLET_ORIENT_LEFT:
			if(menu_xpos > gdk_screen_get_width (gtk_widget_get_screen(pa)))
				menu_xpos -= rq.width;
			else
				menu_xpos += pa->allocation.width;
			break;
		default:
			g_assert_not_reached();
	}

	*x = menu_xpos;
	*y = menu_ypos;
	*push_in = TRUE;
	return;
}

static gboolean menu_item_pf(gpointer data) {
	if(mpd_play_state() == 1) {
		gtk_widget_hide(menu_item_play);
		gtk_widget_show(menu_item_pause);
	}
	else {
		gtk_widget_hide(menu_item_pause);
		gtk_widget_show(menu_item_play);
	}
	return (mpd_info.update_interval == (int)data)?TRUE:FALSE;
}
