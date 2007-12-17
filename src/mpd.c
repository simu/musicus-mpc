/**
 * \file mpd.c
 * \author Simon Gerber
 * \date 2007-12-08
 * This file provides some functions not provided by libmpd
 * and some convenience wrappers around libmpd functions as well
 * as a skeleton function for the MpdStatusChanged callback.
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

#include <stdio.h>

#include "mpd.h"

#include <libmpd/libmpd-internal.h>

/**************file globals**********************/
static int playlist_version = -1;


/**************implementation*******************/

/* wrapper function for mpd_check_connected() */
gboolean mpd_is_connected(MpdObj *obj) {
	return mpd_check_connected(obj)?TRUE:FALSE;
}

/* gets current song */
mpd_Song *mpd_get_current_song(MpdObj *obj) {
    mpd_Song *current_song;

    //mpd_update_status(obj);

    current_song = mpd_playlist_get_current_song(obj);

    return current_song;
}

/* get connection ... */
mpd_Connection *mpd_object_get_connection(MpdObj *obj) {
	_MpdObj *real_obj;
	real_obj = (_MpdObj*)obj;
	return real_obj->connection;
}

MpdData *mpd_data_get_next_keep(MpdData *data) {
    return mpd_data_get_next_real(data, FALSE);
}

/* callback function for mpd_signal_connect_status_changed()
 * standard function, a customized one for the GTK+ playlist interface
 * is in the playlist_gtk module */
