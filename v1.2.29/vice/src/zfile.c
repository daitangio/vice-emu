/*
 * zfile.c - Transparent handling of compressed files.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *
 * ARCHIVE, ZIPCODE and LYNX supports added by
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

/* This code might be improved a lot...  */

#include "vice.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "archdep.h"
#include "log.h"
#include "utils.h"
#include "zfile.h"
#include "zipcode.h"


#ifdef NAME_NEED_QM
#define MAKE_NAME(a,n) {\
    char *exp; \
    archdep_expand_path(&exp, n);\
    a = concat("\"", exp, "\"", NULL); \
    free(exp); \
    }
#else
#define MAKE_NAME(a,n) a=stralloc(n)
#endif

/* ------------------------------------------------------------------------- */

/* #define DEBUG_ZFILE */

#ifdef DEBUG_ZFILE
#define ZDEBUG(a)  log_debug a
#else
#define ZDEBUG(a)
#endif

/* We could add more here...  */
enum compression_type {
    COMPR_NONE,
    COMPR_GZIP,
    COMPR_BZIP,
    COMPR_ARCHIVE,
    COMPR_ZIPCODE,
    COMPR_LYNX,
    COMPR_TZX
};

/* This defines a linked list of all the compressed files that have been
   opened.  */
struct zfile {
    char *tmp_name;		/* Name of the temporary file.  */
    char *orig_name;		/* Name of the original file.  */
    int write_mode;		/* Non-zero if the file is open for writing.  */
    FILE *stream;		/* Associated stdio-style stream.  */
    FILE *fd;			/* Associated file descriptor.  */
    enum compression_type type;	/* Compression algorithm.  */
    struct zfile *prev, *next;  /* Link to the previous and next nodes.  */
    zfile_action_t action;	/* action on close */
    char *request_string;	/* ui string for action=ZFILE_REQUEST */
};
struct zfile *zfile_list = NULL;

static log_t zlog = LOG_ERR;

/* ------------------------------------------------------------------------- */

static int zinit_done = 0;

static int zinit(void)
{
    struct zfile *p;

    zlog = log_open("ZFile");

    /* Free the `zfile_list' if not empty.  */
    for (p = zfile_list; p != NULL; ) {
	struct zfile *next;

	free(p->orig_name);
	free(p->tmp_name);
	next = p->next;
	free(p);
	p = next;
    }

    zfile_list = NULL;
    zinit_done = 1;

    return 0;
}

/* Add one zfile to the list.  `orig_name' is automatically expanded to the
   complete path.  */
static void zfile_list_add(const char *tmp_name,
			   const char *orig_name,
			   enum compression_type type,
			   int write_mode,
			   FILE *stream, FILE *fd)
{
    struct zfile *new_zfile = (struct zfile *)xmalloc(sizeof(struct zfile));

    /* Make sure we have the complete path of the file.  */
    archdep_expand_path(&new_zfile->orig_name, orig_name);

    /* The new zfile becomes first on the list.  */
    new_zfile->tmp_name = tmp_name ? stralloc(tmp_name) : NULL;
    new_zfile->write_mode = write_mode;
    new_zfile->stream = stream;
    new_zfile->fd = fd;
    new_zfile->type = type;
    new_zfile->action = ZFILE_KEEP;
    new_zfile->request_string = NULL;
    new_zfile->next = zfile_list;
    new_zfile->prev = NULL;
    if (zfile_list != NULL)
	zfile_list->prev = new_zfile;
    zfile_list = new_zfile;
}

/* ------------------------------------------------------------------------ */

/* Uncompression.  */

/* If `name' has a gzip-like extension, try to uncompress it into a temporary
   file using gzip.  If this succeeds, return the name of the temporary file;
   return NULL otherwise.  */
static char *try_uncompress_with_gzip(const char *name)
{
#ifdef __riscos
    return NULL;
#else
    static char tmp_name[L_tmpnam];
    int l = strlen(name);
    int exit_status;
    char *argv[4];

    /* Check whether the name sounds like a gzipped file by checking the
       extension.  The last case (3-character extensions whose last character
       is a `z' (or 'Z'), is the standard convention for the MS-DOS version
       of gzip.  */
    if ((l < 4 || strcasecmp(name + l - 3, ".gz"))
	&& (l < 3 || strcasecmp(name + l - 2, ".z"))
	&& (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '.'))
	return NULL;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("gzip");
    argv[1] = stralloc("-cd");
    MAKE_NAME(argv[2], name);
    argv[3] = NULL;

    ZDEBUG(("try_uncompress_with_gzip: spawning gzip -cd %s", name));
    exit_status = archdep_spawn("gzip", argv, tmpnam(tmp_name), NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("try_uncompress_with_gzip: OK"));
	return tmp_name;
    } else {
	ZDEBUG(("try_uncompress_with_gzip: failed"));
	remove_file(tmp_name);
	return NULL;
    }
