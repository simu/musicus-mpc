/**
 * \file mpd_action.h
 * \author Simon Gerber
 * \date 2007-12-15
 * This file provides a universal callback function for
 * all mpd commands which actively do something (not state
 * or information queries
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

#ifndef __MPD_ACTION_H
#define __MPD_ACTION_H

/**\defgroup 15mpdaction MPD actions
 * This module contains an universal callback function
 * which provides an interface to the various active mpd
 * commands such as play/pause
 */
/*@{*/

#include "definitions_gtk.h"

/**
 * enum MpdActionType_
 * \see #MpdActionType
 */
enum  MpdActionType_ {
	/// undefined action
	ACTION_UNDEF,
	/// toggle play/pause
	ACTION_PLAY_PAUSE,
	/// stop playback
	ACTION_STOP,
	/// next song
	ACTION_NEXT,
	/// previous song
	ACTION_PREVIOUS,
	/// open playlist window
	ACTION_PLAYLIST,
	/// update database
	ACTION_UPDATE_DB,
	/// open media browser
	ACTION_MEDIA_BROWSER,
	/// toggle random state
	ACTION_RANDOM,
	/// toggle repeat state
	ACTION_REPEAT,
	/// toggle crossfade state
	ACTION_XFADE,
	/// number of actions
	NUM_ACTIONS
};

/**
 * MpdActionType is used to communicate the
 * desired action to the action callback.<br>
 * Use the macros #GPOINTER_TO_ACTION_TYPE
 * and #ACTION_TYPE_TO_GPOINTER for conversion
 * between gpointers to ActionTypes.
 */
typedef enum MpdActionType_ MpdActionType;


/**
 * convert a gpointer to a MpdActionType
 */
#define GPOINTER_TO_ACTION_TYPE(data) ((MpdActionType)data)
/**
 * convert an ActionType to a gpointer
 */
#define ACTION_TYPE_TO_GPOINTER(data) ((gpointer)data)

/**
 * Action function prototype
 */
typedef gboolean (*ActionFunc)(void);

/**
 * The callback function
 * \param widget The widget from which this function was called
 * \param data supply a MpdActionType (use the conversion macros)
 */
void action(GtkWidget *widget, gpointer data);

/*@}*/

#endif // __MPD_ACTION_H

/* vim:sts=4:shiftwidth=4
 */
