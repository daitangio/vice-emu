/*
 * asm6809.c - 68090 Assembler-related utility functions.
 *
 * Written by
 *  Olaf Seibert <rhialto@falu.nl>
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

#include <stdlib.h>
#include "vice.h"

#include "asm.h"
#include "mon_assemble.h"
#include "mon_register.h"
#include "montypes.h"
#include "types.h"

static const int addr_mode_size[] = {
    1, /* ASM_ADDR_MODE_IMPLIED */
    -1,/* ASM_ADDR_MODE_ACCUMULATOR */
    -1,/* ASM_ADDR_MODE_IMMEDIATE */
    -1,/* ASM_ADDR_MODE_ZERO_PAGE */
    -1,/* ASM_ADDR_MODE_ZERO_PAGE_X */
    -1,/* ASM_ADDR_MODE_ZERO_PAGE_Y */
    -1,/* ASM_ADDR_MODE_ABSOLUTE */
    -1,/* ASM_ADDR_MODE_ABSOLUTE_X */
    -1,/* ASM_ADDR_MODE_ABSOLUTE_Y */
    -1,/* ASM_ADDR_MODE_ABS_INDIRECT */
    -1,/* ASM_ADDR_MODE_INDIRECT_X */
    -1,/* ASM_ADDR_MODE_INDIRECT_Y */
    -1,/* ASM_ADDR_MODE_RELATIVE */
       /* more modes needed for z80 */
    -1,/* ASM_ADDR_MODE_ABSOLUTE_A,*/
    -1,/* ASM_ADDR_MODE_ABSOLUTE_HL, */
    -1,/* ASM_ADDR_MODE_ABSOLUTE_IX,*/
    -1,/* ASM_ADDR_MODE_ABSOLUTE_IY,*/
    -1,/* ASM_ADDR_MODE_ABS_INDIRECT_ZP, */
    -1,/* ASM_ADDR_MODE_IMMEDIATE_16, */
    -1,/* ASM_ADDR_MODE_REG_B, */
    -1,/* ASM_ADDR_MODE_REG_C, */
    -1,/* ASM_ADDR_MODE_REG_D, */
    -1,/* ASM_ADDR_MODE_REG_E, */
    -1,/* ASM_ADDR_MODE_REG_H, */
    -1,/* ASM_ADDR_MODE_REG_IXH, */
    -1,/* ASM_ADDR_MODE_REG_IYH, */
    -1,/* ASM_ADDR_MODE_REG_L, */
    -1,/* ASM_ADDR_MODE_REG_IXL, */
    -1,/* ASM_ADDR_MODE_REG_IYL, */
    -1,/* ASM_ADDR_MODE_REG_AF, */
    -1,/* ASM_ADDR_MODE_REG_BC, */
    -1,/* ASM_ADDR_MODE_REG_DE, */
    -1,/* ASM_ADDR_MODE_REG_HL, */
    -1,/* ASM_ADDR_MODE_REG_IX, */
    -1,/* ASM_ADDR_MODE_REG_IY, */
    -1,/* ASM_ADDR_MODE_REG_SP, */
    -1,/* ASM_ADDR_MODE_REG_IND_BC, */
    -1,/* ASM_ADDR_MODE_REG_IND_DE, */
    -1,/* ASM_ADDR_MODE_REG_IND_HL, */
    -1,/* ASM_ADDR_MODE_REG_IND_IX, */
    -1,/* ASM_ADDR_MODE_REG_IND_IY, */
    -1,/* ASM_ADDR_MODE_REG_IND_SP */
       /* more modes needed for 6809 */
    1, /* ASM_ADDR_MODE_ILLEGAL, */
    2, /* ASM_ADDR_MODE_IMM_BYTE, */
    3, /* ASM_ADDR_MODE_IMM_WORD, */
    2, /* ASM_ADDR_MODE_DIRECT, */
    3, /* ASM_ADDR_MODE_EXTENDED, */
    2, /* ASM_ADDR_MODE_INDEXED,        post-byte determines sub-mode */
    2, /* ASM_ADDR_MODE_REL_BYTE, */
    3, /* ASM_ADDR_MODE_REL_WORD, */
    2, /* ASM_ADDR_MODE_REG_POST, */
    2, /* ASM_ADDR_MODE_SYS_POST, */
    2, /* ASM_ADDR_MODE_USR_POST, */
};

