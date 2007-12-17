/**
 * \file search.h
 * \author Simon Gerber
 * \date 2007-12-15
 * This file contains the procedures to build a search for songs
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

#ifndef __SEARCH_H
#define __SEARCH_H

/**\defgroup 52search Search Database
 * Search Database for Song / Artist / Title */
/*@{*/

#include "definitions_gtk.h"

/**
 * initialize search widget
 * \param parent_window the parent window
 * \return handle for the new widget
 */
GtkWidget *init_search_widget(GtkWidget *parent_window);

/*@}*/
#endif // __SEARCH_H

/* vim:sts=4:shiftwidth=4
 */
