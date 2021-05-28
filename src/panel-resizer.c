/* panel-resizer.c
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

#include "panel-handle-private.h"
#include "panel-resizer-private.h"

#define HANDLE_SIZE 8

struct _PanelResizer
{
  GtkWidget          parent_instance;

  PanelHandle   *handle;
  GtkWidget         *child;

  double             drag_orig_size;
  double             drag_position;

  PanelDockPosition  position : 3;
};

G_DEFINE_TYPE (PanelResizer, panel_resizer, GTK_TYPE_WIDGET)

enum {
  PROP_0,
  PROP_CHILD,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
panel_resizer_drag_begin_cb (PanelResizer *self,
                                  double            start_x,
                                  double            start_y,
                                  GtkGestureDrag   *drag)
{
  g_assert (PANEL_IS_RESIZER (self));
  g_assert (GTK_IS_GESTURE_DRAG (drag));

  if (self->child != NULL)
    {
      switch (self->position)
        {
        case PANEL_DOCK_POSITION_START:
          if (start_x > gtk_widget_get_width (GTK_WIDGET (self)) - HANDLE_SIZE)
            goto start_drag;
          break;

        case PANEL_DOCK_POSITION_END:
          if (start_x <= HANDLE_SIZE)
            goto start_drag;
          break;

        case PANEL_DOCK_POSITION_TOP:
          if (start_y > gtk_widget_get_height (GTK_WIDGET (self)) - HANDLE_SIZE)
            goto start_drag;
          break;

        case PANEL_DOCK_POSITION_BOTTOM:
          if (start_y <= HANDLE_SIZE)
            goto start_drag;
          break;

        case PANEL_DOCK_POSITION_CENTER:
        default:
          break;
        }
    }

  gtk_gesture_set_state (GTK_GESTURE (drag),
                         GTK_EVENT_SEQUENCE_DENIED);

  return;

start_drag:

  if (self->position == PANEL_DOCK_POSITION_START ||
      self->position == PANEL_DOCK_POSITION_END)
    self->drag_orig_size = gtk_widget_get_width (GTK_WIDGET (self->child));
  else
    self->drag_orig_size = gtk_widget_get_height (GTK_WIDGET (self->child));
}

static void
panel_resizer_drag_update_cb (PanelResizer *self,
                                   double            offset_x,
                                   double            offset_y,
                                   GtkGestureDrag   *drag)
{
  g_assert (PANEL_IS_RESIZER (self));
  g_assert (GTK_IS_GESTURE_DRAG (drag));

  if (self->position == PANEL_DOCK_POSITION_START)
    self->drag_position = self->drag_orig_size + offset_x;
  else if (self->position == PANEL_DOCK_POSITION_END)
    self->drag_position = gtk_widget_get_width (GTK_WIDGET (self)) - offset_x;
  else if (self->position == PANEL_DOCK_POSITION_TOP)
    self->drag_position = self->drag_orig_size + offset_y;
  else if (self->position == PANEL_DOCK_POSITION_BOTTOM)
    self->drag_position = gtk_widget_get_height (GTK_WIDGET (self)) - offset_y;

  gtk_widget_queue_resize (GTK_WIDGET (self));
}

static void
panel_resizer_drag_end_cb (PanelResizer *self,
                                double            offset_x,
                                double            offset_y,
                                GtkGestureDrag   *drag)
{
  g_assert (PANEL_IS_RESIZER (self));
  g_assert (GTK_IS_GESTURE_DRAG (drag));
}


GtkWidget *
panel_resizer_new (PanelDockPosition position)
{
  PanelResizer *self;

  self = g_object_new (PANEL_TYPE_RESIZER, NULL);
  self->position = position;
  self->handle = PANEL_HANDLE (panel_handle_new (position));
  gtk_widget_set_parent (GTK_WIDGET (self->handle), GTK_WIDGET (self));

  if (position == PANEL_DOCK_POSITION_CENTER)
    gtk_widget_hide (GTK_WIDGET (self->handle));

  return GTK_WIDGET (self);
}

static void
panel_resizer_measure (GtkWidget      *widget,
                            GtkOrientation  orientation,
                            int             for_size,
                            int            *minimum,
                            int            *natural,
                            int            *minimum_baseline,
                            int            *natural_baseline)
{
  PanelResizer *self = (PanelResizer *)widget;

  g_assert (PANEL_IS_RESIZER (self));

  *minimum_baseline = -1;
  *natural_baseline = -1;

  gtk_widget_measure (GTK_WIDGET (self->handle),
                      orientation,
                      for_size,
                      minimum, natural,
                      NULL, NULL);

  if (self->child != NULL)
    {
      int child_min, child_nat;

      gtk_widget_measure (self->child,
                          orientation,
                          for_size,
                          &child_min, &child_nat,
                          NULL, NULL);

      if (self->drag_position > child_min)
        child_nat = self->drag_position;

      *minimum += child_min;
      *natural += child_nat;
    }
}

static void
panel_resizer_size_allocate (GtkWidget *widget,
                                  int        width,
                                  int        height,
                                  int        baseline)
{
  PanelResizer *self = (PanelResizer *)widget;
  GtkOrientation orientation;
  GtkAllocation child_alloc;
  GtkAllocation handle_alloc;
  int handle_min, handle_nat;

  g_assert (PANEL_IS_RESIZER (self));

  if (self->position == PANEL_DOCK_POSITION_START ||
      self->position == PANEL_DOCK_POSITION_END)
    orientation = GTK_ORIENTATION_HORIZONTAL;
  else
    orientation = GTK_ORIENTATION_VERTICAL;

  gtk_widget_measure (GTK_WIDGET (self->handle),
                      orientation,
                      -1,
                      &handle_min, &handle_nat,
                      NULL, NULL);

  switch (self->position)
    {
    case PANEL_DOCK_POSITION_START:
      handle_alloc.x = width - handle_min;
      handle_alloc.width = handle_min;
      handle_alloc.y = 0;
      handle_alloc.height = height;
      child_alloc.x = 0;
      child_alloc.y = 0;
      child_alloc.width = width - handle_min;
      child_alloc.height = height;
      break;

    case PANEL_DOCK_POSITION_END:
      handle_alloc.x = 0;
      handle_alloc.width = handle_min;
      handle_alloc.y = 0;
      handle_alloc.height = height;
      child_alloc.x = handle_min;
      child_alloc.y = 0;
      child_alloc.width = width - handle_min;
      child_alloc.height = height;
      break;

    case PANEL_DOCK_POSITION_TOP:
      handle_alloc.x = 0;
      handle_alloc.width = width;
      handle_alloc.y = height - handle_min;
      handle_alloc.height = handle_min;
      child_alloc.x = 0;
      child_alloc.y = 0;
      child_alloc.width = width;
      child_alloc.height = height - handle_min;
      break;

    case PANEL_DOCK_POSITION_BOTTOM:
      handle_alloc.x = 0;
      handle_alloc.width = width;
      handle_alloc.y = 0;
      handle_alloc.height = handle_min;
      child_alloc.x = 0;
      child_alloc.y = handle_min;
      child_alloc.width = width;
      child_alloc.height = height - handle_min;
      break;

    case PANEL_DOCK_POSITION_CENTER:
    default:
      handle_alloc.x = 0;
      handle_alloc.width = 0;
      handle_alloc.y = 0;
      handle_alloc.height = 0;
      child_alloc.x = 0;
      child_alloc.y = 0;
      child_alloc.width = width;
      child_alloc.height = height;
      break;
    }

  if (gtk_widget_get_mapped (GTK_WIDGET (self->handle)))
    gtk_widget_size_allocate (GTK_WIDGET (self->handle), &handle_alloc, -1);

  if (self->child != NULL &&
      gtk_widget_get_mapped (self->child))
    gtk_widget_size_allocate (self->child, &child_alloc, -1);
}

static void
panel_resizer_dispose (GObject *object)
{
  PanelResizer *self = (PanelResizer *)object;

  g_clear_pointer ((GtkWidget **)&self->handle, gtk_widget_unparent);
  g_clear_pointer ((GtkWidget **)&self->child, gtk_widget_unparent);

  G_OBJECT_CLASS (panel_resizer_parent_class)->dispose (object);
}

static void
panel_resizer_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  PanelResizer *self = PANEL_RESIZER (object);

  switch (prop_id)
    {
    case PROP_CHILD:
      g_value_set_object (value, panel_resizer_get_child (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_resizer_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  PanelResizer *self = PANEL_RESIZER (object);

  switch (prop_id)
    {
    case PROP_CHILD:
      panel_resizer_set_child (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_resizer_class_init (PanelResizerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = panel_resizer_dispose;
  object_class->get_property = panel_resizer_get_property;
  object_class->set_property = panel_resizer_set_property;

  widget_class->measure = panel_resizer_measure;
  widget_class->size_allocate = panel_resizer_size_allocate;

  properties [PROP_CHILD] =
    g_param_spec_object ("child",
                         "Child",
                         "Child",
                         GTK_TYPE_WIDGET,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
panel_resizer_init (PanelResizer *self)
{
  GtkGesture *gesture;

  gesture = gtk_gesture_drag_new ();
  gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (gesture), GTK_PHASE_CAPTURE);
  g_signal_connect_object (gesture,
                           "drag-begin",
                           G_CALLBACK (panel_resizer_drag_begin_cb),
                           self,
                           G_CONNECT_SWAPPED);
  g_signal_connect_object (gesture,
                           "drag-update",
                           G_CALLBACK (panel_resizer_drag_update_cb),
                           self,
                           G_CONNECT_SWAPPED);
  g_signal_connect_object (gesture,
                           "drag-end",
                           G_CALLBACK (panel_resizer_drag_end_cb),
                           self,
                           G_CONNECT_SWAPPED);
  gtk_widget_add_controller (GTK_WIDGET (self), GTK_EVENT_CONTROLLER (gesture));
}

/**
 * panel_resizer_get_child:
 * @self: a #PanelResizer
 *
 * Gets the child widget of the resizer.
 *
 * Returns: (transfer none) (nullable): A #GtkWidget or %NULL
 */
