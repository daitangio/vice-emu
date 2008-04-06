#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

/* BYTE is already defined in os2def.h    */
/* ADDRESS is already defined in os2def.h */
/* #define ADDRESS 0                      */
#undef BYTE
#undef ADDRESS
#define INCL_DOSSEMAPHORES
#include <os2.h>
#include <dive.h>
#undef ADDRESS
#define ADDRESS WORD

#include "palette.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

typedef struct video_frame_buffer_s {
    int   width;
    int   height;
    BYTE *bitmap;
} video_frame_buffer_t;

#define VIDEO_FRAME_BUFFER_LINE_START(f,n)  ((f->bitmap) + ((f->width) \
                                            * sizeof(BYTE)) * n)
#define VIDEO_FRAME_BUFFER_SIZE(f)          (f->width)
#define VIDEO_FRAME_BUFFER_START(f)         (f->bitmap)

typedef void (*canvas_redraw_t)(UINT width, UINT height);

typedef struct video_canvas_s
{
    HWND   hwndFrame;        // Handle to Frame of Window = WinQueryWindow(hwndClient, QW_PARENT)
    HWND   hwndClient;       // Handle to Paint Area of Window
    HWND   hwndMenu;         // Handle to menu bar
    HWND   hwndTitlebar;     // Hwndle to titlebar
    HWND   hwndPopupMenu;    // Handle to popup menu
    HMTX   hmtx;
    char  *title;
    UINT   width;            // width of canvas graphic area
    UINT   height;           // width of canvas graphic area
    BYTE   stretch;          // actual stretch factor of canvas
    DWORD *palette;          // number of 16-block of palette entries
    BOOL   vrenabled;        // only BlitImage when Visible Region Enabled
    BYTE  *bitmaptrg;
    ULONG  ulBuffer; // DIVE buffer number
    HDIVE  hDiveInst;
    SETUP_BLITTER divesetup;
    canvas_redraw_t exposure;
};

typedef struct video_canvas_s video_canvas_t;

#endif
