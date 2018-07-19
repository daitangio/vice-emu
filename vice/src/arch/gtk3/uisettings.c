/** \file   uisettings.c
 * \brief   GTK3 main settings dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES SaveResourcesOnExit     all
 * $VICERES ConfirmOnExit           all
 */

/*
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
 */


/* The settings_grid is supposed to become this:
 *
 * +--------------+---------------------------+
 * | treeview     |                           |
 * |  with        |                           |
 * |   settings   |    central widget,        |
 * |  more        |    depending on which     |
 * |   foo        |    item is selected in    |
 * |   bar        |    the treeview           |
 * |    whatever  |                           |
 * | burp         |                           |
 * +--------------+---------------------------+
 *
 * And this is handled by the dialog itself:
 * +------------------------------------------+
 * | load | save | load... | save... | close  |
 * +------------------------------------------+
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lib.h"
#include "util.h"
#include "machine.h"
#include "resources.h"
#include "vsync.h"

#include "vice_gtk3.h"

#include "ui.h"
#include "settings_speed.h"
#include "settings_keyboard.h"
#include "settings_sound.h"
#include "settings_autostart.h"
#include "settings_drive.h"
#include "settings_model.h"
#include "settings_misc.h"
#include "settings_ramreset.h"
#include "settings_video.h"
#include "settings_sampler.h"
#include "settings_printer.h"
#include "settings_controlport.h"
#include "settings_joystick.h"
#include "settings_mouse.h"
#include "settings_soundchip.h"
#include "settings_monitor.h"
#include "settings_romset.h"
#include "settings_snapshot.h"
#ifdef HAVE_RAWNET
# include "settings_ethernet.h"
#endif
#include "settings_rs232.h"
#include "scpu64settingswidget.h"
#include "c64dtvflashsettingswidget.h"

/* I/O extension widgets */
#include "settings_io.h"
#include "settings_io_c64_memhacks.h"
#include "settings_io_georam.h"
#include "reuwidget.h"
#include "ramcartwidget.h"
#include "dqbbwidget.h"
#include "expertwidget.h"
#include "isepicwidget.h"
#include "easyflashwidget.h"
#include "gmod2widget.h"
#include "mmcrwidget.h"
#include "mmc64widget.h"
#include "ide64widget.h"
#include "retroreplaywidget.h"

#ifdef HAVE_RAWNET
# include "ethernetcartwidget.h"
# include "rrnetmk3widget.h"
#endif

#include "c128functionromwidget.h"
#include "ieee488widget.h"
#include "digimaxwidget.h"
#include "magicvoicewidget.h"
#include "midiwidget.h"
#include "sfxsoundexpanderwidget.h"
#include "ds12c887widget.h"
#include "userportdeviceswidget.h"
#include "tapeportdeviceswidget.h"
#include "sidcartwidget.h"
#include "v364speechwidget.h"
#include "sfxsoundsamplerwidget.h"
#include "megacartwidget.h"
#include "petreuwidget.h"
#include "petcolourgraphicswidget.h"
#include "petdwwwidget.h"
#include "supersnapshotwidget.h"
#include "cpmwidget.h"
#include "burstmodewidget.h"
#include "c128fullbankswidget.h"
#include "plus4aciawidget.h"
#include "plus4digiblasterwidget.h"
#include "finalexpansionwidget.h"
#include "vicflashwidget.h"
#include "ultimemwidget.h"
#include "vicieee488widget.h"
#include "vicioramwidget.h"
#include "vfliwidget.h"
#include "petdiagpinwidget.h"
#include "pethrewidget.h"
#include "settings_crt.h"
#include "settings_mixer.h"

/* VSID stuff */
#include "hvscsettingswidget.h"

#include "uisettings.h"


/** \brief  Number of columns in the tree model
 */
#define NUM_COLUMNS 3


/** \brief  Column indici for the tree model
 */
enum {
    COLUMN_NAME = 0,    /**< name */
    COLUMN_ID,          /**< id */
    COLUMN_CALLBACK     /**< callback function */
};


/** \brief  Initial dialog width
 *
 * This is not how wide the dialog will actually become, that is determined by
 * the Gtk theme applied. But it's a rough estimate.
 */
#define DIALOG_WIDTH 800


/** \brief  Initial dialog height
 *
 * This is not how tall the dialog will actually become, that is determined by
 * the Gtk theme applied. But it's a rough estimate.
 */
#define DIALOG_HEIGHT 560


/** \brief  Maximum width the UI can be
 *
 * This again is not a really a fixed value, but more of an indicator when the
 * UI might get too large after any decorations are applied. The idea is to
 * have a UI that works on a 1280x768 resolution without requiring scrollbars.
 */
#define DIALOG_WIDTH_MAX 1024


/** \brief  Maximum height the UI can be
 *
 * This again is not a really a fixed value, but more of an indicator when the
 * UI might get too large after any decorations are applied. The idea is to
 * have a UI that works on a 1280x768 resolution without requiring scrollbars.
 */
