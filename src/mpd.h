/**
 * \file mpd.h
 * \author Simon Gerber
 * \date 2007-12-08
 * This file contains mpd related methods
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

#ifndef __MPD_H
#define __MPD_H
/**\defgroup 10generalmpd General MPD
 * MPD related general actions such as connection issues,
 * status changes etc.
 */
/*@{*/

#include "definitions.h"

/**
 * Checks if there is a connection to mpd
 * \param obj a pointer to a MpdObj
 * \return TRUE if connected, FALSE if not
 */
gboolean mpd_is_connected(MpdObj *obj);

/**
 * Convenience function if a MpdData list is used more than once
 * \param data a pointer to a MpdData list
 * \return a pointer to the next element in the list (keeping the current element intact)
 */
MpdData *mpd_data_get_next_keep(MpdData *data);

/**
 * Gets the currently playing song. Clients don't need to free the result
 * but must check the connection to the daemon
 * \param obj a pointer to a connected MpdObj
 * \return a pointer to the current song
 */
mpd_Song *mpd_get_current_song(MpdObj *obj);

/**
 * Gets the mpd_Connection from the supplied MpdObj
 * \param obj the MpdObj you want the connection from
 * \return the mpd_Connection associated with #obj
 */
mpd_Connection *mpd_object_get_connection(MpdObj *obj);

/**
 * A callback function which is registered with libmpd using
 * the mpd_signal_connect_status_changed() function and
 * gets called every time the status of the daemon changes
 * \param obj pointer to the calling MpdObj
 * \param what a code of the changed property
 * \param data additional data
 */
void mpd_status_changed_cb(MpdObj *obj, ChangedStatusType what, gpointer data);

/**
 * Updates the MpdPlayState fields in #mpd_info, does not check if
 * connection to mpd is ok, the caller has to ensure that.
 * \param obj a pointer to the MpdObj from which the state info should
 * be fetched
 */
void mpd_play_state_update(MpdObj *obj);

/* state and song info */
/**
 * Gets the number of entries in the MPD database
 * \return number of entries in the MPD database
 */
int mpd_get_db_length();
/**
 * Gets the number of songs in the current playlist
 * \return number of songs in the current playlist
 */
int mpd_get_pl_length();
/**
 * Get the current playlist from mpd
 * \return a pointer to a #MpdPlContainer which contains
 * the current playlist
 */

int mpd_get_current_song_pos();
/**
 * Get the current play time of the song which is playing
 * \return the current play time (the time of the song that has been played)
 */
int mpd_get_current_play_time();
/**
 * Get the total time of the current song
 * \return the total time of the current song
 */
int mpd_get_current_song_time();
/**
 * Get a string describing the current song in the following format:<br>
 * if playing||paused:
 * <ul>
 * <li>if {artist_name} != NULL: "{artist name} - {song name}"</li>
 * <li>else: "{song name}"</li>
 * </ul>
 * else: "mpd not playing"
 * \param *song a pointer to an char array of length strlen
 * \param strlen the length of the array to which song points
 * \return 0
 */
int mpd_get_current_song_string(char *song, int strlen);
/**
 * Get the title of the current song or "n/a" if no song is playing
 * \param *title a pointer to a char array of length strlen
 * \param strlen the length of the array to which title points
 * \return 0
 */
int mpd_get_current_title_string(char *title, int strlen);
/**
 * Get the album name from which current song comes or "n/a"
 * if no song is playing
 * \param *album a pointer to a char array of length strlen
 * \param strlen the length of the array to which album points
 * \return 0
 */
int mpd_get_current_album_string(char *album, int strlen);
/**
 * Get the track name from which current song comes or "n/a"
 * if no song is playing
 * \param *track a pointer to a char array of length strlen
 * \param strlen the length of the array to which album points
 * \return 0
 */
int mpd_get_current_artist_string(char *artist, int strlen);
/**
 * Get the track name of the current song or "n/a"
 * if no song is playing
 * \param *track a pointer to a char array of length strlen
 * \param strlen the length of the array to which track points
 * \return 0
 */
int mpd_get_current_track_string(char *track, int strlen);
/**
 * Returns TRUE if the current song has changed
 * \return TRUE if changed, FALSE if not
 */
gboolean mpd_has_current_song_changed();
/**
 * Returns TRUE if the playlist has changed
 * \return TRUE if changed, FALSE if not
 */
gboolean mpd_has_playlist_changed();
/**
 * Returns TRUE if the play state has changed
 * \return TRUE if changed, FALSE if not
 */
gboolean mpd_has_play_state_changed();
/**
 * Returns the play state of MPD:
 * <ul>
 *   <li>0 == stopped, whatever</li>
 *   <li>1 == playing</li>
 *   <li>2 == paused</li>
 * </ul>
 * \return the play state as indicated above
 */
int mpd_play_state();

/* mpd state info manipulation */
/**
 * fills the mi.msi field (MpdStateInfo) with
 * information obtained from the MpdObj mi.obj
 * \param mi a MpdInfo struct
 */
void msi_fill(MpdInfo *mi);

/**
 * clears the mi.msi field
 * \param mi a MpdInfo struct
 */
void msi_clear(MpdInfo *mi);

/*@}*/

#endif

/* vim:sts=4:shiftwidth=4
 */
