/*
 * plus4mem.h -- Plus4 memory handling.
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

#ifndef VICE_PLUS4MEM_H
#define VICE_PLUS4MEM_H

#include "types.h"

struct pport_s {
    /* Value written to processor port.  */
    uint8_t dir;
    uint8_t data;

    /* State of processor port pins.  */
    uint8_t data_out;
};
typedef struct pport_s pport_t;

extern pport_t pport;

#define PLUS4_RAM_SIZE        0x10000
#define PLUS4_BASIC_ROM_SIZE  0x4000
#define PLUS4_KERNAL_ROM_SIZE 0x4000

#define PLUS4_C0LO_ROM_SIZE  0x4000
#define PLUS4_C0HI_ROM_SIZE  0x4000
#define PLUS4_C2LO_ROM_SIZE  0x4000
#define PLUS4_C2HI_ROM_SIZE  0x4000

#define PLUS4_CART8K_SIZE     0x2000
#define PLUS4_CART16K_SIZE    0x4000

extern unsigned int mem_config;

extern uint8_t extromlo1[PLUS4_C0LO_ROM_SIZE];
extern uint8_t extromhi1[PLUS4_C0HI_ROM_SIZE];

extern uint8_t extromlo3[PLUS4_C2LO_ROM_SIZE];
extern uint8_t extromhi3[PLUS4_C2HI_ROM_SIZE];

extern int plus4_mem_init_resources(void);
extern int plus4_mem_init_cmdline_options(void);

extern void mem_config_ram_set(unsigned int config);
extern void mem_config_rom_set(unsigned int config);
extern uint8_t *mem_get_tedmem_base(unsigned int segment);

extern void mem_proc_port_trigger_flux_change(unsigned int on);
extern void pio1_set_tape_sense(int sense);
extern void mem_proc_port_set_write_in(int val);
extern void mem_proc_port_set_motor_in(int val);

extern void plus4io_init(void);
extern void plus4_pio1_init(int block);

extern uint8_t read_unused(uint16_t addr);

extern void store_bank_io(uint16_t addr, uint8_t byte);

#endif
