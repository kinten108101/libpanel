/* panel-dock.h
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

#include "panel-frame.h"
#include "panel-version-macros.h"
#include "panel-widget.h"

G_BEGIN_DECLS

#define PANEL_TYPE_DOCK (panel_dock_get_type())

typedef enum
{
  PANEL_DOCK_POSITION_START,
  PANEL_DOCK_POSITION_END,
  PANEL_DOCK_POSITION_TOP,
  PANEL_DOCK_POSITION_BOTTOM,
  PANEL_DOCK_POSITION_CENTER,
} PanelDockPosition;

typedef void (*PanelFrameCallback) (PanelFrame *frame,
                                    gpointer    user_data);

PANEL_AVAILABLE_IN_ALL
G_DECLARE_DERIVABLE_TYPE (PanelDock, panel_dock, PANEL, DOCK, GtkWidget)

struct _PanelDockClass
{
  GtkWidgetClass parent_class;

  void (*panel_drag_begin) (PanelDock   *self,
                            PanelWidget *widget);
  void (*panel_drag_end)   (PanelDock   *self,
                            PanelWidget *widget);
};

PANEL_AVAILABLE_IN_ALL
GtkWidget *panel_dock_new                   (void);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_reveal_start      (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_reveal_end        (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_reveal_top        (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_reveal_bottom     (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_reveal_start      (PanelDock          *self,
                                             gboolean            reveal_start);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_reveal_end        (PanelDock          *self,
                                             gboolean            reveal_end);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_reveal_top        (PanelDock          *self,
                                             gboolean            reveal_top);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_reveal_bottom     (PanelDock          *self,
                                             gboolean            reveal_bottom);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_can_reveal_bottom (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_can_reveal_top    (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_can_reveal_start  (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
gboolean   panel_dock_get_can_reveal_end    (PanelDock          *self);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_start_width       (PanelDock          *self,
                                             int                 width);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_end_width         (PanelDock          *self,
                                             int                 width);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_top_height        (PanelDock          *self,
                                             int                 height);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_set_bottom_height     (PanelDock          *self,
                                             int                 height);
PANEL_AVAILABLE_IN_ALL
void       panel_dock_foreach_frame         (PanelDock          *self,
                                             PanelFrameCallback  callback,
                                             gpointer            user_data);

G_END_DECLS
