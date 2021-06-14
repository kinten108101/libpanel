/* panel-save-delegate.c
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

#include "panel-save-delegate.h"

typedef struct
{
  char *subtitle;
  char *title;
  char *icon_name;
  GIcon *icon;
  double progress;
} PanelSaveDelegatePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (PanelSaveDelegate, panel_save_delegate, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_ICON,
  PROP_ICON_NAME,
  PROP_PROGRESS,
  PROP_SUBTITLE,
  PROP_TITLE,
  N_PROPS
};

enum {
  SAVE,
  N_SIGNALS
};

static GParamSpec *properties [N_PROPS];
static guint signals [N_SIGNALS];

/**
 * panel_save_delegate_new:
 *
 * Create a new #PanelSaveDelegate.
 *
 * Returns: (transfer full): a newly created #PanelSaveDelegate
 */
PanelSaveDelegate *
panel_save_delegate_new (void)
{
  return g_object_new (PANEL_TYPE_SAVE_DELEGATE, NULL);
}

static void
panel_save_delegate_real_save_async (PanelSaveDelegate   *self,
                                     GCancellable        *cancellable,
                                     GAsyncReadyCallback  callback,
                                     gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_assert (PANEL_IS_SAVE_DELEGATE (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, panel_save_delegate_real_save_async);
  g_signal_emit (self, signals [SAVE], 0, task);
}

static gboolean
panel_save_delegate_real_save_finish (PanelSaveDelegate  *self,
                                      GAsyncResult       *result,
                                      GError            **error)
{
  g_assert (PANEL_IS_SAVE_DELEGATE (self));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
panel_save_delegate_real_save (PanelSaveDelegate *self,
                               GTask             *task)
{
  g_assert (PANEL_IS_SAVE_DELEGATE (self));
  g_assert (G_IS_TASK (task));

  if (!g_task_return_error_if_cancelled (task))
    g_task_return_new_error (task,
                             G_IO_ERROR,
                             G_IO_ERROR_NOT_SUPPORTED,
                             "Saving is not supported");
}

static void
panel_save_delegate_finalize (GObject *object)
{
  PanelSaveDelegate *self = (PanelSaveDelegate *)object;
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_clear_pointer (&priv->title, g_free);
  g_clear_pointer (&priv->subtitle, g_free);
  g_clear_pointer (&priv->icon_name, g_free);
  g_clear_object (&priv->icon);

  G_OBJECT_CLASS (panel_save_delegate_parent_class)->finalize (object);
}

static void
panel_save_delegate_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  PanelSaveDelegate *self = PANEL_SAVE_DELEGATE (object);

  switch (prop_id)
    {
    case PROP_ICON:
      g_value_set_object (value, panel_save_delegate_get_icon (self));
      break;

    case PROP_ICON_NAME:
      g_value_set_string (value, panel_save_delegate_get_icon_name (self));
      break;

    case PROP_PROGRESS:
      g_value_set_double (value, panel_save_delegate_get_progress (self));
      break;

    case PROP_SUBTITLE:
      g_value_set_string (value, panel_save_delegate_get_subtitle (self));
      break;

    case PROP_TITLE:
      g_value_set_string (value, panel_save_delegate_get_title (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_save_delegate_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  PanelSaveDelegate *self = PANEL_SAVE_DELEGATE (object);

  switch (prop_id)
    {
    case PROP_ICON:
      panel_save_delegate_set_icon (self, g_value_get_object (value));
      break;

    case PROP_ICON_NAME:
      panel_save_delegate_set_icon_name (self, g_value_get_string (value));
      break;

    case PROP_PROGRESS:
      panel_save_delegate_set_progress (self, g_value_get_double (value));
      break;

    case PROP_SUBTITLE:
      panel_save_delegate_set_subtitle (self, g_value_get_string (value));
      break;

    case PROP_TITLE:
      panel_save_delegate_set_title (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
panel_save_delegate_class_init (PanelSaveDelegateClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = panel_save_delegate_finalize;
  object_class->get_property = panel_save_delegate_get_property;
  object_class->set_property = panel_save_delegate_set_property;

  klass->save_async = panel_save_delegate_real_save_async;
  klass->save_finish = panel_save_delegate_real_save_finish;
  klass->save = panel_save_delegate_real_save;

  /**
   * PanelSaveDelegate:icon:
   *
   * The "icon" property contains a #GIcon that describes the save
   * operation. Generally, this should be the symbolic icon of the
   * document class you are saving.
   *
   * Alternatively, you can use #PanelSaveDelegate:icon-name for a
   * named icon.
   */
  properties [PROP_ICON] =
    g_param_spec_object ("icon",
                         "Icon",
                         "A GIcon representing the save operation",
                         G_TYPE_ICON,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  /**
   * PanelSaveDelegate:icon-name:
   *
   * The "icon-name" property contains the name of an icon to use when
   * showing information about the save operation in UI such as a save
   * dialog.
   *
   * You can also use #PanelSaveDelegate:icon to set a #GIcon instead of
   * an icon name.
   */
  properties [PROP_ICON_NAME] =
    g_param_spec_string ("icon-name",
                         "Icon Name",
                         "Icon Name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  /**
   * PanelSaveDelegate:progress:
   *
   * The "progress" property contains progress between 0.0 and 1.0 and
   * should be updated by the delegate implementation as saving progresses.
   */
  properties [PROP_PROGRESS] =
    g_param_spec_double ("progress",
                         "Progress",
                         "The progress of the save operation",
                         0.0, 1.0, 0.0,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  /**
   * PanelSaveDelegate:title:
   *
   * The "title" property contains the title of the document being saved.
   * Generally, this should be the base name of the document such as
   * `file.txt`.
   */
  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The title of the document or documents to save",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  /**
   * PanelSaveDelegate:subtitle:
   *
   * The "subtitle" property contains additional information that may
   * not make sense to put in the title. This might include the directory
   * that the file will be saved within.
   */
  properties [PROP_SUBTITLE] =
    g_param_spec_string ("subtitle",
                         "Subtitle",
                         "The subtitle of the document or documents to save",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  /**
   * PanelSaveDelegate::save:
   * @self: a #PanelSaveDelegate
   * @task: a #GTask
   *
   * This signal can be used when subclassing #PanelSaveDelegate is not
   * possible or cumbersome. The default implementation of
   * #PanelSaveDelegateClass.save_async() will emit this signal to allow
   * the consumer to implement asynchronous save in a flexible manner.
   *
   * The caller is expected to complete @task with a boolean when the
   * save operation has completed.
   */
  signals [SAVE] = g_signal_new ("save",
                                 G_TYPE_FROM_CLASS (klass),
                                 G_SIGNAL_RUN_LAST,
                                 G_STRUCT_OFFSET (PanelSaveDelegateClass, save),
                                 g_signal_accumulator_first_wins, NULL,
                                 NULL,
                                 G_TYPE_NONE, 1, G_TYPE_TASK);
}

static void
panel_save_delegate_init (PanelSaveDelegate *self)
{
}

const char *
panel_save_delegate_get_icon_name (PanelSaveDelegate *self)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);
  const char * const *names;

  g_return_val_if_fail (PANEL_IS_SAVE_DELEGATE (self), NULL);

  if (priv->icon_name == NULL &&
      G_IS_THEMED_ICON (priv->icon) &&
      (names = g_themed_icon_get_names (G_THEMED_ICON (priv->icon))))
    return names[0];

  return priv->icon_name;
}

void
panel_save_delegate_set_icon_name (PanelSaveDelegate *self,
                                   const char        *icon_name)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_if_fail (PANEL_IS_SAVE_DELEGATE (self));

  if (g_strcmp0 (priv->icon_name, icon_name) != 0)
    {
      g_free (priv->icon_name);
      priv->icon_name = g_strdup (icon_name);

      if (g_set_object (&priv->icon, NULL))
        g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ICON]);

      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ICON_NAME]);
    }
}

const char *
panel_save_delegate_get_subtitle (PanelSaveDelegate *self)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_val_if_fail (PANEL_IS_SAVE_DELEGATE (self), NULL);

  return priv->subtitle;
}

void
panel_save_delegate_set_subtitle (PanelSaveDelegate *self,
                                  const char        *subtitle)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_if_fail (PANEL_IS_SAVE_DELEGATE (self));

  if (g_strcmp0 (priv->subtitle, subtitle) != 0)
    {
      g_free (priv->subtitle);
      priv->subtitle = g_strdup (subtitle);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SUBTITLE]);
    }
}

