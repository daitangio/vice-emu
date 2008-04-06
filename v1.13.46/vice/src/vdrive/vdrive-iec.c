/*
 * vdrive-iec.c - Virtual disk-drive IEC implementation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Olaf Seibert <rhialto@mbfys.kun.nl>
 *  Andr� Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
 *
 * Patches by
 *  Dan Miner <dminer@nyx10.cs.du.edu>
 *  Germano Caronni <caronni@tik.ethz.ch>
 *  Daniel Fandrich <dan@fch.wimsey.bc.ca>
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

/* #define DEBUG_DRIVE */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __riscos
#include "archdep.h"
#endif

#include "cbmdos.h"
#include "diskimage.h"
#include "lib.h"
#include "log.h"
#include "machine-bus.h"
#include "types.h"
#include "vdrive-bam.h"
#include "vdrive-command.h"
#include "vdrive-dir.h"
#include "vdrive-iec.h"
#include "vdrive-rel.h"
#include "vdrive.h"


static log_t vdrive_iec_log = LOG_ERR;


void vdrive_iec_init(void)
{
    vdrive_iec_log = log_open("VDriveIEC");
}

/* ------------------------------------------------------------------------- */

static int iec_open_read_sequential(vdrive_t *vdrive, unsigned int secondary,
                                     unsigned int track, unsigned int sector)
{
    int status;
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    p->mode = BUFFER_SEQUENTIAL;
    p->bufptr = 2;
    p->buffer = (BYTE *)lib_malloc(256);

    status = disk_image_read_sector(vdrive->image, p->buffer, track, sector);

    vdrive_set_last_read(track, sector, p->buffer);

    if (status != 0) {
        vdrive_iec_close(vdrive, secondary);
        return SERIAL_ERROR;
    }
    return SERIAL_OK;
}

static int iec_open_read(vdrive_t *vdrive, unsigned int secondary)
{
    int type;
    unsigned int track, sector;
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    BYTE *slot = p->slot;

    if (!slot) {
        vdrive_iec_close(vdrive, secondary);
        vdrive_command_set_error(vdrive, CBMDOS_IPE_NOT_FOUND, 0, 0);
        return SERIAL_ERROR;
    }

    type = slot[SLOT_TYPE_OFFSET] & 0x07;
    track = (unsigned int)slot[SLOT_FIRST_TRACK];
    sector = (unsigned int)slot[SLOT_FIRST_SECTOR];

    /* Del, Seq, Prg, Usr (Rel not supported here).  */
    if (type != CBMDOS_FT_REL)
        return iec_open_read_sequential(vdrive, secondary, track, sector);

    return SERIAL_ERROR;
}

static int iec_open_read_directory(vdrive_t *vdrive, unsigned int secondary,
                                   cbmdos_cmd_parse_t *cmd_parse)
{
    int retlen;
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    if (secondary > 0)
        return iec_open_read_sequential(vdrive, secondary, vdrive->Dir_Track,
                                        0);

    p->mode = BUFFER_DIRECTORY_READ;
    p->buffer = (BYTE *)lib_malloc(DIR_MAXBUF);

    retlen = vdrive_dir_create_directory(vdrive, cmd_parse->parsecmd,
                                         cmd_parse->parselength,
                                         cmd_parse->filetype, p->buffer);

    if (retlen < 0) {
        /* Directory not valid.  */
        p->mode = BUFFER_NOT_IN_USE;
        lib_free(p->buffer);
        p->length = 0;
        vdrive_command_set_error(vdrive, CBMDOS_IPE_NOT_FOUND, 0, 0);
        return SERIAL_ERROR;
    }

    p->length = (unsigned int)retlen;
    p->bufptr = 0;

    return SERIAL_OK;
}

static int iec_open_write(vdrive_t *vdrive, unsigned int secondary,
                          cbmdos_cmd_parse_t *cmd_parse, const char *name)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    BYTE *slot = p->slot;

    if (vdrive->image->read_only) {
        vdrive_command_set_error(vdrive, CBMDOS_IPE_WRITE_PROTECT_ON, 0, 0);
        return SERIAL_ERROR;
    }

    if (slot) {
        if (*name == '@')
            vdrive_dir_remove_slot(vdrive, slot);
        else {
            vdrive_iec_close(vdrive, secondary);
            vdrive_command_set_error(vdrive, CBMDOS_IPE_FILE_EXISTS, 0, 0);
            return SERIAL_ERROR;
        }
    }

    vdrive_dir_create_slot(p, cmd_parse->parsecmd, cmd_parse->parselength,
                           cmd_parse->filetype);

