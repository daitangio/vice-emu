/*
 * tia1551.h - 1551 tripple interface adaptor emulation.
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

#ifndef _TIA1551_H
#define _TIA1551_H

#include "types.h"

extern void tia1551_init(struct drive_context_s *drv);
extern void REGPARM3 tia1551_store(struct drive_context_s *drv, ADDRESS addr,
                                   BYTE byte);
extern BYTE REGPARM2 tia1551_read(struct drive_context_s *drv, ADDRESS addr);
extern void tia1551_reset(struct drive_context_s *drv);

extern BYTE tia1551_outputa[2], tia1551_outputb[2], tia1551_outputc[2];

#endif

