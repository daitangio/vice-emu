/*
 * mon.h - The VICE built-in monitor, external interface.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Daniel Sladic <sladic@eecg.toronto.edu>
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

#ifndef _MON_H
#define _MON_H

#include "types.h"

/** Generic interface.  **/
#define NUM_MEMSPACES 4

#define any_watchpoints(mem) \
    (watchpoints_load[(mem)] || watchpoints_store[(mem)])

enum mon_int {
    MI_NONE = 0,
    MI_BREAK = 1 << 0,
    MI_WATCH = 1 << 1,
    MI_STEP = 1 << 2
};

enum t_memspace {
   e_default_space = 0,
   e_comp_space,
   e_disk8_space,
   e_disk9_space,
   e_invalid_space
};
typedef enum t_memspace MEMSPACE;

enum CPU_TYPE_s {
    CPU_6502,
    CPU_Z80
};
typedef enum CPU_TYPE_s CPU_TYPE_t;

struct monitor_cpu_type_s {
    CPU_TYPE_t cpu_type;
    unsigned int (*asm_addr_mode_get_size)(unsigned int mode, BYTE p0, BYTE p1);
    struct asm_opcode_info_s *(*asm_opcode_info_get)(BYTE p0, BYTE p1, BYTE p2);
    int (*mon_assemble_instr)(const char *opcode_name, unsigned int operand);
    unsigned int (*mon_register_get_val)(int mem, int reg_id);
    void (*mon_register_set_val)(int mem, int reg_id, WORD val);
    void (*mon_register_print)(int mem);
    struct mon_reg_list_s *(*mon_register_list_get)(int mem);
    void (*mon_register_list_set)(struct mon_reg_list_s *mon_reg_list, int mem);
};
typedef struct monitor_cpu_type_s monitor_cpu_type_t;

/* This is the standard interface through which the monitor accesses a
   certain CPU.  */
struct monitor_interface_s {
    /* Pointer to the registers of the 6502 CPU.  */
    struct mos6510_regs_s *cpu_regs;

    /* Pointer to the registers of the Z80 CPU.  */
    struct z80_regs_s *z80_cpu_regs;

    /* Pointer to the alarm/interrupt status.  */
    struct cpu_int_status_s *int_status;

    /* Pointer to the machine's clock counter.  */
    CLOCK *clk;

    int current_bank;
    const char **(*mem_bank_list)(void);
    int (*mem_bank_from_name)(const char *name);
    BYTE (*mem_bank_read)(int bank, ADDRESS addr);
    BYTE (*mem_bank_peek)(int bank, ADDRESS addr);
    void (*mem_bank_write)(int bank, ADDRESS addr, BYTE byte);

    struct mem_ioreg_list_s *(*mem_ioreg_list_get)(void);

    /* Pointer to a function to disable/enable watchpoint checking.  */
    /*monitor_toggle_func_t *toggle_watchpoints_func;*/
    void (*toggle_watchpoints_func)(int value);

    /* Update bank base (used for drives).  */
    void (*set_bank_base)(void);
};
typedef struct monitor_interface_s monitor_interface_t;


/* Externals */
struct break_list_s;
extern struct break_list_s *watchpoints_load[NUM_MEMSPACES];
extern struct break_list_s *watchpoints_store[NUM_MEMSPACES];
extern MEMSPACE caller_space;
extern unsigned mon_mask[NUM_MEMSPACES];



/* Prototypes */
extern void monitor_init(monitor_interface_t *maincpu_interface,
                         monitor_interface_t *drive8_interface_init,
                         monitor_interface_t *drive9_interface_init,
                         struct monitor_cpu_type_s **asmarray);
extern void mon(ADDRESS a);

extern int mon_force_import(MEMSPACE mem);
extern void mon_check_icount(ADDRESS a);
extern void mon_check_icount_interrupt(void);
extern void mon_check_watchpoints(ADDRESS a);

extern void mon_watch_push_load_addr(ADDRESS addr, MEMSPACE mem);
extern void mon_watch_push_store_addr(ADDRESS addr, MEMSPACE mem);


/** Breakpoint interface.  */
/* Defines */
#define check_breakpoints(mem, addr) \
    mon_breakpoint_check_checkpoint(mem, addr, breakpoints[mem])

/* Externals */
extern struct break_list_s *breakpoints[NUM_MEMSPACES];

/* Prototypes */
extern int mon_breakpoint_check_checkpoint(MEMSPACE mem, ADDRESS addr,
                                           struct break_list_s *list);

/** Disassemble interace */
/* Prototypes */
extern const char *mon_disassemble_to_string(MEMSPACE, ADDRESS addr, BYTE x,
                                             BYTE p1, BYTE p2, BYTE p3,
                                             int hex_mode);

/** Register interface.  */
extern struct mon_reg_list_s *mon_register_list_get(int mem);

#endif

