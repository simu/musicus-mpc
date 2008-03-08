/**
 * \file preferences.h
 * \author Simon Gerber
 * \date 2007-12-15
 * This files provides declarations for the config management
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

#ifndef __preferences_h_
#define __preferences_h_

/**\defgroup 35config Configuration management
 * Procedures for config management */
/*@{*/

/**
 * Create a new properties window to edit configuration settings
 * \param parent The parent window, this window will be centered on
 * the parent window.
 * \return the properties window
 */
GtkWidget *mpd_new_properties_window(GtkWindow *parent);

/**
 * Callback which builds new properties window
 * \param widget the widget from where the call originates
 * \param data use data as parent window
 */
 void mpd_new_properties_window_cb(GtkWidget *widget, gpointer data);

/**
 * create an initial config
 */
gboolean config_new();

/**
 * save configuration to disk
 */
gboolean config_save();

/**
 * load config from disk
 */
gboolean config_load();

/**
 * get string for the configuration file
 * \param is_applet set TRUE if calling from a GNOME applet
 */
gboolean config_get_config_file(gboolean is_applet);

/*@}*/

#endif

/* vim:syn=c:sts=4:shiftwidth=4
 */
