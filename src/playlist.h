/** \file playlist.h
 * \author Simon Gerber
 * \date 2007-12-09
 * This file provides playlist related functions
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

#ifndef __PLAYLIST_H
#define __PLAYLIST_H

/**\defgroup 50playlist Playlist
 * Playlist related functions and objects
 */
/*@{*/

#include "definitions.h"

/**
 * Playlist container structure
 */
typedef struct _MpdPlContainer {
	/// handle for the array of songs (malloc'd)
	mpd_Song **list;
	/// length of the array
	int length;
	/// number of the currently played song
	int current;
} MpdPlContainer;

/**
 * allocate memory for #list in MpdPlContainer
 * \param pl_length number of entries
 * \return handle to the newly allocated memory, appropriately casted
 */
mpd_Song **allocate_pl_mem(int pl_length);
/**
 * creates a new MpdPlContainer
 * \return pointer to the newly created container
 */
MpdPlContainer *mpd_pl_container_new();
/**
 * creates a new MpdPlContainer and allocates the song list
 * \param list_length number of entries for the song list
 * \return pointer to the newly created container
 */
MpdPlContainer *mpd_pl_container_new_alloc(int list_length);
/**
 * frees a MpdPlContainer (also frees the song list, if allocated)
 * \param plc a MpdPlContainer to free
 */
void mpd_pl_container_free(MpdPlContainer *plc);

MpdPlContainer *mpd_get_pl();
/**
 * Get the position of current song in the playlist
 * \return the position of the current song
 */


/*@}*/

#endif


/* vim:sts=4:shiftwidth=4
 */
