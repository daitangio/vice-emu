/*
 * uisnapshot.c - Cartridge save image dialog for the Xaw widget set.
 *
 * Written by
 *  Nathan Huizinga <nathan.huizinga@chess.nl>
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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Toggle.h>

#ifndef ENABLE_TEXTFIELD
#include <X11/Xaw/AsciiText.h>
#else
#include "widgets/TextField.h"
#endif

#include "uicartridge.h"

#include "machine.h"
#include "ui.h"
#include "cartridge.h"

static Widget cartridge_dialog;
static Widget cartridge_dialog_pane;
static Widget file_name_form;
static Widget file_name_label;
static Widget file_name_field;
static Widget browse_button;
static Widget button_box;
static Widget save_button;
static Widget cancel_button;

#define FILL_BOX_WIDTH          10
#define OPTION_LABELS_WIDTH     50
#define OPTION_LABELS_JUSTIFY   XtJustifyLeft

static UI_CALLBACK(browse_callback)
{
    ui_button_t button;

    char *f = ui_select_file("Save cartridge image file", NULL, False, NULL, "*.[cC][rR][tT]",
                             &button, 0);

    if (button == UI_BUTTON_OK)
        XtVaSetValues(file_name_field, XtNstring, f, NULL);
}

static UI_CALLBACK(cancel_callback)
{
    ui_popdown(cartridge_dialog);
}

static UI_CALLBACK(save_callback)
{
    String name;
    
    ui_popdown(cartridge_dialog);

    XtVaGetValues(file_name_field, XtNstring, &name, NULL);
    
    if (cartridge_save_image (name) < 0)
        ui_error("Cannot write cartridge image file\n`%s'\n", name);
}

static void build_cartridge_dialog(void)
{
    if (cartridge_dialog != NULL)
        return;

    cartridge_dialog = ui_create_transient_shell(_ui_top_level,
                                                "cartridgeDialog");

    cartridge_dialog_pane = XtVaCreateManagedWidget
        ("cartridgeDialogPane",
         panedWidgetClass, cartridge_dialog,
         NULL);
    
    file_name_form = XtVaCreateManagedWidget
        ("fileNameForm",
         formWidgetClass, cartridge_dialog_pane,
         XtNshowGrip, False, NULL);
    
    file_name_label = XtVaCreateManagedWidget
        ("fileNameLabel",
         labelWidgetClass, file_name_form,
         XtNjustify, XtJustifyLeft,
         XtNlabel, "File name:",
         XtNborderWidth, 0,
         NULL);

#ifndef ENABLE_TEXTFIELD
    file_name_field = XtVaCreateManagedWidget
        ("fileNameField",
         asciiTextWidgetClass, file_name_form,
         XtNfromHoriz, file_name_label,
         XtNwidth, 200,
         XtNtype, XawAsciiString,
         XtNeditType, XawtextEdit,
         NULL);
#else
    file_name_field = XtVaCreateManagedWidget
        ("fileNameField",
         textfieldWidgetClass, file_name_form,
         XtNfromHoriz, file_name_label,
         XtNwidth, 200,
         XtNstring, "",         /* Otherwise, it does not work correctly.  */
         NULL);
#endif

    browse_button = XtVaCreateManagedWidget
        ("browseButton",
         commandWidgetClass, file_name_form,
         XtNfromHoriz, file_name_field,
         XtNlabel, "Browse...",
         NULL);
    XtAddCallback(browse_button, XtNcallback, browse_callback, NULL);

    button_box = XtVaCreateManagedWidget
        ("buttonBox",
         boxWidgetClass, cartridge_dialog_pane,
         XtNshowGrip, False, NULL);

    save_button = XtVaCreateManagedWidget
        ("saveButton",
         commandWidgetClass, button_box,
         XtNlabel, "Save",
         NULL);
    XtAddCallback(save_button, XtNcallback, save_callback, NULL);
    
    cancel_button = XtVaCreateManagedWidget
        ("cancelButton",
         commandWidgetClass, button_box,
         XtNlabel, "Cancel",
         NULL);
    XtAddCallback(cancel_button, XtNcallback, cancel_callback, NULL);

    XtSetKeyboardFocus(cartridge_dialog_pane, file_name_field);
}

void ui_cartridge_dialog(void)
{
    build_cartridge_dialog();
    ui_popup(cartridge_dialog, "Save cartridge image", True);
}