GtkWidget *
panel_resizer_get_child (PanelResizer *self)
{
  g_return_val_if_fail (PANEL_IS_RESIZER (self), NULL);

  return self->child;
}

void
panel_resizer_set_child (PanelResizer *self,
                              GtkWidget        *child)
{
  g_return_if_fail (PANEL_IS_RESIZER (self));
  g_return_if_fail (!child || GTK_IS_WIDGET (child));

  if (child == self->child)
    return;

  g_clear_pointer (&self->child, gtk_widget_unparent);

  self->child = child;

  if (self->child != NULL)
    {
      gtk_widget_insert_before (self->child,
                                GTK_WIDGET (self),
                                GTK_WIDGET (self->handle));

      if (GTK_IS_ORIENTABLE (child))
        {
          if (self->position == PANEL_DOCK_POSITION_START ||
              self->position == PANEL_DOCK_POSITION_END)
            gtk_orientable_set_orientation (GTK_ORIENTABLE (child),
                                            GTK_ORIENTATION_VERTICAL);
          else
            gtk_orientable_set_orientation (GTK_ORIENTABLE (child),
                                            GTK_ORIENTATION_HORIZONTAL);
        }
    }

  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_CHILD]);
}

PanelDockPosition
panel_resizer_get_position (PanelResizer *self)
{
  g_return_val_if_fail (PANEL_IS_RESIZER (self), 0);

  return self->position;
}

void
panel_resizer_set_position (PanelResizer  *self,
                                 PanelDockPosition  position)
{
  g_return_if_fail (PANEL_IS_RESIZER (self));

  if (position != self->position)
    {
      GtkWidget *child = panel_resizer_get_child (self);

      self->position = position;

      if (child != NULL)
        {
          if (self->position == PANEL_DOCK_POSITION_START ||
              self->position == PANEL_DOCK_POSITION_END)
            gtk_orientable_set_orientation (GTK_ORIENTABLE (child),
                                            GTK_ORIENTATION_VERTICAL);
          else
            gtk_orientable_set_orientation (GTK_ORIENTABLE (child),
                                            GTK_ORIENTATION_HORIZONTAL);
        }

      gtk_widget_queue_resize (GTK_WIDGET (self));
    }
}
