/**
 * \file mpd_action.c
 * \author Simon Gerber
 * \date 2007-12-15
 * This file provides the implementation for an universal
 * callback function for all "action" mpd commands such as
 * play/pause or toggle random/repeat .
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

#include "mpd_action.h"
#include "mpd.h"


/************** local functions **************/
static gboolean mpd_undef(void);
static gboolean mpd_play_pause(void);
static gboolean mpd_stop(void);
static gboolean mpd_next(void);
static gboolean mpd_prev(void);
static gboolean mpd_playlist(void);
static gboolean mpd_update_db(void);
static gboolean mpd_media_browser(void);
static gboolean mpd_random(void);
static gboolean mpd_repeat(void);
static gboolean mpd_xfade(void);

/************** local variables **************/
/**
 * Action function array
 * indexed with corresponding ActionType
 */
static ActionFunc actions[] = {
	mpd_undef,
	mpd_play_pause,
	mpd_stop,
	mpd_next,
	mpd_prev,
	mpd_playlist,
	mpd_update_db,
	mpd_media_browser,
	mpd_random,
	mpd_repeat,
	mpd_xfade
};
static int last_song = -1;

/************** implementation **************/

void action(GtkWidget *widget, gpointer data) {

	MpdActionType what = GPOINTER_TO_ACTION_TYPE(data);

	if(mpd_info.msi.connected) {
	    if(!mpd_check_connected(mpd_info.obj)) {
		if(mpd_connect(mpd_info.obj) != MPD_OK) {
		    msi_clear(&mpd_info);
		    mpd_info.msi.connected = FALSE;
		}
		else {
		    msi_fill(&mpd_info);
		    mpd_info.msi.connected = TRUE;
		}
	    }
	}

	if((what<NUM_ACTIONS) && (what>0)) {
		(*actions[what])();
	}
	else {
		fputs("this is no valid action code", err_file);
		fflush(err_file);
	}
	return;
}

static gboolean mpd_undef(void) {
	fprintf(err_file, "action: undefined action\n");
	fflush(err_file);
	return TRUE;
}

static gboolean mpd_play_pause(void) {
	MpdState state = mpd_player_get_state(mpd_info.obj);
	int retcode;
	if((state == MPD_PLAYER_PLAY)||(state==MPD_PLAYER_PAUSE)) {
		// toggle pause state
		retcode = mpd_player_pause(mpd_info.obj);
		if(retcode != MPD_OK) {
			fprintf(err_file, "action play/pause: error: %d\n", retcode);
			fflush(err_file);
			return FALSE;
		}
	}
	else if(last_song == -1) {
		retcode = mpd_player_play(mpd_info.obj); // play first song
		if(retcode != MPD_OK) {
			fprintf(err_file, "action play: error: %d\n", retcode);
			fflush(err_file);
			return FALSE;
		}
	}
	else {
		retcode = mpd_player_play_id(mpd_info.obj, last_song);
		if(retcode != MPD_OK) {
			fprintf(err_file, "action play_id: error: %d\n", retcode);
			fflush(err_file);
			return FALSE;
		}
	}
	return TRUE;
}

static gboolean mpd_stop(void) {
	int retcode;
	int song_id = mpd_player_get_current_song_id(mpd_info.obj);

	/* save last song */
	last_song = song_id;
	retcode = mpd_player_stop(mpd_info.obj);
	if(retcode != MPD_OK) {
		fprintf(err_file, "action stop: error: %d\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

static gboolean mpd_next(void) {
	int retcode;
	retcode = mpd_player_next(mpd_info.obj);
	if(retcode != MPD_OK) {
		fprintf(err_file, "action next: error: %d\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

static gboolean mpd_prev(void) {
	int retcode;
	retcode = mpd_player_prev(mpd_info.obj);
	if(retcode != MPD_OK) {
		fprintf(err_file, "action prev: error: %d\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

static gboolean mpd_playlist(void) {
	fputs("playlist: not yet implemented", log_file);
	fflush(log_file);
	return TRUE;
}

static gboolean mpd_update_db(void) {
	int retcode;
	retcode = mpd_database_update_dir(mpd_info.obj, "/");
	if(retcode != MPD_OK) {
		fprintf(err_file, "action update db: error: %i\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

static gboolean mpd_media_browser(void) {
	fputs("media browser: not yet implemented", log_file);
	fflush(log_file);
	return TRUE;
}

static gboolean mpd_random(void) {
	int retcode;
	if(mpd_info.mps.random) {
		retcode = mpd_player_set_random(mpd_info.obj, 0);
		mpd_info.mps.random = 0;
	}
	else {
		retcode = mpd_player_set_random(mpd_info.obj, 1);
		mpd_info.mps.random = 1;
	}
	if(retcode != MPD_OK) {
		fprintf(err_file, "action random: error: %i\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

static gboolean mpd_repeat(void) {
	int retcode;
	if(mpd_info.mps.repeat) {
		retcode = mpd_player_set_repeat(mpd_info.obj, 0);
		mpd_info.mps.repeat = 0;
	}
	else {
		retcode = mpd_player_set_repeat(mpd_info.obj, 1);
		mpd_info.mps.repeat = 1;
	}
	if(retcode != MPD_OK) {
		fprintf(err_file, "action repeat: error: %i\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

static gboolean mpd_xfade(void) {
	int retcode;
	if(mpd_info.mps.xfade) {
		retcode = mpd_status_set_crossfade(mpd_info.obj, 0);
		mpd_info.mps.xfade = 0;
	}
	else {
		retcode = mpd_status_set_crossfade(mpd_info.obj, mpd_info.xfade_secs);
		mpd_info.mps.xfade = mpd_info.xfade_secs;
	}
	if(retcode != MPD_OK) {
		fprintf(err_file, "action xfade: error: %i\n", retcode);
		fflush(err_file);
		return FALSE;
	}
	return TRUE;
}

/* vim:sts=4:shiftwidth=4
 */