static const int indexed_size[0x20] = {
    0, /* ASM_ADDR_MODE_INDEXED_INC1 */
    0, /* ASM_ADDR_MODE_INDEXED_INC2 */
    0, /* ASM_ADDR_MODE_INDEXED_DEC1 */
    0, /* ASM_ADDR_MODE_INDEXED_DEC2 */
    0, /* ASM_ADDR_MODE_INDEXED_OFF0 */
    0, /* ASM_ADDR_MODE_INDEXED_OFFB */
    0, /* ASM_ADDR_MODE_INDEXED_OFFA */
    0, /* ASM_ADDR_MODE_INDEXED_07 */
    1, /* ASM_ADDR_MODE_INDEXED_OFF8 */
    2, /* ASM_ADDR_MODE_INDEXED_OFF16 */
    0, /* ASM_ADDR_MODE_INDEXED_0A */
    0, /* ASM_ADDR_MODE_INDEXED_OFFD */
    0, /* ASM_ADDR_MODE_INDEXED_OFFPC8 */
    0, /* ASM_ADDR_MODE_INDEXED_OFFPC16 */
    0, /* ASM_ADDR_MODE_INDEXED_0E */
    0, /* ASM_ADDR_MODE_INDEXED_0F */
    0, /* ASM_ADDR_MODE_INDEXED_10 */
    0, /* ASM_ADDR_MODE_INDEXED_INC2_IND */
    0, /* ASM_ADDR_MODE_INDEXED_12 */
    0, /* ASM_ADDR_MODE_INDEXED_DEC2_IND */
    0, /* ASM_ADDR_MODE_INDEXED_OFF0_IND */
    0, /* ASM_ADDR_MODE_INDEXED_OFFB_IND */
    0, /* ASM_ADDR_MODE_INDEXED_OFFA_IND */
    0, /* ASM_ADDR_MODE_INDEXED_17 */
    1, /* ASM_ADDR_MODE_INDEXED_OFF8_IND */
    2, /* ASM_ADDR_MODE_INDEXED_OFF16_IND  */
    0, /* ASM_ADDR_MODE_INDEXED_1A */
    0, /* ASM_ADDR_MODE_INDEXED_OFFD_IND */
    1, /* ASM_ADDR_MODE_INDEXED_OFFPC8_IND */
    2, /* ASM_ADDR_MODE_INDEXED_OFFPC16_IND */
    0, /* ASM_ADDR_MODE_INDEXED_1E */
    2, /* ASM_ADDR_MODE_INDEXED_16_IND */
};