void mpd_status_changed_cb(MpdObj *obj, ChangedStatusType what, gpointer data) {
    /* update mpd state */
    //player_state_update(player_state);
    mpd_play_state_update(obj);
    /* do appropritate thing for action */
    if (what&MPD_CST_PLAYLIST) { /* playlist has changed */
        fprintf(log_file, "mpd: playlist changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_SONGPOS) { /* song positon of playing song has changed */
        fprintf(log_file, "mpd: position of playing song has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_SONGID) { /* playing song has changed */
        fprintf(log_file, "mpd: playing song has changed\n");
        fflush(log_file);
    }

    if (what&MPD_CST_DATABASE) { /* database has changed */
        fprintf(log_file, "mpd: database has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_UPDATING) { /* database updating status has changed */
        fprintf(log_file, "mpd: database updating status has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_VOLUME) { /* volume has changed */
        fprintf(log_file, "mpd: volume changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_TOTAL_TIME) { /* total time of playing song has changed */
        fprintf(log_file, "mpd: total time of playing song has changed\n");
        fflush(log_file);
    }
if (what&MPD_CST_CROSSFADE) { /* crossfade status has changed */
        fprintf(log_file, "mpd: crossfade status has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_RANDOM) { /* random status has changed */
        fprintf(log_file, "mpd: random status changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_REPEAT) { /* repeat status has changed */
        fprintf(log_file, "mpd: repeat status has changed\n");
        fflush(log_file);
    }
    if (what&MPD_CST_STATE) { /* mpd state (playing/pause/...) has changed */
        fprintf(log_file, "mpd: mpd state has changed\n");
        fflush(log_file);
    }
    /* not checked:
     * MPD_CST_ELAPSED_TIME: elapsed time of current song has changed
     * MPD_CST_BITRATE: bitrate of current song has changed
     * MPD_CST_PERMISSION: permissions of client have changed
     * MPD_CST_AUDIOFORMAT: codec of current song has changed
     */
    return;
}

/* update play state */
void mpd_play_state_update(MpdObj *obj) {
	mpd_info.mps.random = mpd_player_get_random(obj)?TRUE:FALSE;
	mpd_info.mps.repeat = mpd_player_get_repeat(obj)?TRUE:FALSE;
	mpd_info.mps.xfade  = (gint)mpd_status_get_crossfade(obj);
	mpd_info.mps.update = mpd_status_db_is_updating(obj)?TRUE:FALSE;
}

/* get mpd song count */
int mpd_get_db_length() {

	int db_song_num;

	db_song_num = mpd_stats_get_total_songs(mpd_info.obj);

	if(verbose) {
		fprintf(log_file, "[%s:%3i] %s(): %i (db song count)\n", __FILE__, __LINE__, __FUNCTION__, db_song_num);
		fflush(log_file);
	}

	return db_song_num;
}

/* get mpd playlist length */
int mpd_get_pl_length() {
	int pl_length = -1;

	pl_length = mpd_playlist_get_playlist_length(mpd_info.obj);

	if(verbose) {
		fprintf(log_file, "[%s:%3i] %s(): %i (pl length)\n",__FILE__, __LINE__, __FUNCTION__, pl_length);
		fflush(log_file);
	}

	return pl_length;
}

/* get the playlist position of the current song, returns -1 if not playing, -2 on conn err*/
int mpd_get_current_song_pos() {
	return mpd_player_get_current_song_pos(mpd_info.obj);
}

/* get current time of currently played song
 * -1 on error
 */
int mpd_get_current_play_time() {

	int time;

	time = mpd_status_get_elapsed_song_time(mpd_info.obj);
	if(time < 0) {
		switch(time) {
			case MPD_NOT_CONNECTED:
				fprintf(err_file, "no connection to mpd server\n");
				break;
			case MPD_STATUS_FAILED:
				fprintf(err_file, "failed to get status\n");
				break;
			default:
				fprintf(err_file, "unknown error: error-code = %i\n", time);
		}
		fflush(err_file);
		return -1;
	}
	return time;
}

/* get total time of currently played song */
int mpd_get_current_song_time() {

	int time;

	time = mpd_status_get_total_song_time(mpd_info.obj);
	if(time < 0) {
		switch(time) {
			case MPD_ARGS_ERROR:
				fprintf(err_file, "failed to connect to mpd server\n");
				break;
			case MPD_STATUS_FAILED:
				fprintf(err_file, "failed to get status\n");
				break;
			default:
				fprintf(err_file, "unknown error: error-code = %i\n", time);
		}
		fflush(err_file);
		return -1;
	}

	return time;
}

/* returns the artist and title / name of the current song */
int mpd_get_current_song_string(char *song, int strlen) {

	mpd_Song *current_song;
	MpdState state;

	mpd_status_update(mpd_info.obj);

	state = mpd_player_get_state(mpd_info.obj);

	/* playing or paused, so there is a current song */
	if(
		(state == MPD_PLAYER_PLAY) ||
		(state == MPD_PLAYER_PAUSE)
	  )
	{
		current_song = mpd_playlist_get_current_song(mpd_info.obj);
		if(current_song->title) {
			snprintf(song, strlen, "%s - %s", current_song->artist, current_song->title);
		}
		else {
			snprintf(song, strlen, "%s", current_song->name);
			if(verbose) {
				fprintf(log_file, "[%s:%3i] %s(): song = %s\n", __FILE__, __LINE__, __FUNCTION__, song);
				fflush(log_file);
			}
		}
	}
	else {	/* some other state, probably stopped => no current song */
		snprintf(song, strlen, "mpd not playing");
	}

	return 0;
}

/* returns the current title name */
int mpd_get_current_title_string(char *title, int strlen) {

	mpd_Song *song;

	song = mpd_get_current_song(mpd_info.obj);

	if(song != NULL) {

		snprintf(title, strlen, "%s", song->title);

	}
	else {
		snprintf(title, strlen, "n/a");
	}

	return 0;
}

/* returns the current album name */
int mpd_get_current_album_string(char *album, int strlen) {

	mpd_Song *current_song;

	current_song = mpd_get_current_song(mpd_info.obj);

	if(current_song != NULL) {

		snprintf(album, strlen, "%s", current_song->album);

	}
	else {
		snprintf(album, strlen, "n/a");
	}

	return 0;
}

int mpd_get_current_track_string(char *track, int strlen) {

	mpd_Song *current_song;

	current_song = mpd_get_current_song(mpd_info.obj);

	if(current_song != NULL) {
		snprintf(track, strlen, "%s", current_song->track);
	}
	else {
		snprintf(track, strlen, "n/a");
	}

	return 0;
}

/* returns the current artist name */
int mpd_get_current_artist_string(char *artist, int strlen) {

	mpd_Song *current_song;

	current_song = mpd_get_current_song(mpd_info.obj);

	if(current_song != NULL) {

		snprintf(artist, strlen, "%s", current_song->artist);

	}
	else {
		snprintf(artist, strlen, "n/a");
	}

	return 0;
}

/* returns TRUE if the song has changed, FALSE otherwise */
int pl_current_song_pos = -64;
gboolean mpd_has_current_song_changed() {

	mpd_Song *current_song;
	int pl_pos;

	pl_pos = -64;
	current_song = mpd_get_current_song(mpd_info.obj);
	if(current_song == NULL) {
		pl_pos = -1;
	}
	else {
		pl_pos = current_song->pos;
	}
	if(pl_current_song_pos == pl_pos) {
		return FALSE;
	}
	pl_current_song_pos = pl_pos;
	return TRUE;
}

/* returns TRUE if the play state has changed, FALSE otherwise */
int mpd_current_play_state = -64;
gboolean mpd_has_play_state_changed() {

	MpdState state;

	mpd_status_update(mpd_info.obj);

	state = mpd_player_get_state(mpd_info.obj);

	int play_state = -64;
	if(state == MPD_PLAYER_PLAY)
		play_state = 1;
	else if(state == MPD_PLAYER_PAUSE)
		play_state = 2;
	else
		play_state = 0;
	if(mpd_current_play_state == play_state) {
		return FALSE;
	}
	mpd_current_play_state = play_state;
	return TRUE;
}

/* returns True if playlist has changed */
gboolean mpd_has_playlist_changed() {

	int pl_version = -1;

	mpd_status_update(mpd_info.obj);

	pl_version = mpd_playlist_get_playlist_id(mpd_info.obj);

	if(pl_version == playlist_version) {
		return FALSE;
	}
	playlist_version = pl_version;
	return TRUE;
}

/* returns the play state of mpd: 0 stopped, whatever; 1 playing; 2 paused */
int mpd_play_state() {
	int state = 0;
	/* debug off */
	int old_debug = debug; debug = 0;
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s() called\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}

	MpdState mpd_state;

	mpd_status_update(mpd_info.obj);

	mpd_state = mpd_player_get_state(mpd_info.obj);

	if(mpd_state == MPD_PLAYER_PLAY)
		state = 1;
	else if(mpd_state == MPD_PLAYER_PAUSE)
		state = 2;
	if(debug) {
		fprintf(log_file, "[%s:%3i] %s(): before mpd_freeStatus()\n", __FILE__, __LINE__, __FUNCTION__);
		fflush(log_file);
	}

	/* reset debug state: */
	debug = old_debug;
	return state;
}

void msi_fill(MpdInfo *mi) {

    if(mi->msi.host != NULL) g_string_free(mi->msi.host, TRUE);
    mi->msi.host = g_string_new(mi->obj->hostname);
    mi->msi.port = mi->obj->port;
    if(mi->msi.pw != NULL) g_string_free(mi->msi.pw, TRUE);
    mi->msi.pw = g_string_new(mi->obj->password);

    return;
}

void msi_clear(MpdInfo *mi) {

    if(mi->msi.host != NULL) {
        g_string_erase(mi->msi.host, 0, -1);
        g_string_insert(mi->msi.host, 0, "n/a");
    }
    else {
        mi->msi.host = g_string_new("n/a");
    }
    if(mi->msi.pw != NULL) {
        g_string_erase(mi->msi.pw, 0, -1);
        g_string_insert(mi->msi.pw, 0, "n/a");
    }
    else {
        mi->msi.pw = g_string_new("n/a");
    }
    mi->msi.port = -1;

    return;
}

/* vim:sts=4:shiftwidth=4
 */