#define DIALOG_HEIGHT_MAX 640


/** \brief  Enum used for the "response" callback of the settings dialog
 *
 * All values must be positive since Gtk reserves standard responses in its
 * GtkResponse enum as negative values.
 */
enum {
    RESPONSE_RESET = 1, /**< reset current central widget */
    RESPONSE_FACTORY,   /**< set current central widget's resources to their
                             factory settings */
    RESPONSE_DEFAULT    /**< Restore default settings */
};


/*
 * I/O extensions per emulator
 */

/* {{{ c64_io_extensions */
/** \brief  List of C64 I/O extensions (x64, x64sc)
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t c64_io_extensions[] = {
    { "Memory Expansion Hacks",
       "mem-hacks",
       settings_io_c64_memhacks_widget_create, NULL },

    { "GEO-RAM",
        "geo-ram",
        settings_io_georam_widget_create, NULL },
    { "RAM Expansion Module",
        "reu",
        reu_widget_create, NULL },
    { "RamCart",
        "ramcart",
        ramcart_widget_create, NULL },

    { "Double Quick Brown Box",
        "dqbb",
        dqbb_widget_create, NULL },
    { "Expert Cartridge",
        "expert-cart",
        expert_widget_create, NULL },
    { "ISEPIC",
        "isepic",
        isepic_widget_create, NULL },

    { "EasyFlash",
        "easyflash",
        easyflash_widget_create, NULL },
    { "GMod2",
        "gmod2",
        gmod2_widget_create, NULL },
    { "IDE64",
        "ide64",
        ide64_widget_create, NULL },
    { "MMC64",
        "mmc64",
        mmc64_widget_create, NULL },
    { "MMC Replay",
        "mmcr",
        mmcr_widget_create, NULL },
    { "Retro Replay",
        "retroreplay",
        retroreplay_widget_create, NULL },
    { "Super Snapshot V5",
        "super-snapshot",
        super_snapshot_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
        "rrnetmk3",
        rrnetmk3_widget_create, NULL },
#endif

    { "IEEE-448 Interface",
        "ieee-488",
        ieee488_widget_create, NULL },
    { "Burst Mode Modification",
        "burstmode-mode",
        burst_mode_widget_create, NULL },

    { "DigiMAX",
        "digimax",
        digimax_widget_create, NULL },
    { "Magic Voice",
        "magic-voice",
        magic_voice_widget_create, NULL },
    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "SFX Sound Expander",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },
    { "CP/M Cartridge",
        "cpm-cart",
        cpm_widget_create, NULL },

    { "DS12C887 Real Time Clock",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ scpu64_io_extensions */
/** \brief  List of SuperCPU64 extensions (xscpu64)
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t scpu64_io_extensions[] = {
    { "GEO-RAM",
        "geo-ram",
        settings_io_georam_widget_create, NULL },
    { "RAM Expansion Module",
        "reu",
        reu_widget_create, NULL },
    { "RamCart",
        "ramcart",
        ramcart_widget_create, NULL },

    { "Double Quick Brown Box",
        "dqbb",
        dqbb_widget_create, NULL },
    { "Expert Cartridge",
        "expert-cart",
        expert_widget_create, NULL },
    { "ISEPIC",
        "isepic",
        isepic_widget_create, NULL },

    { "EasyFlash",
        "easyflash",
        easyflash_widget_create, NULL },
    { "GMod2",
        "gmod2",
        gmod2_widget_create, NULL },
    { "IDE64",
        "ide64",
        ide64_widget_create, NULL },
    { "MMC64",
        "mmc64",
        mmc64_widget_create, NULL },
    { "MMC Replay",
        "mmcr",
        mmcr_widget_create, NULL },
    { "Retro Replay",
        "retroreplay",
        retroreplay_widget_create, NULL },
    { "Super Snapshot V5",
        "super-snapshot",
        super_snapshot_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
        "rrnetmk3",
        rrnetmk3_widget_create, NULL },
#endif

    { "IEEE-448 Interface",
        "ieee-488",
        ieee488_widget_create, NULL },
    { "Burst Mode Modification",
        "burstmode-mode",
        burst_mode_widget_create, NULL },

    { "DigiMAX",
        "digimax",
        digimax_widget_create, NULL },
    { "Magic Voice",
        "magic-voice",
        magic_voice_widget_create, NULL },
    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "SFX Sound Expander",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },
    { "CP/M Cartridge",
        "cpm-cart",
        cpm_widget_create, NULL },

    { "DS12C887 Real Time Clock",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ c128_io_extensions */
/** \brief  I/O extensions for C128
 */
