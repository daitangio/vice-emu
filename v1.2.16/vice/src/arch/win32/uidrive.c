/*
 * uidrive.c - Implementation of the drive settings dialog box.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "vice.h"

#include <windows.h>
#include <prsht.h>

#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#endif

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "drive.h"
#include "res.h"
#include "resources.h"
#include "serial.h"
#include "ui.h"
#include "uidrive.h"
#include "uilib.h"
#include "winmain.h"

static void enable_controls_for_drive_settings(HWND hwnd, int type)
{
    int drive_type = 0;

    switch (type) {
      case IDC_SELECT_DRIVE_TYPE_1541:
      drive_type = DRIVE_TYPE_1541;
      break;
      case IDC_SELECT_DRIVE_TYPE_1541II:
      drive_type = DRIVE_TYPE_1541II;
      break;
      case IDC_SELECT_DRIVE_TYPE_1571:
      drive_type = DRIVE_TYPE_1571;
      break;
      case IDC_SELECT_DRIVE_TYPE_1581:
      drive_type = DRIVE_TYPE_1581;
      break;
      case IDC_SELECT_DRIVE_TYPE_2031:
      drive_type = DRIVE_TYPE_2031;
      break;
      case IDC_SELECT_DRIVE_TYPE_1001:
      drive_type = DRIVE_TYPE_1001;
      break;
      case IDC_SELECT_DRIVE_TYPE_8050:
      drive_type = DRIVE_TYPE_8050;
      break;
      case IDC_SELECT_DRIVE_TYPE_8250:
      drive_type = DRIVE_TYPE_8250;
      break;
    }

    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_NEVER),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_ASK),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_EXTEND_ACCESS),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_IDLE_NO_IDLE),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_IDLE_TRAP_IDLE),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);

    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_PARALLEL_CABLE),
                 type == IDC_SELECT_DRIVE_TYPE_1541
                 || type == IDC_SELECT_DRIVE_TYPE_1541II
                 || type == IDC_SELECT_DRIVE_TYPE_2031);

    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_2000),
                 DRIVE_EXPANSION_2000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_4000),
                 DRIVE_EXPANSION_4000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_6000),
                 DRIVE_EXPANSION_6000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_8000),
                 DRIVE_EXPANSION_8000(drive_type));
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_A000),
                 DRIVE_EXPANSION_A000(drive_type));
}

static void init_dialog(HWND hwnd, int num)
{
    int drive_type, drive_extend_image_policy, drive_idle_method, n;
    char tmp[256];

    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1541),
                 drive_check_type(DRIVE_TYPE_1541, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1541II),
                 drive_check_type(DRIVE_TYPE_1541II, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1571),
                 drive_check_type(DRIVE_TYPE_1571, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1581),
                 drive_check_type(DRIVE_TYPE_1581, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_2031),
                 drive_check_type(DRIVE_TYPE_2031, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_1001),
                 drive_check_type(DRIVE_TYPE_1001, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_8050),
                 drive_check_type(DRIVE_TYPE_8050, num));
    EnableWindow(GetDlgItem(hwnd, IDC_SELECT_DRIVE_TYPE_8250),
                 drive_check_type(DRIVE_TYPE_8250, num));

    sprintf(tmp, "Drive%dType", num);
    resources_get_value(tmp, (resource_value_t *) &drive_type);
    sprintf(tmp, "Drive%dExtendImagePolicy", num);
    resources_get_value(tmp, (resource_value_t *) &drive_extend_image_policy);
    sprintf(tmp, "Drive%dIdleMethod", num);
    resources_get_value(tmp, (resource_value_t *) &drive_idle_method);

    switch (drive_type) {
      case DRIVE_TYPE_NONE:
        n = IDC_SELECT_DRIVE_TYPE_NONE;
        break;
      case DRIVE_TYPE_1541:
        n = IDC_SELECT_DRIVE_TYPE_1541;
        break;
      case DRIVE_TYPE_1541II:
        n = IDC_SELECT_DRIVE_TYPE_1541II;
        break;
      case DRIVE_TYPE_1571:
        n = IDC_SELECT_DRIVE_TYPE_1571;
        break;
      case DRIVE_TYPE_1581:
        n = IDC_SELECT_DRIVE_TYPE_1581;
        break;
      case DRIVE_TYPE_2031:
        n = IDC_SELECT_DRIVE_TYPE_2031;
        break;
      case DRIVE_TYPE_1001:
        n = IDC_SELECT_DRIVE_TYPE_1001;
        break;
      case DRIVE_TYPE_8050:
        n = IDC_SELECT_DRIVE_TYPE_8050;
        break;
      case DRIVE_TYPE_8250:
        n = IDC_SELECT_DRIVE_TYPE_8250;
        break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_TYPE_1541,
                     IDC_SELECT_DRIVE_TYPE_NONE, n);

    enable_controls_for_drive_settings(hwnd, n);

    switch (drive_extend_image_policy) {
      case DRIVE_EXTEND_NEVER:
        n = IDC_SELECT_DRIVE_EXTEND_NEVER;
        break;
      case DRIVE_EXTEND_ASK:
        n = IDC_SELECT_DRIVE_EXTEND_ASK;
        break;
      case DRIVE_EXTEND_ACCESS:
        n = IDC_SELECT_DRIVE_EXTEND_ACCESS;
        break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_EXTEND_NEVER,
                     IDC_SELECT_DRIVE_EXTEND_ACCESS, n);

    switch (drive_idle_method) {
      case DRIVE_IDLE_NO_IDLE:
        n = IDC_SELECT_DRIVE_IDLE_NO_IDLE;
        break;
      case DRIVE_IDLE_TRAP_IDLE:
        n = IDC_SELECT_DRIVE_IDLE_TRAP_IDLE;
        break;
      case DRIVE_IDLE_SKIP_CYCLES:
        n = IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES;
        break;
    }

    CheckRadioButton(hwnd, IDC_SELECT_DRIVE_IDLE_NO_IDLE,
                     IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES, n);

    sprintf(tmp, "Drive%dParallelCable", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_PARALLEL_CABLE, n
                   ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "Drive%dRAM2000", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_2000, n
                   ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "Drive%dRAM4000", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_4000, n
                   ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "Drive%dRAM6000", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_6000, n
                   ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "Drive%dRAM8000", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_8000, n
                   ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "Drive%dRAMA000", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_DRIVE_EXPANSION_A000, n
                   ? BST_CHECKED : BST_UNCHECKED);
}

static BOOL CALLBACK dialog_proc(int num, HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int n;
    char tmp[256];

    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd, num);
        return TRUE;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SELECT_DRIVE_TYPE_NONE:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_NONE, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_1541:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_1541, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_1541II:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_1541II, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_1571:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_1571, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_1581:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_1581, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_2031:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_2031, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_1001:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_1001, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_8050:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_8050, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_TYPE_8250:
            ui_set_res_num("Drive%dType", DRIVE_TYPE_8250, num);
            enable_controls_for_drive_settings(hwnd, LOWORD(wparam));
            break;
          case IDC_SELECT_DRIVE_EXTEND_NEVER:
            ui_set_res_num("Drive%dExtendImagePolicy", DRIVE_EXTEND_NEVER, num);
            break;
          case IDC_SELECT_DRIVE_EXTEND_ASK:
            ui_set_res_num("Drive%dExtendImagePolicy", DRIVE_EXTEND_ASK, num);
            break;
          case IDC_SELECT_DRIVE_EXTEND_ACCESS:
            ui_set_res_num("Drive%dExtendImagePolicy", DRIVE_EXTEND_ACCESS, num);
            break;
          case IDC_SELECT_DRIVE_IDLE_NO_IDLE:
            ui_set_res_num("Drive%dIdleMethod", DRIVE_IDLE_NO_IDLE, num);
            break;
          case IDC_SELECT_DRIVE_IDLE_TRAP_IDLE:
            ui_set_res_num("Drive%dIdleMethod", DRIVE_IDLE_TRAP_IDLE, num);
            break;
          case IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES:
            ui_set_res_num("Drive%dIdleMethod", DRIVE_IDLE_SKIP_CYCLES, num);
            break;
          case IDC_TOGGLE_DRIVE_PARALLEL_CABLE:
            sprintf(tmp, "Drive%dParallelCable", num);
            resources_get_value(tmp, (resource_value_t *) &n);
            resources_set_value(tmp, (resource_value_t) !n);
            break;
          case IDC_TOGGLE_DRIVE_EXPANSION_2000:
            sprintf(tmp, "Drive%dRAM2000", num);
            resources_get_value(tmp, (resource_value_t *) &n);
            resources_set_value(tmp, (resource_value_t) !n);
            break;
          case IDC_TOGGLE_DRIVE_EXPANSION_4000:
            sprintf(tmp, "Drive%dRAM4000", num);
            resources_get_value(tmp, (resource_value_t *) &n);
            resources_set_value(tmp, (resource_value_t) !n);
            break;
          case IDC_TOGGLE_DRIVE_EXPANSION_6000:
            sprintf(tmp, "Drive%dRAM6000", num);
            resources_get_value(tmp, (resource_value_t *) &n);
            resources_set_value(tmp, (resource_value_t) !n);
            break;
          case IDC_TOGGLE_DRIVE_EXPANSION_8000:
            sprintf(tmp, "Drive%dRAM8000", num);
            resources_get_value(tmp, (resource_value_t *) &n);
            resources_set_value(tmp, (resource_value_t) !n);
            break;
          case IDC_TOGGLE_DRIVE_EXPANSION_A000:
            sprintf(tmp, "Drive%dRAMA000", num);
            resources_get_value(tmp, (resource_value_t *) &n);
            resources_set_value(tmp, (resource_value_t) !n);
            break;
          default:
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

#define _CALLBACK(num)                                            \
static BOOL CALLBACK callback_##num(HWND dialog, UINT msg,        \
                                    WPARAM wparam, LPARAM lparam) \
{                                                                 \
    return dialog_proc(num, dialog, msg, wparam, lparam);         \
}

_CALLBACK(8)
_CALLBACK(9)

void ui_drive_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[2];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 2; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef HAVE_UNNAMED_UNIONS
        psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG);
        psp[i].pszIcon = NULL;
#else
        psp[i].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_DRIVE_SETTINGS_DIALOG);
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = callback_8;
    psp[0].pszTitle = "Drive 8";
    psp[1].pfnDlgProc = callback_9;
    psp[1].pszTitle = "Drive 9";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = "Drive Settings";
    psh.nPages = 2;
#ifdef HAVE_UNNAMED_UNIONS
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

