/*
 * c128mmu.h
 *
 * Written by
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

#ifndef _C128MMU_H
#define _C128MMU_H

#include "types.h"

extern BYTE REGPARM1 mmu_read(ADDRESS addr);
extern void REGPARM2 mmu_store(ADDRESS address, BYTE value);
extern BYTE REGPARM1 mmu_ffxx_read(ADDRESS addr);
extern void REGPARM2 mmu_ffxx_store(ADDRESS addr, BYTE value);

extern void mmu_reset(void);

extern void mmu_init(void);
extern int mmu_init_resources(void);
extern int mmu_init_cmdline_options(void);

#endif

