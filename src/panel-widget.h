/* panel-widget.h
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

#define PANEL_TYPE_WIDGET (panel_widget_get_type())

PANEL_AVAILABLE_IN_ALL
G_DECLARE_DERIVABLE_TYPE (PanelWidget, panel_widget, PANEL, WIDGET, GtkWidget)

struct _PanelWidgetClass
{
  GtkWidgetClass parent_instance;
};

#define PANEL_WIDGET_KIND_ANY      "*"
#define PANEL_WIDGET_KIND_UNKNOWN  "unknown"
#define PANEL_WIDGET_KIND_DOCUMENT "document"
#define PANEL_WIDGET_KIND_UTILITY  "utility"

PANEL_AVAILABLE_IN_ALL
GtkWidget     *panel_widget_new                 (void);
PANEL_AVAILABLE_IN_ALL
GtkWidget     *panel_widget_get_child           (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_child           (PanelWidget   *self,
                                                 GtkWidget     *child);
PANEL_AVAILABLE_IN_ALL
const char    *panel_widget_get_title           (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_title           (PanelWidget   *self,
                                                 const char    *title);
PANEL_AVAILABLE_IN_ALL
GIcon         *panel_widget_get_icon            (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_icon            (PanelWidget   *self,
                                                 GIcon         *icon);
PANEL_AVAILABLE_IN_ALL
const char    *panel_widget_get_icon_name       (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_icon_name       (PanelWidget   *self,
                                                 const char    *icon_name);
PANEL_AVAILABLE_IN_ALL
gboolean       panel_widget_get_reorderable     (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_reorderable     (PanelWidget   *self,
                                                 gboolean       reorderable);
PANEL_AVAILABLE_IN_ALL
gboolean       panel_widget_get_can_maximize    (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_can_maximize    (PanelWidget   *self,
                                                 gboolean       can_maximize);
PANEL_AVAILABLE_IN_ALL
gboolean       panel_widget_get_modified        (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_modified        (PanelWidget   *self,
                                                 gboolean       modified);
PANEL_AVAILABLE_IN_ALL
gboolean       panel_widget_get_needs_attention (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_needs_attention (PanelWidget   *self,
                                                 gboolean       needs_attention);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_maximize            (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_unmaximize          (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
const char    *panel_widget_get_kind            (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_kind            (PanelWidget   *self,
                                                 const char    *kind);
PANEL_AVAILABLE_IN_ALL
gboolean       panel_widget_get_busy            (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_mark_busy           (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_unmark_busy         (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
const GdkRGBA *panel_widget_get_foreground_rgba (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_foreground_rgba (PanelWidget   *self,
                                                 const GdkRGBA *foreground_rgba);
PANEL_AVAILABLE_IN_ALL
const GdkRGBA *panel_widget_get_background_rgba (PanelWidget   *self);
PANEL_AVAILABLE_IN_ALL
void           panel_widget_set_background_rgba (PanelWidget   *self,
                                                 const GdkRGBA *background_rgba);

G_END_DECLS
