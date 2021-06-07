/* panel-frame-header.h
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

#include "panel-version-macros.h"

G_BEGIN_DECLS

#define PANEL_TYPE_FRAME_HEADER (panel_frame_header_get_type())

typedef struct _PanelFrame PanelFrame;

PANEL_AVAILABLE_IN_ALL
G_DECLARE_INTERFACE (PanelFrameHeader, panel_frame_header, PANEL, FRAME_HEADER, GtkWidget)

PANEL_AVAILABLE_IN_ALL
PanelFrame *panel_frame_header_get_frame (PanelFrameHeader *self);
PANEL_AVAILABLE_IN_ALL
void        panel_frame_header_set_frame (PanelFrameHeader *self,
                                          PanelFrame       *frame);

G_END_DECLS