#if 0
    /* XXX keeping entry until close not implemented */
    /* Write the directory entry to disk as an UNCLOSED file. */

    vdrive_dir_find_first_slot(vdrive, NULL, -1, 0);
    e = vdrive_dir_find_next_slot(vdrive);

    if (!e) {
        p->mode = BUFFER_NOT_IN_USE;
        lib_free((char *)p->buffer);
        p->buffer = NULL;
        vdrive_command_set_error(vdrive, CBMDOS_IPE_DISK_FULL, 0, 0);
        status = SERIAL_ERROR;
        goto out;
    }
    memcpy(&vdrive->Dir_buffer[vdrive->SlotNumber * 32 + 2],
           p->slot + 2, 30);

    disk_image_write_sector(vdrive->image, vdrive->Dir_buffer,
    v                        vdrive->Curr_track, vdrive->Curr_sector);
    /*vdrive_bam_write_bam(vdrive);*/
#endif  /* BAM write */

    return SERIAL_OK;
}


/*
 * Open a file on the disk image, and store the information on the
 * directory slot.
 */

int vdrive_iec_open(vdrive_t *vdrive, const char *name, int length,
                    unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    BYTE *slot; /* Current directory entry */
    int rc, status = SERIAL_OK;
    cbmdos_cmd_parse_t cmd_parse;

    if ((!name || !*name) && p->mode != BUFFER_COMMAND_CHANNEL)  /* EP */
        return SERIAL_NO_DEVICE;        /* Routine was called incorrectly. */

   /* No floppy in drive?   */
   if (vdrive->image == NULL
       && p->mode != BUFFER_COMMAND_CHANNEL
       && secondary != 15
       && *name != '#') {
       vdrive_command_set_error(vdrive, CBMDOS_IPE_NOT_READY, 18, 0);
       log_message(vdrive_iec_log, "Drive not ready.");
       return SERIAL_ERROR;
   }

#ifdef DEBUG_DRIVE
    log_debug("VDRIVE#%i: OPEN: FD = %p - Name '%s' (%d) on ch %d.",
                  vdrive->unit, vdrive->image->fd, name, length, secondary);
#endif
#ifdef __riscos
    archdep_set_drive_leds(vdrive->unit - 8, 1);
#endif

    /*
     * If channel is command channel, name will be used as write. Return only
     * status of last write ...
     */
    if (p->mode == BUFFER_COMMAND_CHANNEL) {
        int n;

        for (n = 0; n < length; n++)
            status = vdrive_iec_write(vdrive, name[n], secondary);

        if (length)
            p->readmode = CBMDOS_FAM_WRITE;
        else
            p->readmode = CBMDOS_FAM_READ;
        return status;
    }

    /*
     * Clear error flag
     */
    vdrive_command_set_error(vdrive, CBMDOS_IPE_OK, 0, 0);

    /*
     * In use ?
     */
    if (p->mode != BUFFER_NOT_IN_USE) {
#ifdef DEBUG_DRIVE
        log_debug("Cannot open channel %d. Mode is %d.", secondary, p->mode);
#endif
        vdrive_command_set_error(vdrive, CBMDOS_IPE_NO_CHANNEL, 0, 0);
        return SERIAL_ERROR;
    }

    cmd_parse.cmd = name;
    cmd_parse.cmdlength = length;
    cmd_parse.secondary = secondary;

    rc = cbmdos_command_parse(&cmd_parse);

    if (rc != CBMDOS_IPE_OK) {
        status = SERIAL_ERROR;
        goto out;
    }

#ifdef DEBUG_DRIVE
    log_debug("Raw file name: `%s', length: %i.", name, length);
    log_debug("Parsed file name: `%s', reallength: %i.",
              cmd_parse.parsecmd, cmd_parse.parselength);
#endif

    /* Limit file name to 16 chars.  */
    if (cmd_parse.parselength > 16)
        cmd_parse.parselength = 16;

    /*
     * Internal buffer ?
     */
    if (*name == '#') {
        p->mode = BUFFER_MEMORY_BUFFER;
        p->buffer = (BYTE *)lib_malloc(256);
        p->bufptr = 0;
        status = SERIAL_OK;
        goto out;
    }

    /*
     * Directory read
     * A little-known feature of the 1541: open 1,8,2,"$" (or even 1,8,1).
     * It gives you the BAM+DIR as a sequential file, containing the data
     * just as it appears on disk.  -Olaf Seibert
     */

    if (*name == '$') {
        status = iec_open_read_directory(vdrive, secondary, &cmd_parse);
        goto out;
    }

    /*
     * Check that there is room on directory.
     */
    vdrive_dir_find_first_slot(vdrive, cmd_parse.parsecmd,
                               cmd_parse.parselength, 0);

    /*
     * Find the first non-DEL entry in the directory (if it exists).
     */
    do
        slot = vdrive_dir_find_next_slot(vdrive);
    while (slot && ((slot[SLOT_TYPE_OFFSET] & 0x07) == CBMDOS_FT_DEL));

    p->readmode = cmd_parse.readmode;
    p->slot = slot;

    if (cmd_parse.filetype == CBMDOS_FT_REL) {
        status = vdrive_rel_open(vdrive, secondary, cmd_parse.recordlength);
        goto out;
    }

    if (cmd_parse.readmode == CBMDOS_FAM_READ)
        status = iec_open_read(vdrive, secondary);
    else
        status = iec_open_write(vdrive, secondary, &cmd_parse, name);

out:
    lib_free(cmd_parse.parsecmd);
    return status;
}