static ui_settings_tree_node_t c128_io_extensions[] = {
    { "Function ROM",
        "function-rom",
        c128_function_rom_widget_create, NULL },
    { "Banks 2 & 3",
        "banks-23",
        c128_full_banks_widget_create, NULL },

    { "GEO-RAM",
        "geo-ram",
        settings_io_georam_widget_create, NULL },
    { "RAM Expansion Module",
        "reu",
        reu_widget_create, NULL },
    { "RamCart",
        "ramcart",
        ramcart_widget_create, NULL },

    { "Double Quick Brown Box",
        "dqbb",
        dqbb_widget_create, NULL },
    { "Expert Cartridge",
        "expert-cart",
        expert_widget_create, NULL },
    { "ISEPIC",
        "isepic",
        isepic_widget_create, NULL },

    { "EasyFlash",
        "easyflash",
        easyflash_widget_create, NULL },
    { "GMod2",
        "gmod2",
        gmod2_widget_create, NULL },
    { "IDE64",
        "ide64",
        ide64_widget_create, NULL },
    { "MMC64",
        "mmc64",
        mmc64_widget_create, NULL },
    { "MMC Replay",
        "mmcr",
        mmcr_widget_create, NULL },
    { "Retro Replay",
        "retroreplay",
        retroreplay_widget_create, NULL },
    { "Super Snapshot V5",
        "super-snapshot",
        super_snapshot_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
    { "RR-Net Mk3",
        "rrnetmk3",
        rrnetmk3_widget_create, NULL },
#endif

    { "IEEE-448 Interface",
        "ieee-488",
        ieee488_widget_create, NULL },
    { "Burst Mode Modification",
        "burstmode-mode",
        burst_mode_widget_create, NULL },

    { "DigiMAX",
        "digimax",
        digimax_widget_create, NULL },
    { "Magic Voice",
        "magic-voice",
        magic_voice_widget_create, NULL },
    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "SFX Sound Expander",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },
    { "CP/M Cartridge",
        "cpm-cart",
        cpm_widget_create, NULL },

    { "DS12C887 Real Time Clock",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ vic20_io_extensions */
/** \brief  List of VIC-20 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t vic20_io_extensions[] = {
    { "Mega Cart",
        "mega-cart",
        mega_cart_widget_create, NULL },
    { "Final Expansion",
        "final-expansion",
        final_expansion_widget_create, NULL },
    { "Vic Flash Plugin",
        "vic-flash-plugin",
        vic_flash_widget_create, NULL },
    { "UltiMem",
        "ultimem",
        ultimem_widget_create, NULL },
    { "SID Cartridge",
        "sid-cart",
        sidcart_widget_create, NULL },
    { "VIC-1112 IEEE-488 interface",
        "ieee-488",
        vic_ieee488_widget_create, NULL },
    { "I/O RAM",
        "io-ram",
        vic_ioram_widget_create, NULL },
    { "VFLI modification",
        "vfli",
        vfli_widget_create, NULL },

    { "DigiMAX (MasC=uerade",
        "digimax",
        digimax_widget_create, NULL },
    { "DS12C887 RTC (MasC=uerade)",
        "ds12c887-rtc",
        ds12c887_widget_create, NULL },
    { "GEO-RAM (MasC=uerade)",
       "geo-ram",
       settings_io_georam_widget_create, NULL },
    { "SFX Sound Expander (MasC=uerade)",
        "sfx-expander",
        sfx_sound_expander_widget_create, NULL },
    { "SFX Sound Sampler (MasC=uerade)",
        "sfx-sampler",
        sfx_sound_sampler_widget_create, NULL },

#ifdef HAVE_RAWNET
    { "Ethernet Cartridge (MasC=uerade)",
        "ethernet-cart",
        ethernet_cart_widget_create, NULL },
#endif

    { "MIDI emulation",
        "midi",
        midi_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tapeport devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ plus4_io_extensions */
/** \brief  List of Plus4 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t plus4_io_extensions[] = {
    { "ACIA",
        "acia",
        plus4_acia_widget_create, NULL },
    { "Digiblaster add-on",
        "digiblaster",
        plus4_digiblaster_widget_create, NULL },
    { "SID Cartridge",
        "sid-cart",
        sidcart_widget_create, NULL },
    { "V364 Speech",
        "v364",
        v364_speech_widget_create, NULL },

    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ pet_io_extensions */
/** \brief  List of PET I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t pet_io_extensions[] = {
    { "PET RAM Expansion Unit",
        "pet-reu",
        pet_reu_widget_create, NULL },
    { "PET Colour graphics",
        "pet-colour",
        pet_colour_graphics_widget_create, NULL },
    { "PET DWW hi-res graphics",
        "pet-dww",
        pet_dww_widget_create, NULL },
    { "PET HRE hi-res graphics",
        "pet-hre",
        pet_hre_widget_create, NULL },
    { "SID Cartridge",
        "sid-cart",
        sidcart_widget_create, NULL },
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },
    { "PET userport diagnostic pin",
        "pet-diagpin",
        pet_diagpin_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ cbm5x0_io_extensions */
/** \brief  List of CBM 5x0 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t cbm5x0_io_extensions[] = {
    { "Tape port devices",
        "tapeport-devices",
        tapeport_devices_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ cbm6x0_io_extensions */
/** \brief  List of CBM 6x0 I/O extensions
 *
 * Every empty line indicates a separator in the Gtk2 UI's menu
 */
static ui_settings_tree_node_t cbm6x0_io_extensions[] = {
    { "Userport devices",
        "userport-devices",
        userport_devices_widget_create, NULL },
    { "Tape port devices",
        "tapeport_devices",
        tapeport_devices_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*
 * Main tree nodes per emulator
 */

/*****************************************************************************
 *                  VSID tree nodes for the settings UI                      *
 ****************************************************************************/

/* {{{ main_nodes_vsid */
/** \brief  Main tree nodes for VSID
 */
static ui_settings_tree_node_t main_nodes_vsid[] = {
   { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    /* XXX: basically a selection between 'PAL'/'NTSC' (50/60Hz) */
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    /* XXX: do we need this? Sidplay allows ROM selection for Basic, Kernal and
     *      Chargen, perhaps move into model settings, plenty of space there */
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    /* XXX: perhaps required for VSID-specific things */
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    { "HVSC settings",
      "hvsc",
      hvsc_settings_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  C64 tree nodes for the settings UI                       *
 ****************************************************************************/

/* {{{ machine_nodes_c64 */
static ui_settings_tree_node_t machine_nodes_c64[] = {
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_c64 */
static ui_settings_tree_node_t display_nodes_c64[] = {
    { "VIC-II settings",
      "vicii",
      settings_video_create, NULL },
    { "CRT settings",
        "crt",
        settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_c64 */
static ui_settings_tree_node_t audio_nodes_c64[] = {
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_c64 */
static ui_settings_tree_node_t input_nodes_c64[] = {
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_c64 */
static ui_settings_tree_node_t peripheral_nodes_c64[] = {
    /* "Output devices? drive is also input */
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/* {{{ main_nodes_c64 */
/** \brief  Main tree nodes for x64/x64sc
 */
static ui_settings_tree_node_t main_nodes_c64[] = {

    { "Machine settings",
        "machine",
        NULL, machine_nodes_c64 },
    { "Display settings",
        "display",
        NULL, display_nodes_c64 },
    { "Audio settings",
        "audio",
        NULL, audio_nodes_c64 },
    { "Input devices",
        "input",
        NULL, input_nodes_c64 },
    { "Peripheral devices",
        "peripheral", /* I'll misspell this many times */
        NULL, peripheral_nodes_c64 },
   { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, c64_io_extensions },
    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                  C64DTV tree nodes for the settings UI                    *
 ****************************************************************************/

/* {{{ main_nodes_c64dtv */
/** \brief  Main tree nodes for x64dtv
 */
static ui_settings_tree_node_t main_nodes_c64dtv[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "VIC-II settings",
      "vicii",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Flash settings",
      "flash",
      c64dtv_flash_settings_widget_create, NULL },
    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, NULL },

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
/* }}} */


/*****************************************************************************
 *                      C128 tree nodes for the settings UI                  *
 ****************************************************************************/

/* {{{ machine_nodes_c128 */
/** \brief  Machine subnodes for c128
 */
static ui_settings_tree_node_t machine_nodes_c128[] = {
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ display_nodes_c128 */
/** \brief  C128 Display sub nodes
 */
static ui_settings_tree_node_t display_nodes_c128[] = {
    { "VIC-II/VDC settings",
      "vicii-vdc",
      settings_video_create, NULL },
    { "CRT settings",
      "crt",
      settings_crt_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ audio_nodes_c128 */
/** \brief  C128 Audio sub nodes
 */
static ui_settings_tree_node_t audio_nodes_c128[] = {
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ input_nodes_c128 */
/** \brief  C128 Input sub nodes
 */
static ui_settings_tree_node_t input_nodes_c128[] = {
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

/* {{{ peripheral_nodes_c128 */
/** \brief  C128 Peripheral sub nodes
 */
static ui_settings_tree_node_t peripheral_nodes_c128[] = {
    /* "Output devices? drive is also input */
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    UI_SETTINGS_TERMINATOR
};
/* }}} */


/* {{{ main_nodes_c128 */
/** \brief  Main tree nodes for x128
 */
static ui_settings_tree_node_t main_nodes_c128[] = {
    { "Machine settings",
        "machine",
        NULL, machine_nodes_c128 },
    { "Display settings",
        "display",
        NULL, display_nodes_c128 },
    { "Audio settings",
        "audio",
        NULL, audio_nodes_c128 },
    { "Input devices",
        "input",
        NULL, input_nodes_c128 },
    { "Peripheral devices",
        "peripheral", /* I'll misspell this many times */
        NULL, peripheral_nodes_c128 },
   { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, c128_io_extensions },
    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    UI_SETTINGS_TERMINATOR
};
/* }}} */

#if 0
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "VIC-II/VDC settings",
      "vicii-vdc",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, c128_io_extensions },
#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};
#endif

/** \brief  Main tree nodes for xscpu64
 */
static ui_settings_tree_node_t main_nodes_scpu64[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "SCPU64 settings",
      "scpu64",
      scpu64_settings_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "VIC-II settings",
      "vicii",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, scpu64_io_extensions },
#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif
    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  Main tree nodes for xvic
 */
static ui_settings_tree_node_t main_nodes_vic20[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "VIC settings",
      "vic",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, vic20_io_extensions },

#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  Main tree nodes for xplus4
 */
static ui_settings_tree_node_t main_nodes_plus4[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "TED settings",
      "ted",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, plus4_io_extensions },

#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  Main tree nodes for xpet
 */
static ui_settings_tree_node_t main_nodes_pet[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "CRTC settings",
      "crtc",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, pet_io_extensions },
#ifdef HAVE_RS232DEV
    { "RS232 settings",
      "rs232",
      settings_rs232_widget_create, NULL },
#endif
#ifdef HAVE_RAWNET
    { "Ethernet settings",
      "ethernet",
      settings_ethernet_widget_create, NULL },
#endif

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  Main tree nodes for xcbm5x0
 */
static ui_settings_tree_node_t main_nodes_cbm5x0[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "VIC-II settings",
      "vicii",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, cbm5x0_io_extensions },

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};


/** \brief  Main tree nodes for xcbm6x0
 */
static ui_settings_tree_node_t main_nodes_cbm6x0[] = {
    { "CRT settings",
      "CRT",
      settings_crt_widget_create, NULL },
    { "Mixer settings",
       "mixer",
       settings_mixer_widget_create, NULL },
    { "Speed settings",
      "speed",
       settings_speed_widget_create, NULL },
    { "Keyboard settings",
      "keyboard",
      settings_keyboard_widget_create, NULL },
    { "Sound settings",
      "sound",
      settings_sound_create, NULL },
    { "Sampler settings",
      "sampler",
      settings_sampler_widget_create, NULL },
    { "Autostart settings",
      "autostart",
      settings_autostart_widget_create, NULL },
    { "Drive settings",
      "drive",
      settings_drive_widget_create, NULL },
    { "Printer settings",
      "printer",
      settings_printer_widget_create, NULL },
    { "Control port settings",
      "control-port",
      settings_controlport_widget_create, NULL },
    { "Joystick settings",
      "joystick",
      settings_joystick_widget_create, NULL },
    { "Mouse settings",
      "mouse",
      settings_mouse_widget_create, NULL },
    { "Model settings",
      "model",
      settings_model_widget_create, NULL },
    { "RAM reset pattern",
      "ram-reset",
      settings_ramreset_widget_create, NULL },
    { "ROM settings",
      "rom-settings",
      settings_romset_widget_create, NULL },
    { "Miscellaneous",
      "misc",
      settings_misc_widget_create, NULL },
    { "CRTC settings",
      "crtc",
      settings_video_create, NULL },
    { "SID settings",
      "sid",
      settings_soundchip_widget_create, NULL },

    { "I/O extensions",
      "io-extensions",
      settings_io_widget_create, cbm6x0_io_extensions },

    { "Snaphot/event/media recording",
      "snapshot",
      settings_snapshot_widget_create, NULL },
    { "Monitor settings",
      "monitor",
      settings_monitor_widget_create, NULL },

    UI_SETTINGS_TERMINATOR
};



/** \brief  Reference to the current 'central' widget in the settings dialog
 */
static void ui_settings_set_central_widget(GtkWidget *widget);


/** \brief  Reference to the settings dialog
 *
 * Used to show/hide the widget without rebuilding it each time. Clean up
 * with ui_settings_dialog_shutdown()
 */
static GtkWidget *settings_window = NULL;


/** \brief  Reference to the 'content area' widget of the settings dialog
 */
static GtkWidget *settings_grid = NULL;


/** \brief  Reference to the tree model for the settings tree
 */
static GtkTreeStore *settings_model = NULL;


/** \brief  Reference to the tree view for the settings tree
 */
static GtkWidget *settings_tree = NULL;


/** \brief  Path to the last used settings page
 */
static GtkTreePath *last_node_path = NULL;


/** \brief  Reference to the resource widget manager of the current page
 */
static resource_widget_manager_t *resource_manager = NULL;


/** \brief  Set reference to the resource widget manager of the current page
 *
 * \param[in]   manager resource widget manager reference
 */
void ui_settings_set_resource_widget_manager(resource_widget_manager_t *manager)
{
    resource_manager = manager;
}


/** \brief  Reset widgets in the central widget to their initial state
 *
 * Restores all widgets in the central widget to the state they were in when\
 * the central widget was instanciated. This requires a call from the central
 * widget registering its "resource manager widget".
 *
 * \param[in]   widget  settings dialog
 * \param[in]   data    extra event data (unused at the moment)
 */
static void ui_settings_central_widget_reset(GtkWidget *widget, gpointer data)
{
    debug_gtk3("Resetting current page's widgets to their initial state");
    if (resource_manager != NULL) {
        vice_resource_widget_manager_reset(resource_manager);
    } else {
        debug_gtk3("No resource widget manager registered, skipping");
    }
}


/** \brief  Reset widgets in the central widget to their factory state
 *
 * Restores all widgets in the central widget to the factory state of the
 * resources they are bound to.
 * This requires a call from the central widget registering its
 * "resource manager widget".
 *
 * \param[in]   widget  settings dialog
 * \param[in]   data    extra event data (unused at the moment)
 */
static void ui_settings_central_widget_factory(GtkWidget *widget, gpointer data)
{
    debug_gtk3("Resetting current page's widgets to their factory value");
    if (resource_manager != NULL) {
        vice_resource_widget_manager_factory(resource_manager);
    } else {
        debug_gtk3("No resource widget manager registered, skipping");
    }

}


/** \brief  Create the widget that is initially shown in the settings UI
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
static GtkWidget *ui_settings_inital_widget(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(64, 64);
    label = gtk_label_new(NULL);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_markup(GTK_LABEL(label),
            "This is the first widget/dialog shown when people click on the"
            " settings UI.\n"
            "So perhaps we could show some instructions or something here.\n\n"
            "<b>TODO</b>: remember previous settings-tree location");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


#if 0
/** \brief  Paused state when popping up the UI
 */
static int old_pause_state;
#endif

/** \brief  Handler for the "changed" event of the tree view
 *
 * \param[in]   selection   GtkTreeSelection associated with the tree model
 * \param[in]   user_data   data for the event (unused for now)
 *
 */
static void on_tree_selection_changed(
        GtkTreeSelection *selection,
        gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *name;
        GtkWidget *(*callback)(void *) = NULL;
        const char *id;

        gtk_tree_model_get(model, &iter, COLUMN_NAME, &name, -1);
        gtk_tree_model_get(model, &iter, COLUMN_CALLBACK, &callback, -1);
        gtk_tree_model_get(model, &iter, COLUMN_ID, &id, -1);
        debug_gtk3("node name: %s", name);
        debug_gtk3("node ID: %s", id);
        if (callback != NULL) {
            char *title = lib_msprintf("%s settings :: %s", machine_name, name);
            gtk_window_set_title(GTK_WINDOW(settings_window), title);
            lib_free(title);
            /* create new central widget, using settings_window (this dialog)
             * as its parent, this will allow for proper blocking in modal
             * dialogs, while ui_get_active_window() breaks that. */
            if (last_node_path != NULL) {
                gtk_tree_path_free(last_node_path);
            }
            last_node_path = gtk_tree_model_get_path(
                    GTK_TREE_MODEL(settings_model), &iter);

            /* reset widget manager */
            resource_manager = NULL;
            ui_settings_set_central_widget(callback(settings_window));
        }
        g_free(name);
    }
}


/** \brief  Create the 'Save on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_save_on_exit_checkbox(void)
{
    return vice_gtk3_resource_check_button_new("SaveResourcesOnExit",
            "Save settings on exit");
}


/** \brief  Create the 'Confirm on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_confirm_on_exit_checkbox(void)
{
    return vice_gtk3_resource_check_button_new("ConfirmOnExit",
            "Confirm on exit");
}


/** \brief  Create empty tree model for the settings tree
 */
static void create_tree_model(void)
{
    settings_model = gtk_tree_store_new(NUM_COLUMNS,
            G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
}


/** \brief  Create tree store containing settings items and children
 *
 * \return  GtkTreeStore
 */
static GtkTreeStore *populate_tree_model(void)
{
    GtkTreeStore *model;
    GtkTreeIter iter;
    GtkTreeIter child;
    ui_settings_tree_node_t *nodes = NULL;
    int i;

    model = settings_model;

    switch (machine_class) {
        case VICE_MACHINE_C64:  /* fall through */
        case VICE_MACHINE_C64SC:
            nodes = main_nodes_c64;
            break;
        case VICE_MACHINE_C64DTV:
            nodes = main_nodes_c64dtv;
            break;
        case VICE_MACHINE_C128:
            nodes = main_nodes_c128;
            break;
        case VICE_MACHINE_SCPU64:
            nodes = main_nodes_scpu64;
            break;
        case VICE_MACHINE_VIC20:
            nodes = main_nodes_vic20;
            break;
        case VICE_MACHINE_PLUS4:
            nodes = main_nodes_plus4;
            break;
        case VICE_MACHINE_PET:
            nodes = main_nodes_pet;
            break;
        case VICE_MACHINE_CBM5x0:
            nodes = main_nodes_cbm5x0;
            break;
        case VICE_MACHINE_CBM6x0:
            nodes = main_nodes_cbm6x0;
            break;
        case VICE_MACHINE_VSID:
            nodes = main_nodes_vsid;
            break;
        default:
            fprintf(stderr,
                    "Error: %s:%d:%s(): unsupported machine_class %d\n",
                    __FILE__, __LINE__, __func__, machine_class);
            exit(1);
            break;
    }

    for (i = 0; nodes[i].name != NULL; i++) {
        gtk_tree_store_append(model, &iter, NULL);
        gtk_tree_store_set(model, &iter,
                COLUMN_NAME, nodes[i].name,
                COLUMN_ID, nodes[i].id,
                COLUMN_CALLBACK, nodes[i].callback,
                -1);

        /* this bit will need proper recursion if we need more than two
         * levels of subitems */
        if (nodes[i].children != NULL) {
            int c;
            ui_settings_tree_node_t *list = nodes[i].children;

            for (c = 0; list[c].name != NULL; c++) {
                char buffer[256];

                /* mark items without callback with 'TODO' */
                if (list[c].callback != NULL) {
                    g_snprintf(buffer, 256, "%s", list[c].name);
                } else {
                    g_snprintf(buffer, 256, "TODO: %s", list[c].name);
                }

                gtk_tree_store_append(model, &child, &iter);
                gtk_tree_store_set(model, &child,
                        COLUMN_NAME, buffer,
                        COLUMN_ID, list[c].id,
                        COLUMN_CALLBACK, list[c].callback,
                        -1);
            }
        }
    }
    return model;
}


/** \brief  Create treeview for settings side-menu
 *
 * Reads items from `main_nodes` and adds them to the tree view.
 *
 * \return  GtkTreeView
 *
 * TODO:    Handle nested items, and write up somewhere how the hell I finally
 *          got the callbacks working.
 *          Split into a function creating the tree view and functions adding,
 *          altering or removing nodes.
 */
static GtkWidget *create_treeview(void)
{
    GtkWidget *tree;
    GtkCellRenderer *text_renderer;
    GtkTreeViewColumn *text_column;

    create_tree_model();
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(populate_tree_model()));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

    text_renderer = gtk_cell_renderer_text_new();
    text_column = gtk_tree_view_column_new_with_attributes(
            "item-name",
            text_renderer,
            "text", 0,
            NULL);
    /*    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), obj_column); */
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), text_column);

    return tree;
}


/** \brief  Set the 'central'/action widget for the settings dialog
 *
 * Destroys the old 'central' widget and sets the new one.
 *
 *  \param[in,out]  widget  widget to use as the new 'central' widget
 */
static void ui_settings_set_central_widget(GtkWidget *widget)
{
    GtkWidget *child;

    child = gtk_grid_get_child_at(GTK_GRID(settings_grid), 1, 0);
    if (child != NULL) {
        gtk_widget_destroy(child);
    }
    gtk_grid_attach(GTK_GRID(settings_grid), widget, 1, 0, 1, 1);
    /* add a little space around the widget */
    g_object_set(widget, "margin", 16, NULL);
}


/** \brief  Create the 'content widget' of the settings dialog
 *
 * This creates the widget in the dialog used to display the treeview and room
 * for the widget connected to that tree's currently selected item.
 *
 * \param[in]   widget  parent widget
 *
 * \return  GtkGrid (as a GtkWidget)
 */
static GtkWidget *create_content_widget(GtkWidget *widget)
{
    GtkTreeSelection *selection;
    GtkWidget *scroll;
    GtkWidget *extra;

    settings_grid = gtk_grid_new();
    settings_tree = create_treeview();

    /* pack the tree in a scrolled window to allow scrolling of the tree when
     * it gets too large for the dialog
     */
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), settings_tree);

    gtk_grid_attach(GTK_GRID(settings_grid), scroll, 0, 0, 1, 1);

    /* Remember the previously selected setting/widget and set it here */

    /* do we have a previous settings "page"? */
    if (last_node_path == NULL) {
        /* nope, display the default one */
        ui_settings_set_central_widget(ui_settings_inital_widget(widget));
    } else {
        /* try to restore the page last shown */
        GtkTreeIter iter;

        debug_gtk3("Attempting to get previous settings page");
        if (!gtk_tree_model_get_iter(GTK_TREE_MODEL(settings_model), &iter,
                    last_node_path)) {
            debug_gtk3("Oops");
        } else {
            GtkWidget *(*callback)(GtkWidget *) = NULL;
            gtk_tree_model_get(
                    GTK_TREE_MODEL(settings_model), &iter,
                    COLUMN_CALLBACK, &callback, -1);
            if (callback != NULL) {

                GtkTreeSelection *selection;

                selection = gtk_tree_view_get_selection(
                        GTK_TREE_VIEW(settings_tree));

                ui_settings_set_central_widget(callback(widget));
                gtk_tree_view_expand_to_path(
                        GTK_TREE_VIEW(settings_tree),
                        last_node_path);
                gtk_tree_selection_select_path(selection, last_node_path);

            }
        }
    }

    /* create container for generic settings */
    extra = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(extra), 8);
    gtk_grid_set_row_spacing(GTK_GRID(extra), 8);
    g_object_set(extra, "margin", 16, NULL);

    gtk_grid_attach(GTK_GRID(extra), create_save_on_exit_checkbox(),
            0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(extra), create_confirm_on_exit_checkbox(),
            1, 0, 1, 1);

    /* add to main layout */
    gtk_grid_attach(GTK_GRID(settings_grid), extra, 0, 2, 2, 1);

    gtk_widget_show(settings_grid);
    gtk_widget_show(settings_tree);

    gtk_widget_set_size_request(scroll, 250, 500);
    gtk_widget_set_size_request(settings_grid, DIALOG_WIDTH, DIALOG_HEIGHT);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(settings_tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selection), "changed",
            G_CALLBACK(on_tree_selection_changed), NULL);


    return settings_grid;
}


/** \brief  Handler for the "response" event of the settings dialog
 *
 * This determines what to do based on the 'reponse ID' emitted by the dialog.
 *
 * \param[in]   widget      widget triggering the event (the dialog)
 * \param[in]   response_id response ID
 * \param[in]   user_data   extra data (unused)
 */
static void response_callback(GtkWidget *widget, gint response_id,
                              gpointer user_data)
{
    switch (response_id) {

        /* close dialog */
        case GTK_RESPONSE_DELETE_EVENT:
            gtk_widget_destroy(widget);
            settings_window = NULL;
            break;

        /* reset resources in current central widget to the state they were
         * in before entering the (sub)dialog */
        case RESPONSE_RESET:
            ui_settings_central_widget_reset(widget, user_data);
            break;

        /* restore resources in (sub)dialog to factory settings */
        case RESPONSE_FACTORY:
            ui_settings_central_widget_factory(widget, user_data);
            break;

        case RESPONSE_DEFAULT:
            if (vice_gtk3_message_confirm("Reset to default setting",
                        "Do you wish to reset to default settings?")) {
                resources_set_defaults();
                gtk_widget_destroy(widget);
                /* this one really behaves a little odd: */
#if 0
                machine_reset();
#endif
            }
            break;


        default:
            break;
    }
}


/** \brief  Respond to window size changes
 *
 * This allows for quickly seeing if specific dialog is getting too large. The
 * DIALOG_WIDTH_MAX and DIALOG_HEIGHT_MAX I sucked out of my thumb, since due
 * to window managers using different themes, we can't use 'proper' values, so
 * I had to use approximate values.
 *
 * \param[in]   widget  a GtkWindow
 * \param[in]   event   the GDK event
 * \param[in]   data    extra event data (unused)
 *
 * \return  boolean
 */
static gboolean on_dialog_configure_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer data)
{
    if (event->type == GDK_CONFIGURE) {
        int width = ((GdkEventConfigure*)event)->width;
        int height = ((GdkEventConfigure*)event)->height;

        /* debug_gtk3("width %d, height %d.", width, height); */
        if (width > DIALOG_WIDTH_MAX || height > DIALOG_HEIGHT_MAX) {
            /* uncomment the following to get some 'help' while building
             * new dialogs: */
#if 0
            gtk_window_set_title(GTK_WINDOW(widget),
                    "HELP! --- DIALOG IS TOO BLOODY LARGE -- ERROR!");
#endif
            debug_gtk3("Dialog is too large: %dx%d (max: %dx%d).",
                    width, height, DIALOG_WIDTH_MAX, DIALOG_HEIGHT_MAX);
        }
    }
    return FALSE;
}



/** \brief  Callback to create the main settings dialog from the menu
 *
 * \param[in]   widget      (direct) parent widget, the menu item
 * \param[in]   user_data   data for the event (unused)
 *
 * \note    The appearance of minimize/maximize buttons seems to depend on
 *          which Window Manager is active:
 *
 *          On MATE (marco, a Metacity fork) both buttons are hidden.
 *          On KDE (KWin) the maximize button is still visible but inactive
 *          On OpenBox both min/max are visible with only minimize working
 *
 * \return  TRUE (avoids the key press getting passed to the emulated machine)
 */
gboolean ui_settings_dialog_create(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *content;
    char title[256];

    vsync_suspend_speed_eval();

    g_snprintf(title, 256, "%s Settings", machine_name);

    dialog = gtk_dialog_new_with_buttons(
            title,
            ui_get_active_window(),
            GTK_DIALOG_MODAL,
            "Undo changes in dialog", RESPONSE_RESET,
            "Restore factory settings", RESPONSE_FACTORY,
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_content_widget(dialog));

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    g_signal_connect(dialog, "response", G_CALLBACK(response_callback), NULL);
    g_signal_connect(dialog, "configure-event",
            G_CALLBACK(on_dialog_configure_event), NULL);

    settings_window = dialog;
    gtk_widget_show_all(dialog);

    return TRUE;
}


/** \brief  Clean up resources used on emu exit
 *
 * Do NOT call this when exiting the settings UI, the event handlers will take
 * care of cleaning up resources used by the UI. This function cleans up the
 * data used to present the user with the last used settings page.
 */
void ui_settings_shutdown(void)
{
    if (last_node_path != NULL) {
        gtk_tree_path_free(last_node_path);
    }
}