static const asm_opcode_info_t opcode_list[256] = {
    /* 00 */ { "NEG", ASM_ADDR_MODE_DIRECT },
    /* 01 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEG direct (6809), OIM direct (6309) */
    /* 02 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* carry ? COM direct : NEG direct (6809), AIM direct (6309) */
    /* 03 */ { "COM", ASM_ADDR_MODE_DIRECT },
    /* 04 */ { "LSR", ASM_ADDR_MODE_DIRECT },
    /* 05 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* LSR direct (6809), EIM direct (6309) */
    /* 06 */ { "ROR", ASM_ADDR_MODE_DIRECT },
    /* 07 */ { "ASR", ASM_ADDR_MODE_DIRECT },
    /* 08 */ { "ASL", ASM_ADDR_MODE_DIRECT },
    /* 09 */ { "ROL", ASM_ADDR_MODE_DIRECT },
    /* 0a */ { "DEC", ASM_ADDR_MODE_DIRECT },
    /* 0b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* DEC direct (6809), TIM direct (6309) */
    /* 0c */ { "INC", ASM_ADDR_MODE_DIRECT },
    /* 0d */ { "TST", ASM_ADDR_MODE_DIRECT },
    /* 0e */ { "JMP", ASM_ADDR_MODE_DIRECT },
    /* 0f */ { "CLR", ASM_ADDR_MODE_DIRECT },
    /* 10 */ { "PREFIX", ASM_ADDR_MODE_ILLEGAL },
    /* 11 */ { "PREFIX", ASM_ADDR_MODE_ILLEGAL },
    /* 12 */ { "NOP", ASM_ADDR_MODE_IMPLIED },
    /* 13 */ { "SYNC", ASM_ADDR_MODE_IMPLIED },
    /* 14 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* HCF (6809), SEXW (6309) */
    /* 15 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* HCF (6809), illegal trap (6309) */
    /* 16 */ { "LBRA", ASM_ADDR_MODE_REL_WORD },
    /* 17 */ { "LBSR", ASM_ADDR_MODE_REL_WORD },
    /* 18 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* CCRS (6809), illegal trap (6309) */
    /* 19 */ { "DAA", ASM_ADDR_MODE_IMPLIED },
    /* 1a */ { "ORCC", ASM_ADDR_MODE_IMM_BYTE },
    /* 1b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NOP (6809), illegal trap (6309) */
    /* 1c */ { "ANDCC", ASM_ADDR_MODE_IMM_BYTE },
    /* 1d */ { "SEX", ASM_ADDR_MODE_IMPLIED },
    /* 1e */ { "EXG", ASM_ADDR_MODE_REG_POST },
    /* 1f */ { "TFR", ASM_ADDR_MODE_REG_POST },
    /* 20 */ { "BRA", ASM_ADDR_MODE_REL_BYTE },
    /* 21 */ { "BRN", ASM_ADDR_MODE_REL_BYTE },
    /* 22 */ { "BHI", ASM_ADDR_MODE_REL_BYTE },
    /* 23 */ { "BLS", ASM_ADDR_MODE_REL_BYTE },
    /* 24 */ { "BCC", ASM_ADDR_MODE_REL_BYTE },
    /* 25 */ { "BCS", ASM_ADDR_MODE_REL_BYTE },
    /* 26 */ { "BNE", ASM_ADDR_MODE_REL_BYTE },
    /* 27 */ { "BEQ", ASM_ADDR_MODE_REL_BYTE },
    /* 28 */ { "BVC", ASM_ADDR_MODE_REL_BYTE },
    /* 29 */ { "BVS", ASM_ADDR_MODE_REL_BYTE },
    /* 2a */ { "BPL", ASM_ADDR_MODE_REL_BYTE },
    /* 2b */ { "BMI", ASM_ADDR_MODE_REL_BYTE },
    /* 2c */ { "BGE", ASM_ADDR_MODE_REL_BYTE },
    /* 2d */ { "BLT", ASM_ADDR_MODE_REL_BYTE },
    /* 2e */ { "BGT", ASM_ADDR_MODE_REL_BYTE },
    /* 2f */ { "BLE", ASM_ADDR_MODE_REL_BYTE },
    /* 30 */ { "LEAX", ASM_ADDR_MODE_INDEXED },
    /* 31 */ { "LEAY", ASM_ADDR_MODE_INDEXED },
    /* 32 */ { "LEAS", ASM_ADDR_MODE_INDEXED },
    /* 33 */ { "LEAU", ASM_ADDR_MODE_INDEXED },
    /* 34 */ { "PSHS", ASM_ADDR_MODE_SYS_POST },
    /* 35 */ { "PULS", ASM_ADDR_MODE_SYS_POST },
    /* 36 */ { "PSHU", ASM_ADDR_MODE_USR_POST },
    /* 37 */ { "PULU", ASM_ADDR_MODE_USR_POST },
    /* 38 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* ANDCC immediate +1 cycle (6809), illegal trap (6309) */
    /* 39 */ { "RTS", ASM_ADDR_MODE_IMPLIED },
    /* 3a */ { "ABX", ASM_ADDR_MODE_IMPLIED },
    /* 3b */ { "RTI", ASM_ADDR_MODE_IMPLIED },
    /* 3c */ { "CWAI", ASM_ADDR_MODE_IMM_BYTE },
    /* 3d */ { "MUL", ASM_ADDR_MODE_IMPLIED },
    /* 3e */ { "RESET", ASM_ADDR_MODE_IMPLIED },
    /* 3f */ { "SWI", ASM_ADDR_MODE_IMPLIED },
    /* 40 */ { "NEGA", ASM_ADDR_MODE_IMPLIED },
    /* 41 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEGA (6809), illegal trap (6309) */
    /* 42 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* carry ? COMA : NEGA (6809), illegal trap (6309) */
    /* 43 */ { "COMA", ASM_ADDR_MODE_IMPLIED },
    /* 44 */ { "LSRA", ASM_ADDR_MODE_IMPLIED },
    /* 45 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* LSRA (6809), illegal trap (6309) */
    /* 46 */ { "RORA", ASM_ADDR_MODE_IMPLIED },
    /* 47 */ { "ASRA", ASM_ADDR_MODE_IMPLIED },
    /* 48 */ { "ASLA", ASM_ADDR_MODE_IMPLIED },
    /* 49 */ { "ROLA", ASM_ADDR_MODE_IMPLIED },
    /* 4a */ { "DECA", ASM_ADDR_MODE_IMPLIED },
    /* 4b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* DECA (6809), illegal trap (6309) */
    /* 4c */ { "INCA", ASM_ADDR_MODE_IMPLIED },
    /* 4d */ { "TSTA", ASM_ADDR_MODE_IMPLIED },
    /* 4e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* CLRA (6809), illegal trap (6309) */
    /* 4f */ { "CLRA", ASM_ADDR_MODE_IMPLIED },
    /* 50 */ { "NEGB", ASM_ADDR_MODE_IMPLIED },
    /* 51 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEGB (6809), illegal trap (6309) */
    /* 52 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* carry ? COMB : NEGB (6809), illegal trap (6309) */
    /* 53 */ { "COMB", ASM_ADDR_MODE_IMPLIED },
    /* 54 */ { "LSRB", ASM_ADDR_MODE_IMPLIED },
    /* 55 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* LSRB (6809), illegal trap (6309) */
    /* 56 */ { "RORB", ASM_ADDR_MODE_IMPLIED },
    /* 57 */ { "ASRB", ASM_ADDR_MODE_IMPLIED },
    /* 58 */ { "ASLB", ASM_ADDR_MODE_IMPLIED },
    /* 59 */ { "ROLB", ASM_ADDR_MODE_IMPLIED },
    /* 5a */ { "DECB", ASM_ADDR_MODE_IMPLIED },
    /* 5b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* DECB (6809), illegal trap (6309) */
    /* 5c */ { "INCB", ASM_ADDR_MODE_IMPLIED },
    /* 5d */ { "TSTB", ASM_ADDR_MODE_IMPLIED },
    /* 5e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* CLRB (6809), illegal trap (6309) */
    /* 5f */ { "CLRB", ASM_ADDR_MODE_IMPLIED },
    /* 60 */ { "NEG", ASM_ADDR_MODE_INDEXED },
    /* 61 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEG indexed (6809), OIM indexed (6309) */
    /* 62 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* carry ? COM indexed : NEG indexed (6809), AIM indexed (6309) */
    /* 63 */ { "COM", ASM_ADDR_MODE_INDEXED },
    /* 64 */ { "LSR", ASM_ADDR_MODE_INDEXED },
    /* 65 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* LSR indexed (6809), EIM indexed (6309) */
    /* 66 */ { "ROR", ASM_ADDR_MODE_INDEXED },
    /* 67 */ { "ASR", ASM_ADDR_MODE_INDEXED },
    /* 68 */ { "ASL", ASM_ADDR_MODE_INDEXED },
    /* 69 */ { "ROL", ASM_ADDR_MODE_INDEXED },
    /* 6a */ { "DEC", ASM_ADDR_MODE_INDEXED },
    /* 6b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* DEC indexed (6809), TIM indexed (6309) */
    /* 6c */ { "INC", ASM_ADDR_MODE_INDEXED },
    /* 6d */ { "TST", ASM_ADDR_MODE_INDEXED },
    /* 6e */ { "JMP", ASM_ADDR_MODE_INDEXED },
    /* 6f */ { "CLR", ASM_ADDR_MODE_INDEXED },
    /* 70 */ { "NEG", ASM_ADDR_MODE_EXTENDED },
    /* 71 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEG extended (6809), OIM extended (6309) */
    /* 72 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* carry ? COM extended : NEG extended (6809), AIM extended (6309) */
    /* 73 */ { "COM", ASM_ADDR_MODE_EXTENDED },
    /* 74 */ { "LSR", ASM_ADDR_MODE_EXTENDED },
    /* 75 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* LSR extended (6809), EIM extended (6309) */
    /* 76 */ { "ROR", ASM_ADDR_MODE_EXTENDED },
    /* 77 */ { "ASR", ASM_ADDR_MODE_EXTENDED },
    /* 78 */ { "ASL", ASM_ADDR_MODE_EXTENDED },
    /* 79 */ { "ROL", ASM_ADDR_MODE_EXTENDED },
    /* 7a */ { "DEC", ASM_ADDR_MODE_EXTENDED },
    /* 7b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* DEC extended (6809), TIM extended (6309) */
    /* 7c */ { "INC", ASM_ADDR_MODE_EXTENDED },
    /* 7d */ { "TST", ASM_ADDR_MODE_EXTENDED },
    /* 7e */ { "JMP", ASM_ADDR_MODE_EXTENDED },
    /* 7f */ { "CLR", ASM_ADDR_MODE_EXTENDED },
    /* 80 */ { "SUBA", ASM_ADDR_MODE_IMM_BYTE },
    /* 81 */ { "CMPA", ASM_ADDR_MODE_IMM_BYTE },
    /* 82 */ { "SBCA", ASM_ADDR_MODE_IMM_BYTE },
    /* 83 */ { "SUBD", ASM_ADDR_MODE_IMM_WORD },
    /* 84 */ { "ANDA", ASM_ADDR_MODE_IMM_BYTE },
    /* 85 */ { "BITA", ASM_ADDR_MODE_IMM_BYTE },
    /* 86 */ { "LDA", ASM_ADDR_MODE_IMM_BYTE },
    /* 87 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* SCC immediate (6809), illegal trap (6309) */
    /* 88 */ { "EORA", ASM_ADDR_MODE_IMM_BYTE },
    /* 89 */ { "ADCA", ASM_ADDR_MODE_IMM_BYTE },
    /* 8a */ { "ORA", ASM_ADDR_MODE_IMM_BYTE },
    /* 8b */ { "ADDA", ASM_ADDR_MODE_IMM_BYTE },
    /* 8c */ { "CMPX", ASM_ADDR_MODE_IMM_WORD },
    /* 8d */ { "BSR", ASM_ADDR_MODE_REL_BYTE },
    /* 8e */ { "LDX", ASM_ADDR_MODE_IMM_WORD },
    /* 8f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* STX immediate (6809), illegal trap (6309) */
    /* 90 */ { "SUBA", ASM_ADDR_MODE_DIRECT },
    /* 91 */ { "CMPA", ASM_ADDR_MODE_DIRECT },
    /* 92 */ { "SBCA", ASM_ADDR_MODE_DIRECT },
    /* 93 */ { "SUBD", ASM_ADDR_MODE_DIRECT },
    /* 94 */ { "ANDA", ASM_ADDR_MODE_DIRECT },
    /* 95 */ { "BITA", ASM_ADDR_MODE_DIRECT },
    /* 96 */ { "LDA", ASM_ADDR_MODE_DIRECT },
    /* 97 */ { "STA", ASM_ADDR_MODE_DIRECT },
    /* 98 */ { "EORA", ASM_ADDR_MODE_DIRECT },
    /* 99 */ { "ADCA", ASM_ADDR_MODE_DIRECT },
    /* 9a */ { "ORA", ASM_ADDR_MODE_DIRECT },
    /* 9b */ { "ADDA", ASM_ADDR_MODE_DIRECT },
    /* 9c */ { "CMPX", ASM_ADDR_MODE_DIRECT },
    /* 9d */ { "JSR", ASM_ADDR_MODE_DIRECT },
    /* 9e */ { "LDX", ASM_ADDR_MODE_DIRECT },
    /* 9f */ { "STX", ASM_ADDR_MODE_DIRECT },
    /* a0 */ { "SUBA", ASM_ADDR_MODE_INDEXED },
    /* a1 */ { "CMPA", ASM_ADDR_MODE_INDEXED },
    /* a2 */ { "SBCA", ASM_ADDR_MODE_INDEXED },
    /* a3 */ { "SUBD", ASM_ADDR_MODE_INDEXED },
    /* a4 */ { "ANDA", ASM_ADDR_MODE_INDEXED },
    /* a5 */ { "BITA", ASM_ADDR_MODE_INDEXED },
    /* a6 */ { "LDA", ASM_ADDR_MODE_INDEXED },
    /* a7 */ { "STA", ASM_ADDR_MODE_INDEXED },
    /* a8 */ { "EORA", ASM_ADDR_MODE_INDEXED },
    /* a9 */ { "ADCA", ASM_ADDR_MODE_INDEXED },
    /* aa */ { "ORA", ASM_ADDR_MODE_INDEXED },
    /* ab */ { "ADDA", ASM_ADDR_MODE_INDEXED },
    /* ac */ { "CMPX", ASM_ADDR_MODE_INDEXED },
    /* ad */ { "JSR", ASM_ADDR_MODE_INDEXED },
    /* ae */ { "LDX", ASM_ADDR_MODE_INDEXED },
    /* af */ { "STX", ASM_ADDR_MODE_INDEXED },
    /* b0 */ { "SUBA", ASM_ADDR_MODE_EXTENDED },
    /* b1 */ { "CMPA", ASM_ADDR_MODE_EXTENDED },
    /* b2 */ { "SBCA", ASM_ADDR_MODE_EXTENDED },
    /* b3 */ { "SUBD", ASM_ADDR_MODE_EXTENDED },
    /* b4 */ { "ANDA", ASM_ADDR_MODE_EXTENDED },
    /* b5 */ { "BITA", ASM_ADDR_MODE_EXTENDED },
    /* b6 */ { "LDA", ASM_ADDR_MODE_EXTENDED },
    /* b7 */ { "STA", ASM_ADDR_MODE_EXTENDED },
    /* b8 */ { "EORA", ASM_ADDR_MODE_EXTENDED },
    /* b9 */ { "ADCA", ASM_ADDR_MODE_EXTENDED },
    /* ba */ { "ORA", ASM_ADDR_MODE_EXTENDED },
    /* bb */ { "ADDA", ASM_ADDR_MODE_EXTENDED },
    /* bc */ { "CMPX", ASM_ADDR_MODE_EXTENDED },
    /* bd */ { "JSR", ASM_ADDR_MODE_EXTENDED },
    /* be */ { "LDX", ASM_ADDR_MODE_EXTENDED },
    /* bf */ { "STX", ASM_ADDR_MODE_EXTENDED },
    /* c0 */ { "SUBB", ASM_ADDR_MODE_IMM_BYTE },
    /* c1 */ { "CMPB", ASM_ADDR_MODE_IMM_BYTE },
    /* c2 */ { "SBCB", ASM_ADDR_MODE_IMM_BYTE },
    /* c3 */ { "ADDD", ASM_ADDR_MODE_IMM_WORD },
    /* c4 */ { "ANDB", ASM_ADDR_MODE_IMM_BYTE },
    /* c5 */ { "BITB", ASM_ADDR_MODE_IMM_BYTE },
    /* c6 */ { "LDB", ASM_ADDR_MODE_IMM_BYTE },
    /* c7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* SCC immediate (6809), illegal trap (6309) */
    /* c8 */ { "EORB", ASM_ADDR_MODE_IMM_BYTE },
    /* c9 */ { "ADCB", ASM_ADDR_MODE_IMM_BYTE },
    /* ca */ { "ORB", ASM_ADDR_MODE_IMM_BYTE },
    /* cb */ { "ADDB", ASM_ADDR_MODE_IMM_BYTE },
    /* cc */ { "LDD", ASM_ADDR_MODE_IMM_WORD },
    /* cd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* HCF (6809), LDQ immediate (6309) */
    /* ce */ { "LDU", ASM_ADDR_MODE_IMM_WORD },
    /* cf */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* STU immediate (6809), illegal trap (6309) */
    /* d0 */ { "SUBB", ASM_ADDR_MODE_DIRECT },
    /* d1 */ { "CMPB", ASM_ADDR_MODE_DIRECT },
    /* d2 */ { "SBCB", ASM_ADDR_MODE_DIRECT },
    /* d3 */ { "ADDD", ASM_ADDR_MODE_DIRECT },
    /* d4 */ { "ANDB", ASM_ADDR_MODE_DIRECT },
    /* d5 */ { "BITB", ASM_ADDR_MODE_DIRECT },
    /* d6 */ { "LDB", ASM_ADDR_MODE_DIRECT },
    /* d7 */ { "STB", ASM_ADDR_MODE_DIRECT },
    /* d8 */ { "EORB", ASM_ADDR_MODE_DIRECT },
    /* d9 */ { "ADCB", ASM_ADDR_MODE_DIRECT },
    /* da */ { "ORB", ASM_ADDR_MODE_DIRECT },
    /* db */ { "ADDB", ASM_ADDR_MODE_DIRECT },
    /* dc */ { "LDD", ASM_ADDR_MODE_DIRECT },
    /* dd */ { "STD", ASM_ADDR_MODE_DIRECT },
    /* de */ { "LDU", ASM_ADDR_MODE_DIRECT },
    /* df */ { "STU", ASM_ADDR_MODE_DIRECT },
    /* e0 */ { "SUBB", ASM_ADDR_MODE_INDEXED },
    /* e1 */ { "CMPB", ASM_ADDR_MODE_INDEXED },
    /* e2 */ { "SBCB", ASM_ADDR_MODE_INDEXED },
    /* e3 */ { "ADDD", ASM_ADDR_MODE_INDEXED },
    /* e4 */ { "ANDB", ASM_ADDR_MODE_INDEXED },
    /* e5 */ { "BITB", ASM_ADDR_MODE_INDEXED },
    /* e6 */ { "LDB", ASM_ADDR_MODE_INDEXED },
    /* e7 */ { "STB", ASM_ADDR_MODE_INDEXED },
    /* e8 */ { "EORB", ASM_ADDR_MODE_INDEXED },
    /* e9 */ { "ADCB", ASM_ADDR_MODE_INDEXED },
    /* ea */ { "ORB", ASM_ADDR_MODE_INDEXED },
    /* eb */ { "ADDB", ASM_ADDR_MODE_INDEXED },
    /* ec */ { "LDD", ASM_ADDR_MODE_INDEXED },
    /* ed */ { "STD", ASM_ADDR_MODE_INDEXED },
    /* ee */ { "LDU", ASM_ADDR_MODE_INDEXED },
    /* ef */ { "STU", ASM_ADDR_MODE_INDEXED },
    /* f0 */ { "SUBB", ASM_ADDR_MODE_EXTENDED },
    /* f1 */ { "CMPB", ASM_ADDR_MODE_EXTENDED },
    /* f2 */ { "SBCB", ASM_ADDR_MODE_EXTENDED },
    /* f3 */ { "ADDD", ASM_ADDR_MODE_EXTENDED },
    /* f4 */ { "ANDB", ASM_ADDR_MODE_EXTENDED },
    /* f5 */ { "BITB", ASM_ADDR_MODE_EXTENDED },
    /* f6 */ { "LDB", ASM_ADDR_MODE_EXTENDED },
    /* f7 */ { "STB", ASM_ADDR_MODE_EXTENDED },
    /* f8 */ { "EORB", ASM_ADDR_MODE_EXTENDED },
    /* f9 */ { "ADCB", ASM_ADDR_MODE_EXTENDED },
    /* fa */ { "ORB", ASM_ADDR_MODE_EXTENDED },
    /* fb */ { "ADDB", ASM_ADDR_MODE_EXTENDED },
    /* fc */ { "LDD", ASM_ADDR_MODE_EXTENDED },
    /* fd */ { "STD", ASM_ADDR_MODE_EXTENDED },
    /* fe */ { "LDU", ASM_ADDR_MODE_EXTENDED },
    /* ff */ { "STU", ASM_ADDR_MODE_EXTENDED }
};