/* ------------------------------------------------------------------------- */

static int iec_write_sequential(vdrive_t *vdrive, bufferinfo_t *bi, int length)
{
    unsigned int t_new, s_new;
    int retval;
    BYTE *buf = bi->buffer;
    BYTE *slot = bi->slot;

    /*
     * First block of a file ?
     */
    if (slot[SLOT_FIRST_TRACK] == 0) {
        retval = vdrive_bam_alloc_first_free_sector(vdrive, vdrive->bam, &t_new,                                                    &s_new);
        if (retval < 0) {
            vdrive_command_set_error(vdrive, CBMDOS_IPE_DISK_FULL, 0, 0);
            return -1;
        }
        slot[SLOT_FIRST_TRACK] = bi->track = t_new;
        slot[SLOT_FIRST_SECTOR] = bi->sector = s_new;
    }

    if (length == WRITE_BLOCK) {
        /*
         * Write current sector and allocate next
         */
        t_new = bi->track;
        s_new = bi->sector;
        retval = vdrive_bam_alloc_next_free_sector(vdrive, vdrive->bam, &t_new,
                                                   &s_new);
        if (retval < 0) {
            vdrive_command_set_error(vdrive, CBMDOS_IPE_DISK_FULL, 0, 0);
            return -1;
        }
        buf[0] = t_new;
        buf[1] = s_new;

        disk_image_write_sector(vdrive->image, buf, bi->track, bi->sector);

        bi->track = t_new;
        bi->sector = s_new;
    } else {
        /*
         * Write last block
         */
        buf[0] = 0;
        buf[1] = length - 1;

        disk_image_write_sector(vdrive->image, buf, bi->track, bi->sector);
    }

    if (!(++slot[SLOT_NR_BLOCKS]))
        ++slot[SLOT_NR_BLOCKS + 1];

    return 0;
}

