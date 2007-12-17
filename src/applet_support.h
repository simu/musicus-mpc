/**
 * \file applet_support.h
 * \author Simon Gerber
 * \date 2007-12-16
 * This file declares support functions for the applet
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

#ifndef __APPLET_SUPPORT_H
#define __APPLET_SUPPORT_H

/**\defgroup 85appletsupport Applet support functions
 * These functions are some callbacks and such for the applet
 */
/*@{*/

#include "definitions_applet.h"

/**
 * This is a callback for the applet which opens the controls menu
 * \param widget the widget from which this function was called
 * \param event_btn the event that triggered the call
 * \param data user data
 */
gboolean applet_button_pressed_cb(GtkWidget *widget, GdkEventButton *event_btn, gpointer data);


/**
 * Create the applet popup controls menu.
 */
void create_applet_menu(void);

/*@}*/

#endif // __APPLET_SUPPORT_H