static const asm_opcode_info_t opcode_list_10[256] = {
    /* 00 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEG direct (6809), illegal trap (6309) */
    /* 01 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* NEG direct (6809), illegal trap (6309) */
    /* 02 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* carry ? COM direct : NEG direct, illegal trap (6309) */
    /* 03 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	/* COM direct (6809), illegal trap (6309) */
    /* 04 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },	
    /* 05 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 06 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 07 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 08 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 09 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 10 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 11 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 12 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 13 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 14 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 15 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 16 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 17 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 18 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 19 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 20 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 21 */ { "LBRN", ASM_ADDR_MODE_REL_WORD },
    /* 22 */ { "LBHI", ASM_ADDR_MODE_REL_WORD },
    /* 23 */ { "LBLS", ASM_ADDR_MODE_REL_WORD },
    /* 24 */ { "LBCC", ASM_ADDR_MODE_REL_WORD },
    /* 25 */ { "LBCS", ASM_ADDR_MODE_REL_WORD },
    /* 26 */ { "LBNE", ASM_ADDR_MODE_REL_WORD },
    /* 27 */ { "LBEQ", ASM_ADDR_MODE_REL_WORD },
    /* 28 */ { "LBVC", ASM_ADDR_MODE_REL_WORD },
    /* 29 */ { "LBVS", ASM_ADDR_MODE_REL_WORD },
    /* 2a */ { "LBPL", ASM_ADDR_MODE_REL_WORD },
    /* 2b */ { "LBMI", ASM_ADDR_MODE_REL_WORD },
    /* 2c */ { "LBGE", ASM_ADDR_MODE_REL_WORD },
    /* 2d */ { "LBLT", ASM_ADDR_MODE_REL_WORD },
    /* 2e */ { "LBGT", ASM_ADDR_MODE_REL_WORD },
    /* 2f */ { "LBLE", ASM_ADDR_MODE_REL_WORD },
    /* 30 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 31 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 32 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 33 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 34 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 35 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 36 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 37 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 38 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 39 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3f */ { "SWI2", ASM_ADDR_MODE_IMPLIED },
    /* 40 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL }, /* 10 40 */
    /* 41 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 42 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 43 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 44 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 45 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 46 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 47 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 48 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 49 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 50 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 51 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 52 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 53 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 54 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 55 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 56 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 57 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 58 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 59 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 60 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 61 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 62 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 63 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 64 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 65 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 66 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 67 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 68 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 69 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 70 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 71 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 72 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 73 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 74 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 75 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 76 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 77 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 78 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 79 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 80 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 81 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 82 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 83 */ { "CMPD", ASM_ADDR_MODE_IMM_WORD },
    /* 84 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 85 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 86 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 87 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 88 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 89 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8c */ { "CMPY", ASM_ADDR_MODE_IMM_WORD },
    /* 8d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8e */ { "LDY", ASM_ADDR_MODE_IMM_WORD },
    /* 8f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 90 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 91 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 92 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 93 */ { "CMPD", ASM_ADDR_MODE_DIRECT },
    /* 94 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 95 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 96 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 97 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 98 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 99 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9c */ { "CMPY", ASM_ADDR_MODE_DIRECT },
    /* 9d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9e */ { "LDY", ASM_ADDR_MODE_DIRECT },
    /* 9f */ { "STY", ASM_ADDR_MODE_DIRECT },
    /* a0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a3 */ { "CMPD", ASM_ADDR_MODE_INDEXED },
    /* a4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* aa */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ab */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ac */ { "CMPY", ASM_ADDR_MODE_INDEXED },
    /* ad */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ae */ { "LDY", ASM_ADDR_MODE_INDEXED },
    /* af */ { "STY", ASM_ADDR_MODE_INDEXED },
    /* b0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b3 */ { "CMPD", ASM_ADDR_MODE_EXTENDED },
    /* b4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ba */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* bb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* bc */ { "CMPY", ASM_ADDR_MODE_EXTENDED },
    /* bd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* be */ { "LDY", ASM_ADDR_MODE_EXTENDED },
    /* bf */ { "STY", ASM_ADDR_MODE_EXTENDED },
    /* c0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ca */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cc */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ce */ { "LDS", ASM_ADDR_MODE_IMM_WORD },
    /* cf */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* da */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* db */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* dc */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* dd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* de */ { "LDS", ASM_ADDR_MODE_DIRECT },
    /* df */ { "STS", ASM_ADDR_MODE_DIRECT },
    /* e0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ea */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* eb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ec */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ed */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ee */ { "LDS", ASM_ADDR_MODE_INDEXED },
    /* ef */ { "STS", ASM_ADDR_MODE_INDEXED },
    /* f0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fa */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fc */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fe */ { "LDS", ASM_ADDR_MODE_EXTENDED },
    /* ff */ { "STS", ASM_ADDR_MODE_EXTENDED }
};