static int iec_close_sequential(vdrive_t *vdrive, unsigned int secondary)
{
    BYTE *slot;
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    if (p->readmode & (CBMDOS_FAM_WRITE | CBMDOS_FAM_APPEND)) {
        /*
         * Flush bytes and write slot to directory
         */

        if (vdrive->image->read_only) {
            vdrive_command_set_error(vdrive, CBMDOS_IPE_WRITE_PROTECT_ON, 0, 0);
            return SERIAL_ERROR;
        }

#ifdef DEBUG_DRIVE
        log_debug("DEBUG: flush.");
#endif
        iec_write_sequential(vdrive, p, p->bufptr);

#ifdef DEBUG_DRIVE
        log_debug("DEBUG: find empty DIR slot.");
#endif
        vdrive_dir_find_first_slot(vdrive, NULL, -1, 0);
        slot = vdrive_dir_find_next_slot(vdrive);

        if (!slot) {
            p->mode = BUFFER_NOT_IN_USE;
            lib_free((char *)p->buffer);
            p->buffer = NULL;

            vdrive_command_set_error(vdrive, CBMDOS_IPE_DISK_FULL, 0, 0);
            return SERIAL_ERROR;
        }
        p->slot[SLOT_TYPE_OFFSET] |= 0x80; /* Closed */

        memcpy(&vdrive->Dir_buffer[vdrive->SlotNumber * 32 + 2],
               p->slot + 2, 30);

#ifdef DEBUG_DRIVE
        log_debug("DEBUG: closing, write DIR slot (%d %d) and BAM.",
                  vdrive->Curr_track, vdrive->Curr_sector);
#endif
        disk_image_write_sector(vdrive->image, vdrive->Dir_buffer,
                                vdrive->Curr_track, vdrive->Curr_sector);
        vdrive_bam_write_bam(vdrive);
    }
    p->mode = BUFFER_NOT_IN_USE;
    lib_free((char *)p->buffer);
    p->buffer = NULL;

    return SERIAL_OK;
}

int vdrive_iec_close(vdrive_t *vdrive, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    int status = SERIAL_OK;

#ifdef __riscos
    archdep_set_drive_leds(vdrive->unit - 8, 0);
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        return SERIAL_OK; /* FIXME: Is this correct? */

      case BUFFER_MEMORY_BUFFER:
      case BUFFER_DIRECTORY_READ:
        lib_free((char *)p->buffer);
        p->mode = BUFFER_NOT_IN_USE;
        p->buffer = NULL;
        p->slot = NULL;
        break;
      case BUFFER_SEQUENTIAL:
        status = iec_close_sequential(vdrive, secondary);
        break;
      case BUFFER_RELATIVE:
        /* FIXME !!! */
        p->mode = BUFFER_NOT_IN_USE;
        lib_free((char *)p->buffer);
        p->buffer = NULL;
        break;
      case BUFFER_COMMAND_CHANNEL:
        /* I'm not sure if this is correct, but really closing the buffer
           should reset the read pointer to the beginning for the next
           write! */
        vdrive_command_set_error(vdrive, CBMDOS_IPE_OK, 0, 0);
        vdrive_close_all_channels(vdrive);
        break;
      default:
        log_error(vdrive_iec_log, "Fatal: unknown floppy-close-mode: %i.",
                  p->mode);
    }

    return status;
}

/* ------------------------------------------------------------------------- */

static int iec_read_sequential(vdrive_t *vdrive, BYTE *data,
                               unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    if (p->readmode != CBMDOS_FAM_READ)
        return SERIAL_ERROR;

    /*
     * Read next block if needed
     */
    if (p->buffer[0]) {
        if (p->bufptr >= 256) {
            int status;
            unsigned int track, sector;

            track = (unsigned int)p->buffer[0];
            sector = (unsigned int)p->buffer[1];

            status = disk_image_read_sector(vdrive->image, p->buffer,
                                                track, sector);
            vdrive_set_last_read(track, sector, p->buffer);

            if (status == 0) {
                p->bufptr = 2;
            } else {
                *data = 0xc7;
                return SERIAL_EOF;
            }
        }
    } else {
        if (p->bufptr > p->buffer[1]) {
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data)
                          ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
    }

    *data = p->buffer[p->bufptr];
    p->bufptr++;

    return SERIAL_OK;
}

