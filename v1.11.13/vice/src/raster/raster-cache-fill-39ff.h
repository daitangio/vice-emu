/*
 * raster-cache-fill-39ff.h - Raster line cache.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _RASTER_CACHE_FILL_39FF_H
#define _RASTER_CACHE_FILL_39FF_H

#include <string.h>

#include "types.h"

inline static int raster_cache_data_fill_39ff(BYTE *dest,
                                              const BYTE *src_base,
                                              int src_cnt,
                                              unsigned int length,
                                              int src_step,
                                              unsigned int *xs,
                                              unsigned int *xe,
                                              int no_check)
{
    if (no_check) {
        unsigned int i;

        *xs = 0;
        *xe = length - 1;

        for (i = 0; i < length; i++, src_cnt += src_step)
            dest[i] = src_base[src_cnt & 0x19ff];

        return 1;
    } else {
        unsigned int x = 0, i;

        for (i = 0; i < length && dest[i] == src_base[src_cnt & 0x19ff];
            i++, src_cnt += src_step)
            /* do nothing */ ;

        if (i < length) {
            if (*xs > i)
                *xs = i;

            for (; i < length; i++, src_cnt += src_step) {
                if (dest[i] != src_base[src_cnt & 0x19ff]) {
                    dest[i] = src_base[src_cnt & 0x19ff];
                    x = i;
                }
            }

            if (*xe < x)
                *xe = x;

            return 1;
        } else {
            return 0;
        }
    }
}

#endif