static const asm_opcode_info_t opcode_list_11[256] = {
    /* 00 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 01 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 02 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 03 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 04 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 05 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 06 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 07 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 08 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 09 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 0f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 10 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 11 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 12 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 13 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 14 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 15 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 16 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 17 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 18 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 19 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 1f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 20 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 21 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 22 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 23 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 24 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 25 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 26 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 27 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 28 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 29 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 2a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 2b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 2c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 2d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 2e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 2f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 30 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 31 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 32 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 33 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 34 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 35 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 36 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 37 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 38 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 39 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 3f */ { "SWI3", ASM_ADDR_MODE_IMPLIED },
    /* 40 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL }, /* 11 40 */
    /* 41 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 42 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 43 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 44 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 45 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 46 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 47 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 48 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 49 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 4f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 50 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 51 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 52 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 53 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 54 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 55 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 56 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 57 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 58 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 59 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 5f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 60 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 61 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 62 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 63 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 64 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 65 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 66 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 67 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 68 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 69 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 6f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 70 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 71 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 72 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 73 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 74 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 75 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 76 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 77 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 78 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 79 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7c */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 7f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 80 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 81 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 82 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 83 */ { "CMPU", ASM_ADDR_MODE_IMM_WORD },
    /* 84 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 85 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 86 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 87 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 88 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 89 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8c */ { "CMPS", ASM_ADDR_MODE_IMM_WORD },
    /* 8d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 8f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 90 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 91 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 92 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 93 */ { "CMPU", ASM_ADDR_MODE_DIRECT },
    /* 94 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 95 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 96 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 97 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 98 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 99 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9a */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9b */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9c */ { "CMPS", ASM_ADDR_MODE_DIRECT },
    /* 9d */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9e */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* 9f */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a3 */ { "CMPU", ASM_ADDR_MODE_INDEXED },
    /* a4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* a9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* aa */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ab */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ac */ { "CMPS", ASM_ADDR_MODE_INDEXED },
    /* ad */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ae */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* af */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b3 */ { "CMPU", ASM_ADDR_MODE_EXTENDED },
    /* b4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* b9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ba */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* bb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* bc */ { "CMPS", ASM_ADDR_MODE_EXTENDED },
    /* bd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* be */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* bf */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* c9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ca */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cc */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ce */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* cf */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* d9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* da */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* db */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* dc */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* dd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* de */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* df */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* e9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ea */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* eb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ec */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ed */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ee */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ef */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f0 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f1 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f2 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f3 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f4 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f5 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f6 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f7 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f8 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* f9 */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fa */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fb */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fc */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fd */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* fe */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL },
    /* ff */ { "UNDOC", ASM_ADDR_MODE_ILLEGAL }
};

