/**
 * \file support.h
 * \author Simon Gerber
 * \date 2007-12-09
 * This file has support routines
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

#ifndef __SUPPORT_H
#define __SUPPORT_H

/**\defgroup 30Support Support
 * Support routines for Musicus
 */
/*@{*/

#ifdef APPLET
#include "definitions_applet.h"
#else
#include "definitions_gtk.h"
#endif

#include "playlist.h"

/**
 * This function creates a TreeStore from a playlist
 * \param plc the playlist
 * \return the new TreeStore
 */
GtkTreeStore *pl_create_tree_store(MpdPlContainer *plc);

/**
 * This function creates a ListStore from a playlist
 * \param plc the playlist
 * \return the new ListStore
 */
GtkListStore *pl_create_list_store(MpdPlContainer *list);

/**
 * Create a TreeView for the supplied TreeModel
 * \param model the TreeModel for which to generate a TreeView
 * \return a TreeView widget
 */
GtkWidget *pl_create_tree_view(GtkTreeModel *model);

/**
 * Quit callback
 * \param widget Widget from which the call originated
 * \param data the user data
 */
void pl_win_quit(GtkWidget *widget, gpointer data);

/**
 * Delete the GtkWidget pointed to by widget. Install this
 * function by calling g_signal_connect_swapped
 * \param widget the widget to delete
 * \param data not used
 */
void delete_widget(GtkWidget *widget, gpointer data);

/**
 * Dummy procedure for things which are not yet implemented
 */
void dummy();

/**
 * Callback wrapper for gtk_main_quit
 * \param widget Widget from which the call originated
 * \param data the user data
 */
void quit_procedure(GtkWidget *widget, gpointer data);

/**
 * periodic function for updating title and statusbar
 * \param data user data use for update_interval, it is used
 * to check for a changed update interval
 */
gboolean update_info(gpointer data);

/**
 * periodic function which checks if update_interval changes
 * and if it does restarts periodic functions in mpd_info.update_routines
 * is probably best called in an interval between 500 and 1000 ms
 * \param data user data: used on startup to initialize old_interval
 */
gboolean check_update_interval(gpointer data);

/**
 * convenience wrapper for
 * \code
 * fprintf(out, format, ...);
 * fflush(out);
 * \endcode
 * \param out the file to write to
 * \param format the format specifier (like printf)
 * \param ... variables as in printf
 * check man 3 printf for more documentation on the format specifier
 */
void debug_fprintf(FILE *out, char *format, ...);

/**
 * get the data dir for the config file<br/>
 * (~/.musicus/ for the standalone version,<br/>
 *  ~/.gnome2/musicus/ for the applet)
 * \param dir a buffer for the result
 * \param len the length of dir
 * \param is_applet TRUE if the dir for the applet is wanted
 * \return FALSE on error, TRUE if all went well
 */
gboolean get_data_dir(gchar *dir,int len,gboolean is_applet);

/**
 * Add an item to the menu #menu.
 * \param menu the menu to which to add the item
 * \param label the label of the item
 * \param callback the callback function
 * \param data any user data
 */
void menu_add_entry(GtkContainer *menu, gchar *label, GCallback callback, gpointer data);

/**
 * Add an item with stock icon to the menu #menu.
 * \param menu the menu to which to add the item
 * \param label the label of the item
 * \param stock_id the string for the stock icon
 * \param callback the callback function
 * \param data any user data
 */
GtkWidget *menu_add_entry_stock_icon(GtkContainer *menu, gchar *label, const gchar *stock_id, GCallback callback, gpointer data);

/**
 * Add a separator to the menu #menu
 * \param menu the menu to which to add the separator
 */
void menu_add_separator(GtkContainer *menu);

/**
 * Add an item to the MenuShell #menu.
 * \param menu the MenuShell to which to add the item
 * \param label the label of the item
 * \param callback the callback function
 * \param data any user data
 */
void menu_shell_add_entry(GtkMenuShell *menu, gchar *label, GCallback callback, gpointer data);

/**
 * Add an item with stock icon to the MenuShell #menu.
 * \param menu the MenuShell to which to add the item
 * \param label the label of the item
 * \param stock_id the string for the stock icon
 * \param callback the callback function
 * \param data any user data
 */
GtkWidget *menu_shell_add_entry_stock_icon(GtkMenuShell *menu, gchar *label, const gchar *stock_id, GCallback callback, gpointer data);

/**
 * Add a separator to the menu #menu
 * \param menu the MenuShell to which to add the separator
 */
void menu_shell_add_separator(GtkMenuShell *menu);

/**
 * print window size_info_entity
 * usage:<br>
 * \code
 * g_callback(G_OBJECT(obj), "size-allocate",
 *            G_CALLBACK(print_size), NULL);
 * \endcode
 * \param widget the widget it is called from
 * \param allocation the allocation info (size/pos) on screen
 * \param data user data
 */
void print_size(GtkWidget *widget, GtkAllocation *allocation, gpointer data);

/*@}*/

#endif

/* vim:sts=4:shiftwidth=4
 */
