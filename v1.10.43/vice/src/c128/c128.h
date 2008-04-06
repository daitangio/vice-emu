/*
 * c128.h
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef _C128_H
#define _C128_H

#define C128_PAL_CYCLES_PER_SEC  985248
#define C128_PAL_CYCLES_PER_LINE 63
#define C128_PAL_SCREEN_LINES    312
#define C128_PAL_CYCLES_PER_RFSH (C128_PAL_SCREEN_LINES \
                                 * C128_PAL_CYCLES_PER_LINE)
#define C128_PAL_RFSH_PER_SEC    (1.0 / ((double)C128_PAL_CYCLES_PER_RFSH \
                                 / (double)C128_PAL_CYCLES_PER_SEC))

#define C128_NTSC_CYCLES_PER_SEC  1022730
#define C128_NTSC_CYCLES_PER_LINE 65
#define C128_NTSC_SCREEN_LINES    263
#define C128_NTSC_CYCLES_PER_RFSH (C128_NTSC_SCREEN_LINES \
                                  * C128_NTSC_CYCLES_PER_LINE)
#define C128_NTSC_RFSH_PER_SEC    (1.0 / ((double)C128_NTSC_CYCLES_PER_RFSH   \
                                  / (double)C128_NTSC_CYCLES_PER_SEC))

#endif

