/* panel-statusbar.c
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

#include "config.h"

#include "panel-statusbar.h"

struct _PanelStatusbar
{
  GtkWidget  parent_instance;
  GtkWidget *expander;
};

static void buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (PanelStatusbar, panel_statusbar, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, buildable_iface_init))

GtkWidget *
panel_statusbar_new (void)
{
  return g_object_new (PANEL_TYPE_STATUSBAR, NULL);
}

static void
panel_statusbar_dispose (GObject *object)
{
  PanelStatusbar *self = (PanelStatusbar *)object;
  GtkWidget *child;

  self->expander = NULL;

  while ((child = gtk_widget_get_first_child (GTK_WIDGET (self))))
    gtk_widget_unparent (child);

  G_OBJECT_CLASS (panel_statusbar_parent_class)->dispose (object);
}

static void
panel_statusbar_class_init (PanelStatusbarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = panel_statusbar_dispose;

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, "panelstatusbar");
}

static void
panel_statusbar_init (PanelStatusbar *self)
{
  self->expander = g_object_new (GTK_TYPE_LABEL,
                                 "hexpand", TRUE,
                                 "visible", TRUE,
                                 NULL);
  gtk_widget_add_css_class (self->expander, "expander");
  gtk_widget_set_parent (self->expander, GTK_WIDGET (self));
}

static void
update_expander (PanelStatusbar *self)
{
  gboolean hexpand = FALSE;

  g_assert (PANEL_IS_STATUSBAR (self));

  for (GtkWidget *child = gtk_widget_get_first_child (GTK_WIDGET (self));
       child != NULL;
       child = gtk_widget_get_next_sibling (child))
    {
      if (child != self->expander)
        hexpand |= gtk_widget_get_hexpand (child);
    }

  gtk_widget_set_visible (self->expander, !hexpand);
}

void
panel_statusbar_add_prefix (PanelStatusbar *self,
                            GtkWidget      *widget)
{
  g_return_if_fail (PANEL_IS_STATUSBAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  gtk_widget_insert_before (widget, GTK_WIDGET (self), self->expander);
  update_expander (self);
}

void
panel_statusbar_add_suffix (PanelStatusbar *self,
                            GtkWidget      *widget)
{
  g_return_if_fail (PANEL_IS_STATUSBAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  gtk_widget_insert_after (widget, GTK_WIDGET (self), self->expander);
  update_expander (self);
}

void
panel_statusbar_remove (PanelStatusbar *self,
                        GtkWidget      *widget)
{
  g_return_if_fail (PANEL_IS_STATUSBAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));
  g_return_if_fail (GTK_WIDGET (self) == gtk_widget_get_parent (widget));
  g_return_if_fail (widget != self->expander);

  gtk_widget_unparent (widget);
  update_expander (self);
}

static void
panel_statusbar_add_child (GtkBuildable *buildable,
                           GtkBuilder   *builder,
                           GObject      *child,
                           const char   *type)
{
  PanelStatusbar *self = (PanelStatusbar *)buildable;

  g_assert (PANEL_IS_STATUSBAR (self));
  g_assert (GTK_IS_BUILDER (builder));

  if (g_strcmp0 (type, "suffix") == 0)
    panel_statusbar_add_suffix (self, GTK_WIDGET (child));
  else if (GTK_IS_WIDGET (child))
    panel_statusbar_add_prefix (self, GTK_WIDGET (child));
  else
    g_warning ("%s cannot add child of type %s",
               G_OBJECT_TYPE_NAME (self),
               G_OBJECT_TYPE_NAME (child));
}

static void
buildable_iface_init (GtkBuildableIface *iface)
{
  iface->add_child = panel_statusbar_add_child;
}