const char *
panel_save_delegate_get_title (PanelSaveDelegate *self)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_val_if_fail (PANEL_IS_SAVE_DELEGATE (self), NULL);

  return priv->title;
}

void
panel_save_delegate_set_title (PanelSaveDelegate *self,
                               const char        *title)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_if_fail (PANEL_IS_SAVE_DELEGATE (self));

  if (g_strcmp0 (priv->title, title) != 0)
    {
      g_free (priv->title);
      priv->title = g_strdup (title);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TITLE]);
    }
}

double
panel_save_delegate_get_progress (PanelSaveDelegate *self)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_val_if_fail (PANEL_IS_SAVE_DELEGATE (self), 0);

  return priv->progress;
}

void
panel_save_delegate_set_progress (PanelSaveDelegate *self,
                                  double             progress)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_if_fail (PANEL_IS_SAVE_DELEGATE (self));

  if (progress != priv->progress)
    {
      priv->progress = progress;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_PROGRESS]);
    }
}

void
panel_save_delegate_save_async (PanelSaveDelegate   *self,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  g_return_if_fail (PANEL_IS_SAVE_DELEGATE (self));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  PANEL_SAVE_DELEGATE_GET_CLASS (self)->save_async (self, cancellable, callback, user_data);
}

gboolean
panel_save_delegate_save_finish (PanelSaveDelegate  *self,
                                 GAsyncResult       *result,
                                 GError            **error)
{
  g_return_val_if_fail (PANEL_IS_SAVE_DELEGATE (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  return PANEL_SAVE_DELEGATE_GET_CLASS (self)->save_finish (self, result, error);
}

/**
 * panel_save_delegate_get_icon:
 * @self: a #PanelSaveDelegate
 *
 * Gets the #GIcon for the save delegate, or %NULL if unset.
 *
 * Returns: (transfer none) (nullable): a #GIcon or %NULL
 */
GIcon *
panel_save_delegate_get_icon (PanelSaveDelegate *self)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_val_if_fail (PANEL_IS_SAVE_DELEGATE (self), NULL);

  return priv->icon;
}

void
panel_save_delegate_set_icon (PanelSaveDelegate *self,
                              GIcon             *icon)
{
  PanelSaveDelegatePrivate *priv = panel_save_delegate_get_instance_private (self);

  g_return_if_fail (PANEL_IS_SAVE_DELEGATE (self));

  if (g_set_object (&priv->icon, icon))
    {
      g_clear_pointer (&priv->icon_name, g_free);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ICON]);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ICON_NAME]);
    }
}
