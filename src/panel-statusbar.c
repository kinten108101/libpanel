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
  GtkBox    *box;
  GtkBox    *prefix;
  GtkBox    *suffix;
};

static void buildable_iface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (PanelStatusbar, panel_statusbar, GTK_TYPE_WIDGET,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE, buildable_iface_init))

enum {
  PROP_0,
  N_PROPS
};

G_GNUC_UNUSED static GParamSpec *properties [N_PROPS];

GtkWidget *
panel_statusbar_new (void)
{
  return g_object_new (PANEL_TYPE_STATUSBAR, NULL);
}

static void
panel_statusbar_dispose (GObject *object)
{
  PanelStatusbar *self = (PanelStatusbar *)object;

  g_clear_pointer ((GtkWidget **)&self->box, gtk_widget_unparent);

  G_OBJECT_CLASS (panel_statusbar_parent_class)->dispose (object);
}

static void
panel_statusbar_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  G_GNUC_UNUSED PanelStatusbar *self = PANEL_STATUSBAR (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_statusbar_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  G_GNUC_UNUSED PanelStatusbar *self = PANEL_STATUSBAR (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_statusbar_class_init (PanelStatusbarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = panel_statusbar_dispose;
  object_class->get_property = panel_statusbar_get_property;
  object_class->set_property = panel_statusbar_set_property;

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, "panelstatusbar");
}

static void
panel_statusbar_init (PanelStatusbar *self)
{
  self->box = g_object_new (GTK_TYPE_BOX,
                            "orientation", GTK_ORIENTATION_HORIZONTAL,
                            NULL);
  gtk_widget_set_parent (GTK_WIDGET (self->box), GTK_WIDGET (self));

  self->prefix = g_object_new (GTK_TYPE_BOX,
                               "orientation", GTK_ORIENTATION_HORIZONTAL,
                               NULL);
  gtk_box_prepend (self->box, GTK_WIDGET (self->prefix));

  self->suffix = g_object_new (GTK_TYPE_BOX,
                               "orientation", GTK_ORIENTATION_HORIZONTAL,
                               NULL);
  gtk_box_append (self->box, GTK_WIDGET (self->suffix));
}

void
panel_statusbar_add_prefix (PanelStatusbar *self,
                            GtkWidget      *widget)
{
  g_return_if_fail (PANEL_IS_STATUSBAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  gtk_box_append (self->prefix, widget);
}

void
panel_statusbar_add_suffix (PanelStatusbar *self,
                            GtkWidget      *widget)
{
  g_return_if_fail (PANEL_IS_STATUSBAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  gtk_box_prepend (self->suffix, widget);
}

void
panel_statusbar_remove (PanelStatusbar *self,
                        GtkWidget      *widget)
{
  g_return_if_fail (PANEL_IS_STATUSBAR (self));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  if (gtk_widget_get_parent (widget) == GTK_WIDGET (self->prefix))
    gtk_box_remove (self->prefix, widget);
  else
    gtk_box_remove (self->suffix, widget);
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
