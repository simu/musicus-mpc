/**
 * \file playlist.c
 * \author Simon Gerber
 * \date 2007-12-15
 * This file provides basic playlist functionality
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

/**************includes**************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdarg.h>

#include "playlist.h"
#include "mpd.h"
/* #include "support.h" */
/* #include "callbacks.h" * */



/**************implementation********************/

/* return memory for a playlist of pl_length songs */
mpd_Song **allocate_pl_mem(int pl_length) {
	return malloc(pl_length*sizeof(mpd_Song*));
}

/* return memory for a MpdPlContainer */
MpdPlContainer *mpd_pl_container_new() {
	return malloc(sizeof(MpdPlContainer));
}

/* build a new MpdPlContainer and allocate memory for the playlist array */
MpdPlContainer *mpd_pl_container_new_alloc(int list_length) {
	MpdPlContainer *plc = mpd_pl_container_new();
	plc->list = allocate_pl_mem(list_length);
	plc->length = list_length;
	// we don't know the current song yet
	plc->current = -1;
	return plc;
}

/* free a MpdPlContainer */
void mpd_pl_container_free(MpdPlContainer *plc) {
	int i;
	// free mpd_Songs
	if(plc->list != NULL) {
		for(i=0;(i<plc->length);++i)
			if(plc->list[i] != NULL)
                mpd_freeSong(plc->list[i]);
		free(plc->list);
	}
	free(plc);
	return;
}

/* get mpd playlist */

MpdPlContainer *mpd_get_pl() {
	MpdPlContainer *plc = mpd_pl_container_new();
	MpdData *pl_data;
	int i;

	// -1 -> fetch a new playlist
	if(mpd_info.msi.connected) {
	    if(!mpd_check_connected(mpd_info.obj)) {
		if(mpd_connect(mpd_info.obj)!=MPD_OK) {
		    msi_clear(&mpd_info);
		    mpd_info.msi.connected = FALSE;
		    return NULL;
		}
		else {
		    msi_fill(&mpd_info);
		    mpd_info.msi.connected = TRUE;
		}
	    }
	}
	pl_data = mpd_playlist_get_changes(mpd_info.obj, -1);

	// return NULL if no songs in playlist
	if(pl_data == NULL)
		return NULL;

	// allocate storage for pl
	plc->length = mpd_get_pl_length();
	plc->list = allocate_pl_mem(plc->length);

	// put MpdData in pl
	mpd_data_get_first(pl_data);
	i = 0;
	while(pl_data != NULL) {
		// guaranteed only to have songs in the MpdData list
		plc->list[i++] = mpd_songDup(pl_data->song);
		pl_data = mpd_data_get_next(pl_data);
	}

	// set current song
	plc->current = mpd_get_current_song_pos();

	return plc;
}

/* vim:sts=4:shiftwidth=4
 */
