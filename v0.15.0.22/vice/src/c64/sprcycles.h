/*
 * sprcycles.h - Defines sprite timing for the MOS6569 (VIC-II) chip.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _SPRCYCLES_H
#define _SPRCYCLES_H

/* This defines the stolen sprite cycles for all the values of `dma_msk'. */
/* The table derives from what Christian Bauer (bauec002@physik.uni-mainz.de)
   says in both the `VIC-Article' and Frodo's VIC_SC.cpp source file. */

struct sprite_fetch {
    int cycle, num;
    int first, last;
};

struct sprite_fetch sprite_fetch_tab[256][4] = {
    /* $00 */ { { -1, -1 } },
    /* $01 */ { { 0, 5, 0, 0 },  { -1, -1 } },
    /* $02 */ { { 2, 5, 1, 1 },  { -1, -1 } },
    /* $03 */ { { 0, 7, 0, 1 },  { -1, -1 } },
    /* $04 */ { { 4, 5, 2, 2 },  { -1, -1 } },
    /* $05 */ { { 0, 9, 0, 2 },  { -1, -1 } },
    /* $06 */ { { 2, 7, 1, 2 },  { -1, -1 } },
    /* $07 */ { { 0, 9, 0, 2 },  { -1, -1 } },
    /* $08 */ { { 6, 5, 3, 3 },  { -1, -1 } },
    /* $09 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { -1, -1 } },
    /* $0A */ { { 2, 9, 1, 3 },  { -1, -1 } },
    /* $0B */ { { 0, 11, 0, 3 },  { -1, -1 } },
    /* $0C */ { { 4, 7, 2, 3 },  { -1, -1 } },
    /* $0D */ { { 0, 11, 0, 3 },  { -1, -1 } },
    /* $0E */ { { 2, 9, 1, 3 },  { -1, -1 } },
    /* $0F */ { { 0, 11, 0, 3 },  { -1, -1 } },
    /* $10 */ { { 8, 5, 4, 4 },  { -1, -1 } },
    /* $11 */ { { 0, 5, 0, 0 },  { 8, 5, 4, 4 },  { -1, -1 } },
    /* $12 */ { { 2, 5, 1, 1 },  { 8, 5, 4, 4 },  { -1, -1 } },
    /* $13 */ { { 0, 7, 0, 1 },  { 8, 5, 4, 4 },  { -1, -1 } },
    /* $14 */ { { 4, 9, 2, 4 },  { -1, -1 } },
    /* $15 */ { { 0, 13, 0, 4 },  { -1, -1 } },
    /* $16 */ { { 2, 11, 1, 4 },  { -1, -1 } },
    /* $17 */ { { 0, 13, 0, 4 },  { -1, -1 } },
    /* $18 */ { { 6, 7, 3, 4 },  { -1, -1 } },
    /* $19 */ { { 0, 5, 0, 0 },  { 6, 7, 3, 4 },  { -1, -1 } },
    /* $1A */ { { 2, 11, 1, 4 },  { -1, -1 } },
    /* $1B */ { { 0, 13, 0, 4 },  { -1, -1 } },
    /* $1C */ { { 4, 9, 2, 4 },  { -1, -1 } },
    /* $1D */ { { 0, 13, 0, 4 },  { -1, -1 } },
    /* $1E */ { { 2, 11, 1, 4 },  { -1, -1 } },
    /* $1F */ { { 0, 13, 0, 4 },  { -1, -1 } },
    /* $20 */ { { 10, 5, 5, 5 },  { -1, -1 } },
    /* $21 */ { { 0, 5, 0, 0 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $22 */ { { 2, 5, 1, 1 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $23 */ { { 0, 7, 0, 1 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $24 */ { { 4, 5, 2, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $25 */ { { 0, 9, 0, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $26 */ { { 2, 7, 1, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $27 */ { { 0, 9, 0, 2 },  { 10, 5, 5, 5 },  { -1, -1 } },
    /* $28 */ { { 6, 9, 3, 5 },  { -1, -1 } },
    /* $29 */ { { 0, 5, 0, 0 },  { 6, 9, 3, 5 },  { -1, -1 } },
    /* $2A */ { { 2, 13, 1, 5 },  { -1, -1 } },
    /* $2B */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $2C */ { { 4, 11, 2, 5 },  { -1, -1 } },
    /* $2D */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $2E */ { { 2, 13, 1, 5 },  { -1, -1 } },
    /* $2F */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $30 */ { { 8, 7, 4, 5 },  { -1, -1 } },
    /* $31 */ { { 0, 5, 0, 0 },  { 8, 7, 4, 5 },  { -1, -1 } },
    /* $32 */ { { 2, 5, 1, 1 },  { 8, 7, 4, 5 },  { -1, -1 } },
    /* $33 */ { { 0, 7, 0, 1 },  { 8, 7, 4, 5 },  { -1, -1 } },
    /* $34 */ { { 4, 11, 2, 5 },  { -1, -1 } },
    /* $35 */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $36 */ { { 2, 13, 1, 5 },  { -1, -1 } },
    /* $37 */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $38 */ { { 6, 9, 3, 5 },  { -1, -1 } },
    /* $39 */ { { 0, 5, 0, 0 },  { 6, 9, 3, 5 },  { -1, -1 } },
    /* $3A */ { { 2, 13, 1, 5 },  { -1, -1 } },
    /* $3B */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $3C */ { { 4, 11, 2, 5 },  { -1, -1 } },
    /* $3D */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $3E */ { { 2, 13, 1, 5 },  { -1, -1 } },
    /* $3F */ { { 0, 15, 0, 5 },  { -1, -1 } },
    /* $40 */ { { 12, 5, 6, 6 },  { -1, -1 } },
    /* $41 */ { { 0, 5, 0, 0 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $42 */ { { 2, 5, 1, 1 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $43 */ { { 0, 7, 0, 1 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $44 */ { { 4, 5, 2, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $45 */ { { 0, 9, 0, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $46 */ { { 2, 7, 1, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $47 */ { { 0, 9, 0, 2 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $48 */ { { 6, 5, 3, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $49 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $4A */ { { 2, 9, 1, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $4B */ { { 0, 11, 0, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $4C */ { { 4, 7, 2, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $4D */ { { 0, 11, 0, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $4E */ { { 2, 9, 1, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $4F */ { { 0, 11, 0, 3 },  { 12, 5, 6, 6 },  { -1, -1 } },
    /* $50 */ { { 8, 9, 4, 6 },  { -1, -1 } },
    /* $51 */ { { 0, 5, 0, 0 },  { 8, 9, 4, 6 },  { -1, -1 } },
    /* $52 */ { { 2, 5, 1, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
    /* $53 */ { { 0, 7, 0, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
    /* $54 */ { { 4, 13, 2, 6 },  { -1, -1 } },
    /* $55 */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $56 */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $57 */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $58 */ { { 6, 11, 3, 6 },  { -1, -1 } },
    /* $59 */ { { 0, 5, 0, 0 },  { 6, 11, 3, 6 },  { -1, -1 } },
    /* $5A */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $5B */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $5C */ { { 4, 13, 2, 6 },  { -1, -1 } },
    /* $5D */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $5E */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $5F */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $60 */ { { 10, 7, 5, 6 },  { -1, -1 } },
    /* $61 */ { { 0, 5, 0, 0 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $62 */ { { 2, 5, 1, 1 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $63 */ { { 0, 7, 0, 1 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $64 */ { { 4, 5, 2, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $65 */ { { 0, 9, 0, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $66 */ { { 2, 7, 1, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $67 */ { { 0, 9, 0, 2 },  { 10, 7, 5, 6 },  { -1, -1 } },
    /* $68 */ { { 6, 11, 3, 6 },  { -1, -1 } },
    /* $69 */ { { 0, 5, 0, 0 },  { 6, 11, 3, 6 },  { -1, -1 } },
    /* $6A */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $6B */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $6C */ { { 4, 13, 2, 6 },  { -1, -1 } },
    /* $6D */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $6E */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $6F */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $70 */ { { 8, 9, 4, 6 },  { -1, -1 } },
    /* $71 */ { { 0, 5, 0, 0 },  { 8, 9, 4, 6 },  { -1, -1 } },
    /* $72 */ { { 2, 5, 1, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
    /* $73 */ { { 0, 7, 0, 1 },  { 8, 9, 4, 6 },  { -1, -1 } },
    /* $74 */ { { 4, 13, 2, 6 },  { -1, -1 } },
    /* $75 */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $76 */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $77 */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $78 */ { { 6, 11, 3, 6 },  { -1, -1 } },
    /* $79 */ { { 0, 5, 0, 0 },  { 6, 11, 3, 6 },  { -1, -1 } },
    /* $7A */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $7B */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $7C */ { { 4, 13, 2, 6 },  { -1, -1 } },
    /* $7D */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $7E */ { { 2, 15, 1, 6 },  { -1, -1 } },
    /* $7F */ { { 0, 17, 0, 6 },  { -1, -1 } },
    /* $80 */ { { 14, 5, 7, 7 },  { -1, -1 } },
    /* $81 */ { { 0, 5, 0, 0 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $82 */ { { 2, 5, 1, 1 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $83 */ { { 0, 7, 0, 1 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $84 */ { { 4, 5, 2, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $85 */ { { 0, 9, 0, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $86 */ { { 2, 7, 1, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $87 */ { { 0, 9, 0, 2 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $88 */ { { 6, 5, 3, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $89 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $8A */ { { 2, 9, 1, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $8B */ { { 0, 11, 0, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $8C */ { { 4, 7, 2, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $8D */ { { 0, 11, 0, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $8E */ { { 2, 9, 1, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $8F */ { { 0, 11, 0, 3 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $90 */ { { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $91 */ { { 0, 5, 0, 0 },  { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $92 */ { { 2, 5, 1, 1 },  { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $93 */ { { 0, 7, 0, 1 },  { 8, 5, 4, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $94 */ { { 4, 9, 2, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $95 */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $96 */ { { 2, 11, 1, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $97 */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $98 */ { { 6, 7, 3, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $99 */ { { 0, 5, 0, 0 },  { 6, 7, 3, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $9A */ { { 2, 11, 1, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $9B */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $9C */ { { 4, 9, 2, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $9D */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $9E */ { { 2, 11, 1, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $9F */ { { 0, 13, 0, 4 },  { 14, 5, 7, 7 },  { -1, -1 } },
    /* $A0 */ { { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A1 */ { { 0, 5, 0, 0 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A2 */ { { 2, 5, 1, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A3 */ { { 0, 7, 0, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A4 */ { { 4, 5, 2, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A5 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A6 */ { { 2, 7, 1, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A7 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $A8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
    /* $A9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
    /* $AA */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $AB */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $AC */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $AD */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $AE */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $AF */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $B0 */ { { 8, 11, 4, 7 },  { -1, -1 } },
    /* $B1 */ { { 0, 5, 0, 0 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $B2 */ { { 2, 5, 1, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $B3 */ { { 0, 7, 0, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $B4 */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $B5 */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $B6 */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $B7 */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $B8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
    /* $B9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
    /* $BA */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $BB */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $BC */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $BD */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $BE */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $BF */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $C0 */ { { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C1 */ { { 0, 5, 0, 0 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C2 */ { { 2, 5, 1, 1 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C3 */ { { 0, 7, 0, 1 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C4 */ { { 4, 5, 2, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C5 */ { { 0, 9, 0, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C6 */ { { 2, 7, 1, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C7 */ { { 0, 9, 0, 2 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C8 */ { { 6, 5, 3, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $C9 */ { { 0, 5, 0, 0 },  { 6, 5, 3, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $CA */ { { 2, 9, 1, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $CB */ { { 0, 11, 0, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $CC */ { { 4, 7, 2, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $CD */ { { 0, 11, 0, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $CE */ { { 2, 9, 1, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $CF */ { { 0, 11, 0, 3 },  { 12, 7, 6, 7 },  { -1, -1 } },
    /* $D0 */ { { 8, 11, 4, 7 },  { -1, -1 } },
    /* $D1 */ { { 0, 5, 0, 0 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $D2 */ { { 2, 5, 1, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $D3 */ { { 0, 7, 0, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $D4 */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $D5 */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $D6 */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $D7 */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $D8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
    /* $D9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
    /* $DA */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $DB */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $DC */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $DD */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $DE */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $DF */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $E0 */ { { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E1 */ { { 0, 5, 0, 0 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E2 */ { { 2, 5, 1, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E3 */ { { 0, 7, 0, 1 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E4 */ { { 4, 5, 2, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E5 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E6 */ { { 2, 7, 1, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E7 */ { { 0, 9, 0, 2 },  { 10, 9, 5, 7 },  { -1, -1 } },
    /* $E8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
    /* $E9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
    /* $EA */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $EB */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $EC */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $ED */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $EE */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $EF */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $F0 */ { { 8, 11, 4, 7 },  { -1, -1 } },
    /* $F1 */ { { 0, 5, 0, 0 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $F2 */ { { 2, 5, 1, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $F3 */ { { 0, 7, 0, 1 },  { 8, 11, 4, 7 },  { -1, -1 } },
    /* $F4 */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $F5 */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $F6 */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $F7 */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $F8 */ { { 6, 13, 3, 7 },  { -1, -1 } },
    /* $F9 */ { { 0, 5, 0, 0 },  { 6, 13, 3, 7 },  { -1, -1 } },
    /* $FA */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $FB */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $FC */ { { 4, 15, 2, 7 },  { -1, -1 } },
    /* $FD */ { { 0, 19, 0, 7 },  { -1, -1 } },
    /* $FE */ { { 2, 17, 1, 7 },  { -1, -1 } },
    /* $FF */ { { 0, 19, 0, 7 },  { -1, -1 } } };
    
#endif /* _SPRCYCLES_H */
