/* panel-switcher.c
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

#include "config.h"

#include "panel-dock-private.h"
#include "panel-dock-child-private.h"
#include "panel-dock-switcher.h"
#include "panel-enums.h"

#define TIMEOUT_EXPAND 500
#define EMPTY_DRAG_SIZE 100

struct _PanelDockSwitcher
{
  GtkWidget        parent_instance;

  PanelArea        area;

  PanelDock       *dock;

  GtkToggleButton *button;
  GtkRevealer     *revealer;
  GBinding        *binding;
};

G_DEFINE_TYPE (PanelDockSwitcher, panel_dock_switcher, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_DOCK,
  PROP_AREA,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
panel_dock_switcher_new (PanelDock *dock,
                         PanelArea  area)
{
  g_return_val_if_fail (PANEL_IS_DOCK (dock), NULL);
  g_return_val_if_fail (area == PANEL_AREA_START ||
                        area == PANEL_AREA_END ||
                        area == PANEL_AREA_TOP ||
                        area == PANEL_AREA_BOTTOM,
                        NULL);

  return g_object_new (PANEL_TYPE_DOCK_SWITCHER,
                       "dock", dock,
                       "area", area,
                       NULL);
}

static gboolean
panel_dock_switcher_switch_timeout (gpointer data)
{
  GtkToggleButton *button = data;

  g_assert (GTK_IS_TOGGLE_BUTTON (button));

  g_object_steal_data (G_OBJECT (button), "-panel-switch-timer");
  gtk_toggle_button_set_active (button, TRUE);

  return G_SOURCE_REMOVE;
}

static void
drag_enter_cb (GtkDropControllerMotion *motion,
               double                   x,
               double                   y,
               G_GNUC_UNUSED gpointer   unused)
{
  GtkWidget *button;

  g_assert (GTK_IS_DROP_CONTROLLER_MOTION (motion));

  button = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (motion));

  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
    {
      guint switch_timer = g_timeout_add_full (G_PRIORITY_DEFAULT,
                                               TIMEOUT_EXPAND,
                                               panel_dock_switcher_switch_timeout,
                                               g_object_ref (button),
                                               g_object_unref);
      g_source_set_name_by_id (switch_timer, "[panel] panel_dock_switcher_switch_timeout");
      g_object_set_data (G_OBJECT (button), "-panel-switch-timer", GUINT_TO_POINTER (switch_timer));
    }
}

static void
drag_leave_cb (GtkDropControllerMotion *motion,
               G_GNUC_UNUSED gpointer   unused)
{
  GtkWidget *button;
  guint switch_timer;

  g_assert (GTK_IS_DROP_CONTROLLER_MOTION (motion));

  button = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (motion));
  switch_timer = GPOINTER_TO_UINT (g_object_steal_data (G_OBJECT (button), "-panel-switch-timer"));
  if (switch_timer)
    g_source_remove (switch_timer);
}

static void
notify_child_revealed_cb (GtkRevealer            *revealer,
                          GParamSpec             *pspec,
                          G_GNUC_UNUSED gpointer  user_data)
{
  g_assert (GTK_IS_REVEALER (revealer));

  if (!gtk_revealer_get_child_revealed (revealer))
    gtk_widget_hide (GTK_WIDGET (revealer));
}

static void
panel_dock_switcher_panel_drag_begin_cb (PanelDockSwitcher *self,
                                         PanelWidget       *widget,
                                         PanelDock         *dock)
{
  g_assert (PANEL_IS_DOCK_SWITCHER (self));
  g_assert (PANEL_IS_WIDGET (widget));
  g_assert (PANEL_IS_DOCK (dock));

  gtk_widget_add_css_class (GTK_WIDGET (self), "drag-active");

  if (!gtk_widget_get_visible (GTK_WIDGET (self->revealer)))
    {
      gtk_toggle_button_set_active (self->button, FALSE);
      gtk_widget_show (GTK_WIDGET (self->revealer));
      gtk_revealer_set_reveal_child (self->revealer, TRUE);
    }
}

static void
panel_dock_switcher_panel_drag_end_cb (PanelDockSwitcher *self,
                                       PanelWidget       *widget,
                                       PanelDock         *dock)
{
  g_assert (PANEL_IS_DOCK_SWITCHER (self));
  g_assert (PANEL_IS_WIDGET (widget));
  g_assert (PANEL_IS_DOCK (dock));

  gtk_widget_remove_css_class (GTK_WIDGET (self), "drag-active");

  if (!panel_dock_get_can_reveal_area (dock, self->area))
    {
      gtk_revealer_set_reveal_child (self->revealer, FALSE);
      gtk_toggle_button_set_active (self->button, FALSE);
    }
}

static void
panel_dock_switcher_notify_can_reveal_cb (PanelDock   *dock,
                                          GParamSpec  *pspec,
                                          GtkRevealer *revealer)
{
  GValue value = G_VALUE_INIT;

  g_assert (PANEL_IS_DOCK (dock));
  g_assert (G_IS_PARAM_SPEC_BOOLEAN (pspec));
  g_assert (GTK_IS_REVEALER (revealer));

  g_value_init (&value, G_TYPE_BOOLEAN);
  g_object_get_property (G_OBJECT (dock), pspec->name, &value);

  if (g_value_get_boolean (&value))
    {
      gtk_widget_show (GTK_WIDGET (revealer));
      gtk_revealer_set_reveal_child (revealer, TRUE);
    }
  else
    {
      gtk_revealer_set_reveal_child (revealer, FALSE);
    }

  g_value_unset (&value);
}

static void
panel_dock_switcher_set_dock (PanelDockSwitcher *self,
                              PanelDock         *dock)
{
  g_return_if_fail (PANEL_IS_DOCK_SWITCHER (self));
  g_return_if_fail (!dock || PANEL_IS_DOCK (dock));

  if (dock == self->dock)
    return;

  if (self->dock)
    {
      g_clear_pointer (&self->binding, g_binding_unbind);
      g_signal_handlers_disconnect_by_func (self->dock,
                                            G_CALLBACK (panel_dock_switcher_notify_can_reveal_cb),
                                            self->revealer);
      g_signal_handlers_disconnect_by_func (self->dock,
                                            G_CALLBACK (panel_dock_switcher_panel_drag_begin_cb),
                                            self);
      g_signal_handlers_disconnect_by_func (self->dock,
                                            G_CALLBACK (panel_dock_switcher_panel_drag_end_cb),
                                            self);
    }

  g_set_object (&self->dock, dock);

  if (self->dock)
    {
      const char *nick = NULL;
      char *notify = NULL;

      switch (self->area)
        {
        case PANEL_AREA_END: nick = "reveal-end"; break;
        case PANEL_AREA_TOP: nick = "reveal-top"; break;
        case PANEL_AREA_BOTTOM: nick = "reveal-bottom"; break;
        case PANEL_AREA_START: nick = "reveal-start"; break;
        case PANEL_AREA_CENTER:
        default: break;
        }

      notify = g_strdup_printf ("notify::can-%s", nick);

      /* Set default state before binding */
      gtk_toggle_button_set_active (self->button,
                                    panel_dock_get_reveal_area (self->dock, self->area));
      gtk_revealer_set_reveal_child (self->revealer,
                                     panel_dock_get_can_reveal_area (self->dock, self->area));

      self->binding = g_object_bind_property (self->dock, nick,
                                              self->button, "active",
                                              G_BINDING_BIDIRECTIONAL);
      g_signal_connect_object (self->dock,
                               "panel-drag-begin",
                               G_CALLBACK (panel_dock_switcher_panel_drag_begin_cb),
                               self,
                               G_CONNECT_SWAPPED);
      g_signal_connect_object (self->dock,
                               "panel-drag-end",
                               G_CALLBACK (panel_dock_switcher_panel_drag_end_cb),
                               self,
                               G_CONNECT_SWAPPED);
      g_signal_connect_object (self->dock,
                               notify,
                               G_CALLBACK (panel_dock_switcher_notify_can_reveal_cb),
                               self->revealer,
                               0);

      g_clear_pointer (&notify, g_free);
    }
}

