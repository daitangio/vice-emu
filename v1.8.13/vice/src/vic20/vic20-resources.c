/*
 * vic20-resources.c
 *
 * Written by
 *  Andr� Fachat <fachat@physik.tu-chemnitz.de>
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "vic20mem.h"
#include "resources.h"
#include "ui.h"
#include "utils.h"

/* Name of the character ROM.  */
static char *chargen_rom_name;

/* Name of the BASIC ROM.  */
static char *basic_rom_name;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name;

/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;

/* Flag: Do we enable the VIC-1112 IEEE488 interface?  */
int ieee488_enabled;

/* which ROMs are loaded - bits are VIC_BLK* */
int mem_rom_blocks;

/* Flag: Do we have RAM block `n'?  */
int ram_block_0_enabled;
int ram_block_1_enabled;
int ram_block_2_enabled;
int ram_block_3_enabled;
int ram_block_5_enabled;

/* ------------------------------------------------------------------------- */

static int set_chargen_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_rom_name, (const char *)v))
        return 0;

    return mem_load_chargen(chargen_rom_name);
}

static int set_kernal_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_rom_name, (const char *)v))
        return 0;

    return mem_load_kernal(kernal_rom_name);
}

static int set_basic_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basic_rom_name, (const char *)v))
        return 0;

    return mem_load_basic(basic_rom_name);
}

/* Ugly hack...  */
#define DEFINE_SET_BLOCK_FUNC(num)                                            \
    static int set_ram_block_##num##_enabled(resource_value_t v, void *param) \
    {                                                                         \
        int value = (int)v;                                                   \
                                                                              \
        ram_block_##num##_enabled = value;                                    \
        if (value) {                                                          \
            mem_rom_blocks &= (VIC_ROM_BLK##num##A | VIC_ROM_BLK##num##B);    \
            return vic20_mem_enable_ram_block(num);                           \
        } else                                                                \
            return vic20_mem_disable_ram_block(num);                          \
    }

DEFINE_SET_BLOCK_FUNC(0)
DEFINE_SET_BLOCK_FUNC(1)
DEFINE_SET_BLOCK_FUNC(2)
DEFINE_SET_BLOCK_FUNC(3)
DEFINE_SET_BLOCK_FUNC(5)

static int set_emu_id_enabled(resource_value_t v, void *param)
{
    emu_id_enabled = (int)v;
    return 0;
}

static int set_ieee488_enabled(resource_value_t v, void *param)
{
    ieee488_enabled = (int)v;

    ui_update_menus();

    return 0;
}

/* Enable/disable the Emulator ID.  */
void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}

static resource_t resources[] =
{
    {"ChargenName", RES_STRING, (resource_value_t)"chargen",
     (resource_value_t *)&chargen_rom_name,
     set_chargen_rom_name, NULL },
    {"KernalName", RES_STRING, (resource_value_t)"kernal",
     (resource_value_t *)&kernal_rom_name,
     set_kernal_rom_name, NULL },
    {"BasicName", RES_STRING, (resource_value_t)"basic",
     (resource_value_t *)&basic_rom_name,
     set_basic_rom_name, NULL },
    {"RAMBlock0", RES_INTEGER, (resource_value_t)1,
     (resource_value_t *)&ram_block_0_enabled,
     set_ram_block_0_enabled, NULL },
    {"RAMBlock1", RES_INTEGER, (resource_value_t)1,
     (resource_value_t *)&ram_block_1_enabled,
     set_ram_block_1_enabled, NULL },
    {"RAMBlock2", RES_INTEGER, (resource_value_t)1,
     (resource_value_t *)&ram_block_2_enabled,
     set_ram_block_2_enabled, NULL },
    {"RAMBlock3", RES_INTEGER, (resource_value_t)1,
     (resource_value_t *)&ram_block_3_enabled,
     set_ram_block_3_enabled, NULL },
    {"RAMBlock5", RES_INTEGER, (resource_value_t)1,
     (resource_value_t *)&ram_block_5_enabled,
     set_ram_block_5_enabled, NULL },
    {"EmuID", RES_INTEGER, (resource_value_t)0,
     (resource_value_t *)&emu_id_enabled,
     set_emu_id_enabled, NULL },
    {"IEEE488", RES_INTEGER, (resource_value_t)0,
     (resource_value_t *)&ieee488_enabled,
     set_ieee488_enabled, NULL },
    {NULL}
};

int vic20_resources_init(void)
{
    return resources_register(resources);
}