#endif
}

/* If `name' has a bzip-like extension, try to uncompress it into a temporary
   file using bzip.  If this succeeds, return the name of the temporary file;
   return NULL otherwise.  */
static char *try_uncompress_with_bzip(const char *name)
{
    static char tmp_name[L_tmpnam];
    int l = strlen(name);
    int exit_status;
    char *argv[4];

    /* Check whether the name sounds like a bzipped file by checking the
       extension.  MSDOS and UNIX variants of bzip v2 use the extension
       '.bz2'.  bzip v1 is obsolete.  */
    if (l < 5 || strcasecmp(name + l - 4, ".bz2") != 0)
	return NULL;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("bzip2");
    argv[1] = stralloc("-cd");
    MAKE_NAME(argv[2], name);
    argv[3] = NULL;

    ZDEBUG(("try_uncompress_with_bzip: spawning bzip -cd %s", name));
    exit_status = archdep_spawn("bzip2", argv, tmpnam(tmp_name), NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {

  	ZDEBUG(("try_uncompress_with_bzip: OK"));
	return tmp_name;
    } else {
	ZDEBUG(("try_uncompress_with_bzip: failed"));
	remove_file(tmp_name);
	return NULL;
    }
}

static char *try_uncompress_with_tzx(const char *name)
{
#ifdef __riscos
    return NULL;
#else
    static char tmp_name[L_tmpnam];
    int l = strlen(name);
    int exit_status;
    char *argv[4];

    /* Check whether the name sounds like a tzx file. */
    if (l < 4 || strcasecmp(name + l - 4, ".tzx") != 0)
        return NULL;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("64tzxtap");
    MAKE_NAME(argv[1], name);
    argv[2] = NULL;

    ZDEBUG(("try_uncompress_with_tzx: spawning 64tzxtap %s", name));
    exit_status = archdep_spawn("64tzxtap", argv, tmpnam(tmp_name), NULL);

    free(argv[0]);
    free(argv[1]);

    if (exit_status == 0) {
        ZDEBUG(("try_uncompress_with_tzx: OK"));
        return tmp_name;
    } else {
        ZDEBUG(("try_uncompress_with_tzx: failed"));
        remove_file(tmp_name);
        return NULL;
    }
#endif
}

/* is the name zipcode -name? */
static int is_zipcode_name(char *name)
{
    if (name[0] >= '1' && name[0] <= '4' && name[1] == '!')
	return 1;
    return 0;
}

/* Extensions we know about */
static const char *extensions[] = {
    FSDEV_EXT_SEP_STR "d64",
    FSDEV_EXT_SEP_STR "d71",
    FSDEV_EXT_SEP_STR "d80",
    FSDEV_EXT_SEP_STR "d81",
    FSDEV_EXT_SEP_STR "d82",
    FSDEV_EXT_SEP_STR "g64",
    FSDEV_EXT_SEP_STR "g41",
    FSDEV_EXT_SEP_STR "x64",
    FSDEV_EXT_SEP_STR "dsk",
    FSDEV_EXT_SEP_STR "t64",
    FSDEV_EXT_SEP_STR "p00",
    FSDEV_EXT_SEP_STR "prg",
    FSDEV_EXT_SEP_STR "lnx",
    FSDEV_EXT_SEP_STR "tap",
    NULL
};

static int is_valid_extension(char *end, int l, int nameoffset)
{
    int				i, len;
    /* zipcode testing is a special case */
    if (l > nameoffset + 2 && is_zipcode_name(end + nameoffset))
	return 1;
    /* others */
    for (i = 0; extensions[i]; i++)
    {
	len = strlen(extensions[i]);
	if (l < nameoffset + len)
	    continue;
	if (!strcasecmp(extensions[i], end + l - len))
	    return 1;
    }
    return 0;
}


/* If `name' has a correct extension, try to list its contents and search for
   the first file with a proper extension; if found, extract it.  If this
   succeeds, return the name of the temporary file; if the archive file is
   valid but `write_mode' is non-zero, return a zero-length string; in all
   the other cases, return NULL.  */
static const char *try_uncompress_archive(const char *name, int write_mode,
                                          const char *program,
                                          const char *listopts,
                                          const char *extractopts,
                                          const char *extension,
                                          const char *search)
{
#ifdef __riscos
    return NULL;
#else
    static char tmp_name[L_tmpnam];
    int l = strlen(name), nameoffset, found = 0, len;
    int exit_status;
    char *argv[8];
    FILE *fd;
    char tmp[1024];

    /* Do we have correct extension?  */
    len = strlen(extension);
    if (l <= len || strcasecmp(name + l - len, extension) != 0)
	return NULL;

    /* First run listing and search for first recognizeable extension.  */
    argv[0] = stralloc(program);
    argv[1] = stralloc(listopts);
    MAKE_NAME(argv[2], name);
    argv[3] = NULL;

    ZDEBUG(("try_uncompress_archive: spawning `%s %s %s'",
	    program, listopts, name));
    exit_status = archdep_spawn(program, argv, tmpnam(tmp_name), NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    /* No luck?  */
    if (exit_status != 0) {
	ZDEBUG(("try_uncompress_archive: `%s %s' failed.", program,
		listopts));
	remove_file(tmp_name);
	return NULL;
    }

    ZDEBUG(("try_uncompress_archive: `%s %s' successful.", program,
	    listopts));

    fd = fopen(tmp_name, MODE_READ);
    if (!fd) {
	ZDEBUG(("try_uncompress_archive: cannot read `%s %s' output.",
		program, tmp_name));
	remove_file(tmp_name);
	return NULL;
    }

    ZDEBUG(("try_uncompress_archive: searching for the first valid file."));

    /* Search for `search' first (if any) to see the offset where
       filename begins, then search for first recognizeable file.  */
    nameoffset = search ? -1 : 0;
    len = search ? strlen(search) : 0;
    while (!feof(fd)) {
	fgets(tmp, 1024, fd);
	l = strlen(tmp);
	while (l > 0 && (tmp[l-1] == '\n' || tmp[l-1] == '\r'))
	    tmp[--l] = 0;
	if (nameoffset < 0 && l >= len &&
	    !strcasecmp(tmp + l - len, search) != 0) {
	    nameoffset = l - 4;
	}
	if (nameoffset >= 0 && is_valid_extension(tmp, l, nameoffset)) {
	    ZDEBUG(("try_uncompress_archive: found `%s'.",
		    tmp + nameoffset));
	    found = 1;
	    break;
	}
    }

    fclose(fd);
    remove_file(tmp_name);
    if (!found) {
	ZDEBUG(("try_uncompress_archive: no valid file found."));
	return NULL;
    }

    /* This would be a valid ZIP file, but we cannot handle ZIP files in
       write mode.  Return a null temporary file name to report this.  */
    if (write_mode) {
	ZDEBUG(("try_uncompress_archive: cannot open file in write mode."));
	return "";
    }

    /* And then file inside zip.  If we have a zipcode extract all of them
       to the same file. */
    argv[0] = stralloc(program);
    argv[1] = stralloc(extractopts);
    argv[2] = stralloc(name);
    if (is_zipcode_name(tmp + nameoffset)) {
	argv[3] = stralloc(tmp + nameoffset);
	argv[4] = stralloc(tmp + nameoffset);
	argv[5] = stralloc(tmp + nameoffset);
	argv[6] = stralloc(tmp + nameoffset);
	argv[7] = NULL;
	argv[3][0] = '1';
	argv[4][0] = '2';
	argv[5][0] = '3';
	argv[6][0] = '4';
    } else {
	argv[3] = stralloc(tmp + nameoffset);
	argv[4] = NULL;
    }

    ZDEBUG(("try_uncompress_archive: spawning `%s %s %s %s'.",
	    program, extractopts, name, tmp + nameoffset));
    exit_status = archdep_spawn(program, argv, tmp_name, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    if (is_zipcode_name(tmp + nameoffset)) {
        free(argv[4]);
        free(argv[5]);
        free(argv[6]);
    }

    if (exit_status != 0) {
	ZDEBUG(("try_uncompress_archive: `%s %s' failed.",
		program, extractopts));
	remove_file(tmp_name);
	return NULL;
    }

    ZDEBUG(("try_uncompress_archive: `%s %s' successful.", program,
	    tmp_name));
    return tmp_name;
#endif
}

#ifdef __riscos
#define C1541_NAME     "Vice:c1541"
#else
#define C1541_NAME     "c1541"
#endif

/* If this file looks like a zipcode, try to extract is using c1541. We have
   to figure this out by reading the contents of the file */
static const char *try_uncompress_zipcode(const char *name, int write_mode)
{
    static char tmp_name[L_tmpnam];
    int i, count, sector, sectors = 0;
    FILE *fd;
    char tmp[256];
    char *argv[5];
    int	exit_status;

    /* The 2nd char has to be '!'? */
    if (strlen(name) < 3 || name[1] != '!')
        return NULL;

    /* can we read this file? */
    fd = fopen(name, MODE_READ);
    if (fd == NULL)
        return NULL;
    /* Read first track to see if this is zipcode */
    fseek(fd, 4, SEEK_SET);
    for (count = 1; count < 21; count++) {
        i = zipcode_read_sector(fd, 1, &sector, tmp);
        if (i || sector < 0 || sector > 20 || (sectors & (1 << sector))) {
            fclose(fd);
            return NULL;
        }
        sectors |= 1 << sector;
    }
    fclose(fd);

    /* it is a zipcode. We cannot support write_mode */
    if (write_mode)
        return "";

    /* format image first */
    tmpnam(tmp_name);

    /* ok, now extract the zipcode */
    argv[0] = stralloc(C1541_NAME);
    argv[1] = stralloc("-zcreate");
    argv[2] = stralloc(tmp_name);
    MAKE_NAME(argv[3], name);
    argv[4] = NULL;

    exit_status = archdep_spawn(C1541_NAME, argv, NULL, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);

    if (exit_status) {
        remove_file(tmp_name);
        return NULL;
    }
    /* everything ok */
    return tmp_name;
}

/* If the file looks like a lynx image, try to extract it using c1541. We have
   to figure this out by reading the contsnts of the file */
static const char *try_uncompress_lynx(const char *name, int write_mode)
{
    static char tmp_name[L_tmpnam];
    int i, count;
    FILE *fd;
    char tmp[256];
    char *argv[20];
    int exit_status;

    /* can we read this file? */
    fd = fopen(name, MODE_READ);
    if (fd == NULL)
        return NULL;
    /* is this lynx -image? */
    i = fread(tmp, 1, 2, fd);
    if (i != 2 || tmp[0] != 1 || tmp[1] != 8) {
	fclose(fd);
	return NULL;
    }
    count = 0;
    while (1) {
	i = fread(tmp, 1, 1, fd);
	if (i != 1) {
	    fclose(fd);
	    return NULL;
	}
	if (tmp[0])
	    count = 0;
	else
	    count++;
	if (count == 3)
	    break;
    }
    i = fread(tmp, 1, 1, fd);
    if (i != 1 || tmp[0] != 13) {
	fclose(fd);
	return NULL;
    }
    count = 0;
    while (1) {
	i = fread(&tmp[count], 1, 1, fd);
	if (i != 1 || count == 254) {
	    fclose(fd);
	    return NULL;
	}
	if (tmp[count++] == 13)
	    break;
    }
    tmp[count] = 0;
    if (!atoi(tmp)) {
        fclose(fd);
        return NULL;
    }
    /* XXX: this is not a full check, but perhaps enough? */

    fclose(fd);

    /* it is a lynx image. We cannot support write_mode */
    if (write_mode)
	return "";

    tmpnam(tmp_name);

    /* now create the image */
    argv[0] = stralloc("c1541");
    argv[1] = stralloc("-format");
    argv[2] = stralloc("lynximage,00");
    argv[3] = stralloc("x64");
    argv[4] = stralloc(tmp_name);
    argv[5] = stralloc("-unlynx");
    MAKE_NAME(argv[6], name);
    argv[7] = NULL;

    exit_status = archdep_spawn("c1541", argv, NULL, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
    free(argv[5]);
    free(argv[6]);

    if (exit_status) {
	remove_file(tmp_name);
	return NULL;
    }
    /* everything ok */
    return tmp_name;
}

/* List of archives we understand.  */
static struct {
    const char	*program;
    const char	*listopts;
    const char	*extractopts;
    const char	*extension;
    const char	*search;
} valid_archives[] = {
#if (!defined(__MSDOS__) && !defined(__riscos))
    { "unzip",   "-l",   "-p",    ".zip",    "Name" },
    { "lha",     "lv",   "pq",    ".lzh",    NULL },
    { "lha",     "lv",   "pq",    ".lha",    NULL },
    /* Hmmm.  Did non-gnu tar have a -O -option?  */
    { "gtar",    "-tf",  "-xOf",  ".tar",    NULL },
    { "tar",     "-tf",  "-xOf",  ".tar",    NULL },
    { "gtar",    "-ztf", "-zxOf", ".tar.gz", NULL },
    { "tar",     "-ztf", "-zxOf", ".tar.gz", NULL },
    { "gtar",    "-ztf", "-zxOf", ".tgz",    NULL },
    { "tar",     "-ztf", "-zxOf", ".tgz",    NULL },
    /* this might be overkill, but adding this was sooo easy...  */
    { "zoo",     "lf1q", "xpq",	  ".zoo",    NULL },
#endif
#if defined __MSDOS__ || defined WIN32
    { "pkunzip", "-v",   " ",     ".zip",    "Name" },
#endif
    { NULL }
};


/* Try to uncompress file `name' using the algorithms we now of.  If this is
   not possible, return `COMPR_NONE'.  Otherwise, uncompress the file into a
   temporary file, return the type of algorithm used and the name of the
   temporary file in `tmp_name'.  If `write_mode' is non-zero and the
   returned `tmp_name' has zero length, then the file cannot be accessed in
   write mode.  */
static enum compression_type try_uncompress(const char *name,
                                            const char **tmp_name,
                                            int write_mode)
{
    int i;

    for (i = 0; valid_archives[i].program; i++) {
	if ((*tmp_name = try_uncompress_archive(name, write_mode,
						valid_archives[i].program,
						valid_archives[i].listopts,
						valid_archives[i].extractopts,
						valid_archives[i].extension,
						valid_archives[i].search))
	    != NULL) {
	    return COMPR_ARCHIVE;
	}
    }

    /* need this order or .tar.gz is misunderstood */
    if ((*tmp_name = try_uncompress_with_gzip(name)) != NULL)
        return COMPR_GZIP;

    if ((*tmp_name = try_uncompress_with_bzip(name)) != NULL)
        return COMPR_BZIP;

    if ((*tmp_name = try_uncompress_zipcode(name, write_mode)) != NULL)
        return COMPR_ZIPCODE;

    if ((*tmp_name = try_uncompress_lynx(name, write_mode)) != NULL)
        return COMPR_LYNX;

    if ((*tmp_name = try_uncompress_with_tzx(name)) != NULL)
        return COMPR_TZX;

    return COMPR_NONE;
}

/* ------------------------------------------------------------------------- */

/* Compression.  */

/* Compress `src' into `dest' using gzip.  */
static int compress_with_gzip(const char *src, const char *dest)
{
    static char *argv[4];
    int exit_status;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("gzip");
    argv[1] = stralloc("-c");
    argv[2] = stralloc(src);
    argv[3] = NULL;

    ZDEBUG(("compress_with_gzip: spawning gzip -c %s", src));
    exit_status = archdep_spawn("gzip", argv, dest, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("compress_with_gzip: OK."));
	return 0;
    } else {
	ZDEBUG(("compress_with_gzip: failed."));
	return -1;
    }
}

/* Compress `src' into `dest' using bzip.  */
static int compress_with_bzip(const char *src, const char *dest)
{
    static char *argv[4];
    int exit_status;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("bzip2");
    argv[1] = stralloc("-c");
    argv[2] = stralloc(src);
    argv[3] = NULL;

    ZDEBUG(("compress_with_bzip: spawning bzip -c %s", src));
    exit_status = archdep_spawn("bzip2", argv, dest, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("compress_with_bzip: OK."));
	return 0;
    } else {
	ZDEBUG(("compress_with_bzip: failed."));
	return -1;
    }
}

/* Compress `src' into `dest' using algorithm `type'.  */
static int compress(const char *src, const char *dest,
		    enum compression_type type)
{
    char *dest_backup_name;
    int retval;

    /* This shouldn't happen */
    if (type == COMPR_ARCHIVE) {
        log_error(zlog, "compress: trying to compress archive-file.");
        return -1;
    }

    /* This shouldn't happen */
    if (type == COMPR_ZIPCODE) {
        log_error(zlog, "compress: trying to compress zipcode-file.");
        return -1;
    }

    /* This shouldn't happen */
    if (type == COMPR_LYNX) {
        log_error(zlog, "compress: trying to compress lynx-file.");
        return -1;
    }

    /* This shouldn't happen */
    if (type == COMPR_TZX) {
        log_error(zlog, "compress: trying to compress tzx-file.");
        return -1;
    }

    /* Check whether `compression_type' is a known one.  */
    if (type != COMPR_GZIP && type != COMPR_BZIP) {
        log_error(zlog, "compress: unknown compression type");
        return -1;
    }

    /* If we have no write permissions for `dest', give up.  */
    if (access(dest, W_OK) < 0) {
        ZDEBUG(("compress: no write permissions for `%s'",
                dest));
        return -1;
    }

    if (access(dest, R_OK) < 0) {
        ZDEBUG(("compress: no read permissions for `%s'", dest));
        dest_backup_name = NULL;
    } else {
	/* If `dest' exists, make a backup first.  */
	dest_backup_name = archdep_make_backup_filename(dest);
	if (dest_backup_name != NULL)
	    ZDEBUG(("compress: making backup %s... ", dest_backup_name));
#ifdef WIN32
    if (dest_backup_name != NULL)
        remove_file(dest_backup_name);
#endif
	if (dest_backup_name != NULL && rename(dest, dest_backup_name) < 0) {
	    ZDEBUG(("failed."));
	    log_error(zlog, "Could not make pre-compression backup.");
	    return -1;
	} else {
	    ZDEBUG(("OK."));
	}
    }

    switch (type) {
      case COMPR_GZIP:
        retval = compress_with_gzip(src, dest);
        break;
      case COMPR_BZIP:
        retval = compress_with_bzip(src, dest);
        break;
      default:
        retval = -1;
    }

    if (retval == -1) {
	/* Compression failed: restore original file.  */
#ifdef WIN32
        if (dest_backup_name != NULL) {
            remove_file(dest);
        }
#endif
	if (dest_backup_name != NULL && rename(dest_backup_name, dest) < 0) {
	    log_error(zlog,
                      "Could not restore backup file after failed compression.");
	}
    } else {
	/* Compression succeeded: remove backup file.  */
	if (dest_backup_name != NULL && remove_file(dest_backup_name) < 0) {
	    log_error(zlog, "Warning: could not remove backup file.");
	    /* Do not return an error anyway (no data is lost).  */
	}
    }

    if (dest_backup_name)
	free(dest_backup_name);
    return retval;
}

/* ------------------------------------------------------------------------ */

/* Here we have the actual fopen and fclose wrappers.

   These functions work exactly like the standard library versions, but
   handle compression and decompression automatically.  When a file is
   opened, we check whether it looks like a compressed file of some kind.
   If so, we uncompress it and then actually open the uncompressed version.
   When a file that was opened for writing is closed, we re-compress the
   uncompressed version and update the original file.  */

/* `fopen()' wrapper.  */
FILE *zfopen(const char *name, const char *mode)
{
    const char *tmp_name;
    FILE *stream;
    enum compression_type type;
    int write_mode = 0;

    if (!zinit_done)
	zinit();

    /* Do we want to write to this file?  */
    if ((strchr(mode, 'w') != NULL) || (strchr(mode, '+') != NULL))
        write_mode = 1;

    /* Check for write permissions.  */
    if (write_mode && access(name, W_OK) < 0)
	return NULL;

    type = try_uncompress(name, &tmp_name, write_mode);
    if (type == COMPR_NONE) {
	stream = fopen(name, mode);
        if (stream == NULL)
	    return NULL;
        zfile_list_add(NULL, name, type, write_mode, stream, NULL);
	return stream;
    } else if (*tmp_name == '\0') {
	errno = EACCES;
	return NULL;
    }

    /* Open the uncompressed version of the file.  */
    stream = fopen(tmp_name, mode);
    if (stream == NULL)
	return NULL;

    zfile_list_add(tmp_name, name, type, write_mode, stream, NULL);
    return stream;
}

/* Handle close-action of a file.  `ptr' points to the zfile to close.  */
static int handle_close_action(struct zfile *ptr)
{
    if (ptr == NULL || ptr->orig_name == NULL)
	return -1;

    switch(ptr->action) {
    case ZFILE_KEEP:
	break;
    case ZFILE_REQUEST:
/*
	ui_zfile_close_request(ptr->orig_name, ptr->request_string);
	break;
*/
    case ZFILE_DEL:
        if (remove_file(ptr->orig_name) < 0)
	    log_error(zlog, "Cannot unlink `%s': %s",
                  ptr->orig_name, strerror(errno));
	break;
    }
    return 0;
}

/* Handle close of a (compressed file). `ptr' points to the zfile to close.  */
static int handle_close(struct zfile *ptr)
{
    ZDEBUG(("handle_close: closing `%s' (`%s'), write_mode = %d",
            ptr->tmp_name ? ptr->tmp_name : "(null)",
	    ptr->orig_name, ptr->write_mode));

    if (ptr->tmp_name) {
        /* Recompress into the original file.  */
        if (ptr->orig_name
	    && ptr->write_mode
	    && compress(ptr->tmp_name, ptr->orig_name, ptr->type))
	    return -1;

        /* Remove temporary file.  */
        if (remove_file(ptr->tmp_name) < 0)
	    log_error(zlog, "Cannot unlink `%s': %s",
                  ptr->tmp_name, strerror(errno));
    }

    handle_close_action(ptr);

    /* Remove item from list.  */
    if (ptr->prev != NULL)
	ptr->prev->next = ptr->next;
    else
	zfile_list = ptr->next;

    if (ptr->orig_name)
        free(ptr->orig_name);
    if (ptr->tmp_name)
        free(ptr->tmp_name);
    if (ptr->request_string)
        free(ptr->request_string);

    free(ptr);

    return 0;
}

/* `fclose()' wrapper.  */
int zfclose(FILE *stream)
{
    struct zfile *ptr;

    if (!zinit_done) {
	errno = EBADF;
	return -1;
    }

    /* Search for the matching file in the list.  */
    for (ptr = zfile_list; ptr != NULL; ptr = ptr->next) {
	if (ptr->stream == stream) {

	    /* Close temporary file.  */
	    if (fclose(stream) == -1)
		return -1;

	    if (handle_close(ptr) < 0) {
		errno = EBADF;
		return -1;
	    }

	    return 0;
	}
    }

    return fclose(stream);
}

#if 0
/* Close all files.  */
int zclose_all(void)
{
    struct zfile *p = zfile_list, *pnext;
    int ret = 0;

    if (!zinit_done)
	return 0;

    while (p != NULL) {
	if (p->stream != NULL) {
	    if (fclose(p->stream) == -1)
		ret = -1;
	} else if (p->fd != -1) {
	    if (close(p->fd) == -1)
		ret = -1;
	} else {
	    log_error(zlog, "Inconsistent zfile list!");
            if (p->orig_name != NULL)
                free(p->orig_name);
            if (p->tmp_name != NULL)
                free(p->tmp_name);
            pnext = p->next;
            free(p);
            p = pnext;
	    continue;
	}

	/* Recompress into the original file.  */
	if (p->tmp_name) {
	    if (p->orig_name
	        && p->write_mode
		&& compress(p->tmp_name, p->orig_name, p->type)) {
	        return -1;
	    }
	    if (remove_file(p->tmp_name) < 0)
                log_error(zlog, "Cannot unlink `%s': %s",
                      p->tmp_name, strerror(errno));
	}

	handle_close_action(p);

	if (p->orig_name)
	    free(p->orig_name);
	if (p->tmp_name)
	    free(p->tmp_name);
	if (p->request_string)
	    free(p->request_string);
	pnext = p->next;
	free(p);
	p = pnext;
    }
    return ret;
}
#endif

int zfile_close_action(const char *filename, zfile_action_t action,
                       const char *request_str)
{
    char *fullname = NULL;
    struct zfile *p = zfile_list;

    archdep_expand_path(&fullname, filename);

    while (p != NULL) {
	if (p->orig_name && !strcmp(p->orig_name, fullname)) {
	    p->action = action;
	    p->request_string = request_str ? stralloc(request_str): NULL;
	    free(fullname);
	    return 0;
	}
	p = p->next;
    }

    free(fullname);
    return -1;
}