static const asm_opcode_info_t *asm_opcode_info_get(unsigned int p0, unsigned int p1, unsigned int p2)
{
    /*
     * Extra prefix bytes after the first one should be ignored (UNDOC),
     * but since we get only a limited amount of bytes here we can't
     * really do that.
     */
    if (p0 == 0x10) {
        return opcode_list_10 + p1;
    }
    if (p0 == 0x11) {
        return opcode_list_11 + p1;
    }
    return opcode_list + p0;
}

static unsigned int asm_addr_mode_get_size(unsigned int mode, unsigned int p0,
                                           unsigned int p1, unsigned int p2)
{
    int size = 0;

    if (p0 == 0x10 || p0 == 0x11) {
        size++;
        p0 = p1;
        p1 = p2;
    }
    if (mode == ASM_ADDR_MODE_INDEXED) {
        /* post-byte determines submode */
        if (p1 & 0x80) {
            size += indexed_size[p1 & 0x1F];
        }
    }
    return size + addr_mode_size[mode];
}

void asm6809_init(monitor_cpu_type_t *monitor_cpu_type)
{
    monitor_cpu_type->cpu_type = CPU_6809;
    monitor_cpu_type->asm_addr_mode_get_size = asm_addr_mode_get_size;
    monitor_cpu_type->asm_opcode_info_get = asm_opcode_info_get;

    /* Once we have a generic processor specific init, this will move.  */
    mon_assemble6809_init(monitor_cpu_type);
    mon_register6809_init(monitor_cpu_type);
}

