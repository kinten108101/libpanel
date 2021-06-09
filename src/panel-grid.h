/* panel-grid.h
 *
 * Copyright 2021 Christian Hergert <chergert@redhat.com>
 *
 * This file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

#pragma once

#include <gtk/gtk.h>

#include "panel-grid-column.h"
#include "panel-version-macros.h"

G_BEGIN_DECLS

#define PANEL_TYPE_GRID (panel_grid_get_type())

PANEL_AVAILABLE_IN_ALL
G_DECLARE_FINAL_TYPE (PanelGrid, panel_grid, PANEL, GRID, GtkWidget)

PANEL_AVAILABLE_IN_ALL
GtkWidget       *panel_grid_new                    (void);
PANEL_AVAILABLE_IN_ALL
PanelGridColumn *panel_grid_get_most_recent_column (PanelGrid   *self);
PANEL_AVAILABLE_IN_ALL
PanelFrame      *panel_grid_get_most_recent_frame  (PanelGrid   *self);
PANEL_AVAILABLE_IN_ALL
void             panel_grid_add                    (PanelGrid   *self,
                                                    PanelWidget *widget);
PANEL_AVAILABLE_IN_ALL
PanelGridColumn *panel_grid_get_column             (PanelGrid   *self,
                                                    guint        column);

G_END_DECLS
