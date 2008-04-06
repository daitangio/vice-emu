/*
 * reu.h - REU 1750 emulation.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Richard Hable <K3027E7@edvz.uni-linz.ac.at>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _REU_H
#define _REU_H

#include "types.h"

struct snapshot_s;

extern void reu_init(void);
extern int reu_reset(int size);
extern void reu_activate(void);
extern void reu_deactivate(void);
extern void close_reu(void);
extern BYTE REGPARM1 reu_read(ADDRESS addr);
extern void REGPARM2 reu_store(ADDRESS addr, BYTE byte);
extern void reu_dma(int immed);
extern int reu_read_snapshot_module(struct snapshot_s *s);
extern int reu_write_snapshot_module(struct snapshot_s *s);

#endif

