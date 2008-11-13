/**
 * \file mpd_gui_standalone.c
 * \author Simon Gerber
 * \date 2007-12-15
 * standalone GTK+ GUI
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
#include <string.h>

#include "definitions_gtk.h"
#include "gui.h"
#include "mpd.h"
#include "support.h"
#include "preferences.h"

int main(int argc, char *argv[]) {

    debug = 0;
    verbose = 0;
    gboolean run_new_config = FALSE;
    GtkWidget *mpd_win;

    log_file = stdout;
    err_file = stderr;

    gtk_init(&argc,&argv);

    config_get_config_file(FALSE);
    if(!config_load())
        run_new_config = TRUE;

    if(run_new_config)
        config_new();

    mpd_info.update_routines = NULL;
    mpd_info.period_funcs = NULL;

    if(strcmp(PASSWORD, "") == 0)
	mpd_info.obj = mpd_new(HOST,PORT,NULL);
    else
        mpd_info.obj = mpd_new(HOST,PORT,PASSWORD);

    msi_fill(&mpd_info);

    /* check if we want to auto-connect to mpd */
    if(mpd_info.auto_connect) {
        if(mpd_connect(mpd_info.obj)==MPD_OK) {
	    mpd_send_password(mpd_info.obj);
	    mpd_set_connection_timeout(mpd_info.obj, 5);
            mpd_info.msi.connected = TRUE;
	}
	else
	    mpd_info.msi.connected = FALSE;
    }

    mpd_play_state_update(mpd_info.obj);

    mpd_win = mpd_win_new(1, 0);

    g_signal_connect(G_OBJECT(mpd_win), "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(mpd_win), "delete_event",
                     G_CALLBACK(gtk_main_quit), NULL);

    mpd_info.update_routines = g_list_append(mpd_info.update_routines, update_info);

    mpd_info.period_funcs = g_list_append(mpd_info.period_funcs, check_update_interval);

    g_timeout_add(mpd_info.update_interval, update_info, (gpointer) mpd_info.update_interval);
    g_timeout_add(500, check_update_interval, (gpointer) mpd_info.update_interval);

    //mpd_signal_connect_status_changed(mpd_info.obj, (StatusChangedCallback)mpd_status_changed_cb, NULL);
    mpd_signal_connect_status_changed(mpd_info.obj, (StatusChangedCallback)gui_mpd_status_changed_cb, NULL);

    gtk_widget_show_all(mpd_win);

    MpdWin.window_creation_finished = 1;

    if(mpd_info.msi.connected)
        mpd_gui_show_mpd_elements();
    else
        mpd_gui_hide_mpd_elements();

    gtk_main();

    return EXIT_SUCCESS;
}

/* vim:sts=4:shiftwidth=4
 */
