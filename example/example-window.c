/* example-window.c
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

#include <libpanel.h>

#include "example-window.h"

struct _ExampleWindow
{
  GtkApplicationWindow parent_instance;
  PanelDock *dock;
  PanelGrid *grid;
  GMenuModel *page_menu;
};

G_DEFINE_TYPE (ExampleWindow, example_window, ADW_TYPE_APPLICATION_WINDOW)

GtkWidget *
example_window_new (GtkApplication *application)
{
  return g_object_new (EXAMPLE_TYPE_WINDOW,
                       "application", application,
                       NULL);
}

static GtkWidget *
get_default_focus_cb (GtkWidget *widget,
                      GtkWidget *text_view)
{
  return text_view;
}

static void
example_window_add_document (ExampleWindow *self)
{
  static guint count;
  PanelWidget *widget;
  GtkWidget *text_view;
  char *title;

  g_return_if_fail (EXAMPLE_IS_WINDOW (self));

  title = g_strdup_printf ("Untitled Document %u", ++count);
  text_view = g_object_new (GTK_TYPE_TEXT_VIEW,
                            "buffer", g_object_new (GTK_TYPE_TEXT_BUFFER,
                                                    "text", title,
                                                    NULL),
                            NULL);
  widget = g_object_new (PANEL_TYPE_WIDGET,
                         "title", title,
                         "icon-name", "text-x-generic-symbolic",
                         "menu-model", self->page_menu,
                         "can-maximize", TRUE,
                         "child", g_object_new (GTK_TYPE_SCROLLED_WINDOW,
                                                "child", text_view,
                                                NULL),
                         NULL);
  g_signal_connect (widget, "get-default-focus", G_CALLBACK (get_default_focus_cb), text_view);

  panel_grid_add (self->grid, widget);
  panel_widget_raise (widget);
  panel_widget_focus_default (widget);
}

static void
add_document_action (GtkWidget  *widget,
                     const char *action_name,
                     GVariant   *param)
{
  example_window_add_document (EXAMPLE_WINDOW (widget));
}

static PanelFrame *
create_frame_cb (PanelGrid *grid)
{
  PanelFrame *frame = PANEL_FRAME (panel_frame_new ());
  PanelFrameHeader *header = PANEL_FRAME_HEADER (panel_frame_header_bar_new ());
  panel_frame_set_header (frame, header);
  return frame;
}

static void
example_window_class_init (ExampleWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/example-window.ui");
  gtk_widget_class_bind_template_child (widget_class, ExampleWindow, dock);
  gtk_widget_class_bind_template_child (widget_class, ExampleWindow, grid);
  gtk_widget_class_bind_template_child (widget_class, ExampleWindow, page_menu);
  gtk_widget_class_bind_template_callback (widget_class, create_frame_cb);

  gtk_widget_class_install_action (widget_class, "document.new", NULL, add_document_action);

  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_n, GDK_CONTROL_MASK, "document.new", NULL);
}

static void
example_window_init (ExampleWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  example_window_add_document (self);
}