int vdrive_iec_read(vdrive_t *vdrive, BYTE *data, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    int status = SERIAL_OK;

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        vdrive_command_set_error(vdrive, CBMDOS_IPE_NOT_OPEN, 0, 0);
        return SERIAL_ERROR;

      case BUFFER_DIRECTORY_READ:
        if (p->bufptr >= p->length) {
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;

      case BUFFER_MEMORY_BUFFER:
        if (p->bufptr >= 256) {
            *data = 0xc7;
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;

      case BUFFER_SEQUENTIAL:
        status = iec_read_sequential(vdrive, data, secondary);
        break;
      case BUFFER_COMMAND_CHANNEL:
        if (p->bufptr > p->length) {
            vdrive_command_set_error(vdrive, CBMDOS_IPE_OK, 0, 0);
#ifdef DEBUG_DRIVE
            log_debug("End of buffer in command channel.");
#endif
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;
      case BUFFER_RELATIVE:
        if (p->bufptr > p->length) {
            vdrive_command_set_error(vdrive, CBMDOS_IPE_OK, 0, 0);
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;

      default:
        log_error(vdrive_iec_log, "Fatal: unknown buffermode on floppy-read.");
    }

#ifdef DEBUG_DRIVE
    if (p->mode == BUFFER_COMMAND_CHANNEL)
        log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                  p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
    return status;
}

/* ------------------------------------------------------------------------- */

int vdrive_iec_write(vdrive_t *vdrive, BYTE data, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    if (vdrive->image->read_only && p->mode != BUFFER_COMMAND_CHANNEL) {
        vdrive_command_set_error(vdrive, CBMDOS_IPE_WRITE_PROTECT_ON, 0, 0);
        return SERIAL_ERROR;
    }

#ifdef DEBUG_DRIVE
    if (p -> mode == BUFFER_COMMAND_CHANNEL)
        log_debug("Disk write %d [%02d %02d] data %02x (%c).",
                  p->mode, 0, 0, data, (isprint(data) ? data : '.') );
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        vdrive_command_set_error(vdrive, CBMDOS_IPE_NOT_OPEN, 0, 0);
        return SERIAL_ERROR;
      case BUFFER_DIRECTORY_READ:
        vdrive_command_set_error(vdrive, CBMDOS_IPE_NOT_WRITE, 0, 0);
        return SERIAL_ERROR;
      case BUFFER_MEMORY_BUFFER:
        if (p->bufptr >= 256)
            return SERIAL_ERROR;
        p->buffer[p->bufptr] = data;
        p->bufptr++;
        return SERIAL_OK;
      case BUFFER_SEQUENTIAL:
        if (p->readmode == CBMDOS_FAM_READ)
            return SERIAL_ERROR;

        if (p->bufptr >= 256) {
            p->bufptr = 2;
            if (iec_write_sequential(vdrive, p, WRITE_BLOCK) < 0)
                return SERIAL_ERROR;
        }
        p->buffer[p->bufptr] = data;
        p->bufptr++;
        break;
      case BUFFER_COMMAND_CHANNEL:
        if (p->readmode == CBMDOS_FAM_READ) {
            p->bufptr = 0;
            p->readmode = CBMDOS_FAM_WRITE;
        }
        if (p->bufptr >= 256) /* Limits checked later */
            return SERIAL_ERROR;
        p->buffer[p->bufptr] = data;
        p->bufptr++;
        break;
      default:
        log_error(vdrive_iec_log, "Fatal: Unknown write mode.");
        exit(-1);
    }
    return SERIAL_OK;
}

/* ------------------------------------------------------------------------- */

void vdrive_iec_flush(vdrive_t *vdrive, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    int status;

#ifdef DEBUG_DRIVE
       log_debug("FLUSH:, secondary = %d, buffer=%s\n "
                 "  bufptr=%d, length=%d, read?=%d.", secondary, p->buffer,
                 p->bufptr, p->length, p->readmode == CBMDOS_FAM_READ);
#endif

    if (p->mode != BUFFER_COMMAND_CHANNEL)
        return;

#ifdef DEBUG_DRIVE
       log_debug("FLUSH: COMMAND CHANNEL");
#endif

    if (p->readmode == CBMDOS_FAM_READ)
        return;

#ifdef DEBUG_DRIVE
       log_debug("FLUSH: READ MODE");
#endif

    if (p->length) {
        /* If no command, do nothing - keep error code.  */
        status = vdrive_command_execute(vdrive, p->buffer, p->bufptr);
        p->bufptr = 0;
    }
}

/* ------------------------------------------------------------------------- */

int vdrive_iec_attach(unsigned int unit, const char *name)
{
    return machine_bus_device_attach(unit, name, vdrive_iec_read,
                                     vdrive_iec_write, vdrive_iec_open,
                                     vdrive_iec_close, vdrive_iec_flush);
}