static void
panel_dock_switcher_constructed (GObject *object)
{
  PanelDockSwitcher *self = (PanelDockSwitcher *)object;

  g_assert (PANEL_IS_DOCK_SWITCHER (self));

  G_OBJECT_CLASS (panel_dock_switcher_parent_class)->constructed (object);

  switch (self->area)
    {
    case PANEL_AREA_START:
      g_object_set (self->button, "icon-name", "panel-left-symbolic", NULL);
      break;

    case PANEL_AREA_END:
      g_object_set (self->button, "icon-name", "panel-right-symbolic", NULL);
      break;

    case PANEL_AREA_TOP:
      g_object_set (self->button, "icon-name", "panel-top-symbolic", NULL);
      break;

    case PANEL_AREA_BOTTOM:
      g_object_set (self->button, "icon-name", "panel-bottom-symbolic", NULL);
      break;

    case PANEL_AREA_CENTER:
    default:
      break;
    }
}

static void
panel_dock_switcher_dispose (GObject *object)
{
  PanelDockSwitcher *self = (PanelDockSwitcher *)object;

  g_clear_object (&self->dock);
  g_clear_pointer ((GtkWidget **)&self->revealer, gtk_widget_unparent);

  G_OBJECT_CLASS (panel_dock_switcher_parent_class)->dispose (object);
}

static void
panel_dock_switcher_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  PanelDockSwitcher *self = PANEL_DOCK_SWITCHER (object);

  switch (prop_id)
    {
    case PROP_DOCK:
      g_value_set_object (value, self->dock);
      break;

    case PROP_AREA:
      g_value_set_enum (value, self->area);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_dock_switcher_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  PanelDockSwitcher *self = PANEL_DOCK_SWITCHER (object);

  switch (prop_id)
    {
    case PROP_DOCK:
      panel_dock_switcher_set_dock (self, g_value_get_object (value));
      break;

    case PROP_AREA:
      self->area = g_value_get_enum (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_dock_switcher_class_init (PanelDockSwitcherClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = panel_dock_switcher_constructed;
  object_class->dispose = panel_dock_switcher_dispose;
  object_class->get_property = panel_dock_switcher_get_property;
  object_class->set_property = panel_dock_switcher_set_property;

  properties [PROP_DOCK] =
    g_param_spec_object ("dock", NULL, NULL,
                         PANEL_TYPE_DOCK,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_AREA] =
    g_param_spec_enum ("area", NULL, NULL,
                       PANEL_TYPE_AREA,
                       PANEL_AREA_START,
                       (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BIN_LAYOUT);
  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/libpanel/panel-dock-switcher.ui");
  gtk_widget_class_set_css_name (widget_class, "paneldockswitcher");
  gtk_widget_class_bind_template_child (widget_class, PanelDockSwitcher, button);
  gtk_widget_class_bind_template_child (widget_class, PanelDockSwitcher, revealer);
  gtk_widget_class_bind_template_callback (widget_class, drag_enter_cb);
  gtk_widget_class_bind_template_callback (widget_class, drag_leave_cb);
  gtk_widget_class_bind_template_callback (widget_class, notify_child_revealed_cb);
}

static void
panel_dock_switcher_init (PanelDockSwitcher *self)
{
  self->area = PANEL_AREA_START;

  gtk_widget_init_template (GTK_WIDGET (self));
}
