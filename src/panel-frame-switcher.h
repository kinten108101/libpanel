/* panel-frame-switcher.h
 *
 * Copyright 2021 Christian Hergert <chergert@redhat.com>
 *
 * This file is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include <gtk/gtk.h>

#include "panel-frame-header.h"
#include "panel-version-macros.h"

G_BEGIN_DECLS

#define PANEL_TYPE_FRAME_SWITCHER (panel_frame_switcher_get_type())

PANEL_AVAILABLE_IN_ALL
G_DECLARE_FINAL_TYPE (PanelFrameSwitcher, panel_frame_switcher, PANEL, FRAME_SWITCHER, GtkWidget)

PANEL_AVAILABLE_IN_ALL
GtkWidget     *panel_frame_switcher_new                 (void);
PANEL_AVAILABLE_IN_ALL
const GdkRGBA *panel_frame_switcher_get_background_rgba (PanelFrameSwitcher *self);
PANEL_AVAILABLE_IN_ALL
const GdkRGBA *panel_frame_switcher_get_foreground_rgba (PanelFrameSwitcher *self);
PANEL_AVAILABLE_IN_ALL
void           panel_frame_switcher_set_background_rgba (PanelFrameSwitcher *self,
                                                         const GdkRGBA      *background_rgba);
PANEL_AVAILABLE_IN_ALL
void           panel_frame_switcher_set_foreground_rgba (PanelFrameSwitcher *self,
                                                         const GdkRGBA      *foreground_rgba);

G_END_DECLS
