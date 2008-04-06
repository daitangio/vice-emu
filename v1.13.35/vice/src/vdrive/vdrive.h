/*
 * vdrive.h - Virtual disk-drive implementation.
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

#ifndef _VDRIVE_H
#define _VDRIVE_H

#include "types.h"

/* High level disk formats.
   They can be different than the disk image type.  */
#define VDRIVE_IMAGE_FORMAT_1541 0
#define VDRIVE_IMAGE_FORMAT_1571 1
#define VDRIVE_IMAGE_FORMAT_1581 2
#define VDRIVE_IMAGE_FORMAT_8050 3
#define VDRIVE_IMAGE_FORMAT_8250 4
#define VDRIVE_IMAGE_FORMAT_2040 5

#define BUFFER_NOT_IN_USE      0
#define BUFFER_DIRECTORY_READ  1
#define BUFFER_SEQUENTIAL      2
#define BUFFER_MEMORY_BUFFER   3
#define BUFFER_RELATIVE        4
#define BUFFER_COMMAND_CHANNEL 5

#define WRITE_BLOCK 512

#define SET_LO_HI(p, val)             \
    do {                              \
        *((p)++) = (val) & 0xff;      \
        *((p)++) = ((val)>>8) & 0xff; \
    } while (0)

#define DRIVE_RAMSIZE           0x400

#define DIR_MAXBUF  (40 * 256)

/* File Types */
#define FT_DEL          0
#define FT_SEQ          1
#define FT_PRG          2
#define FT_USR          3
#define FT_REL          4
#define FT_CBM          5       /* 1581 partition */
#define FT_DJJ          6       /* 1581 */
#define FT_FAB          7       /* 1581 - Fred's format */
#define FT_REPLACEMENT  0x20
#define FT_LOCKED       0x40
#define FT_CLOSED       0x80

/* Access Control Methods */
#define FAM_READ        0
#define FAM_WRITE       1
#define FAM_APPEND      2
#define FAM_M           4
#define FAM_F           8

/* Serial Error Codes. */
#define SERIAL_OK               0
#define SERIAL_WRITE_TIMEOUT    1
#define SERIAL_READ_TIMEOUT     2
#define SERIAL_FILE_NOT_FOUND   64
#define SERIAL_NO_DEVICE        128

#define SERIAL_ERROR            (2)
#define SERIAL_EOF              (64)

typedef struct bufferinfo_s {
    unsigned int mode;     /* Mode on this buffer */
    unsigned int readmode; /* Is this channel for reading or writing */
    BYTE *buffer;          /* Use this to save data */
    BYTE *slot;            /* Save data for directory-slot */
    unsigned int bufptr;   /* Use this to save/read data to disk */
    unsigned int track;    /* which track is allocated for this sector */
    unsigned int sector;   /*   (for write files only) */
    unsigned int length;   /* Directory-read length */
    unsigned int record;   /* Current record */
} bufferinfo_t;

struct disk_image_s;

/* Run-time data struct for each drive. */
typedef struct vdrive_s {
    struct disk_image_s *image;

    /* Current image file */
    unsigned int mode;         /* Read/Write */
    unsigned int image_format; /* 1541/71/81 */
    unsigned int unit;

    unsigned int Bam_Track;
    unsigned int Bam_Sector;
    unsigned int bam_name;   /* Offset from start of BAM to disk name.   */
    unsigned int bam_id;     /* Offset from start of BAM to disk ID.  */
    unsigned int Dir_Track;
    unsigned int Dir_Sector;
    unsigned int num_tracks;

    /* FIXME: bam sizeof define */
    BYTE bam[5 * 256]; /* The 1581 uses 3 secs as BAM - but the 8250 uses 5.  */
    bufferinfo_t buffers[16];

    /* File information */
    BYTE Dir_buffer[256];  /* Current directory sector.  */
    unsigned int SlotNumber;

    const char *find_name; /* Current search pattern.  */
    int find_length;       /* -1 allowed.  */
    unsigned int find_type;

    unsigned int Curr_track;
    unsigned int Curr_sector;

    /* Files deleted with the `S' command.  */
    int deleted_files;

    /* Memory read command buffer.  */
    BYTE mem_buf[256];
    unsigned int mem_length;

    BYTE *side_sector;

    BYTE ram[0x800];
} vdrive_t;

/* Actually, serial-code errors ... */

#define FLOPPY_COMMAND_OK       0
#define FLOPPY_ERROR            (2)


/* Return values used around. */

#define FD_OK           0
#define FD_EXIT         1       /* -1,0, 1 are fixed values */

#define FD_NOTREADY     -2
#define FD_CHANGED      -3      /* File has changed on disk */
#define FD_NOTRD        -4
#define FD_NOTWRT       -5
#define FD_WRTERR       -6
#define FD_RDERR        -7
#define FD_INCOMP       -8      /* DOS Format Mismatch */
#define FD_BADIMAGE     -9      /* ID mismatch (Disk or tape) */
#define FD_BADNAME      -10     /* Illegal filename */
#define FD_BADVAL       -11     /* Illegal value */
#define FD_BADDEV       -12
#define FD_BAD_TS       -13     /* Track or sector */


#define CHK_NUM         0
#define CHK_RDY         1

/* ------------------------------------------------------------------------- */

extern void vdrive_init(void);
extern int vdrive_device_setup(vdrive_t *vdrive, unsigned int unit);
extern void vdrive_device_shutdown(vdrive_t *vdrive);
extern int vdrive_attach_image(struct disk_image_s *image, unsigned int unit,
                               vdrive_t *vdrive);
extern void vdrive_detach_image(struct disk_image_s *image, unsigned int unit,
                                vdrive_t *vdrive);
extern int vdrive_calc_num_blocks(unsigned int format, unsigned int tracks);
extern void vdrive_close_all_channels(vdrive_t *vdrive);
extern int vdrive_calculate_disk_half(unsigned int type);
extern int vdrive_get_max_sectors(unsigned int type, unsigned int track);
extern void vdrive_get_last_read(unsigned int *track, unsigned int *sector,
                                 BYTE **buffer);
extern void vdrive_set_last_read(unsigned int track, unsigned int sector,
                                 BYTE *buffer);
#endif

