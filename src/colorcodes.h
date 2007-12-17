/**
 * \file colorcodes.h
 * \author Simon Gerber
 * \date 2007-12-14
 * This file contains colorcode defines for the terminal
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

#ifndef __colorcodes_h
#define __colorcodes_h

/**\defgroup 99colorcodes Color codes
 * Macros for colored terminal output.
 */
/*@{*/

/* colorcodes for terminal */

/** red text color in the terminal */
#define RED "\033[31m"
/** green text color in the terminal */
#define GREEN "\033[32m"
/** yellow text color in the terminal */
#define YELLOW "\033[33m"
/** blue text color in the terminal */
#define BLUE "\033[34m"
/** violet text color in the terminal */
#define VIOLET "\033[35m"
/** turquoise text color in the terminal */
#define TURQUOISE "\033[36m"

/** bold text in the terminal */
#define BOLD "\033[1m"
/** underlined text in the terminal */
#define UNDERLINED "\033[4m"

/**
 * reset the last applied from the above formattings
 * You need to have as many NORMALs in your format
 * string as you had of the others.<br/>
 * They have to be properly nested.
 */
#define NORMAL "\033[0m"

/*@}*/
#endif
