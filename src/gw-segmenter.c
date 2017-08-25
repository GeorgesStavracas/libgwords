/* gw-segmenter.c
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "gw-segmenter.h"

G_DEFINE_INTERFACE (GwSegmenter, gw_segmenter, G_TYPE_OBJECT)

static void
segment_in_thread_cb (GTask        *task,
                      gpointer      source_object,
                      gpointer      task_data,
                      GCancellable *cancellable)
{
  GError *local_error = NULL;

  gw_segmenter_segment_sync (source_object, cancellable, &local_error);

  if (local_error)
    {
      g_task_return_error (task, local_error);
      return;
    }

  g_task_return_boolean (task, TRUE);
}

static void
gw_segmenter_default_init (GwSegmenterInterface *iface)
{
}

/**
 * gw_segmenter_get_text:
 * @self: a #GwSegmenter
 *
 * Retrieves the current text being segmented by @self.
 *
 * Returns: (transfer none)(nullable): a #GwString
 *
 * Since: 0.1.0
 */
GwString*
gw_segmenter_get_text (GwSegmenter *self)
{
  g_return_val_if_fail (GW_IS_SEGMENTER (self), NULL);
  g_return_val_if_fail (GW_SEGMENTER_GET_IFACE (self)->get_text, NULL);

  return GW_SEGMENTER_GET_IFACE (self)->get_text (self);
}

/**
 * gw_segmenter_set_text:
 * @self: a #GwSegmenter
 * @text: (nullable): a #GwString
 *
 * Sets the text to be segmented by @self to @text.
 *
 * Since: 0.1.0
 */
void
gw_segmenter_set_text (GwSegmenter *self,
                       GwString    *text)
{
  g_return_if_fail (GW_IS_SEGMENTER (self));
  g_return_if_fail (GW_SEGMENTER_GET_IFACE (self)->set_text);

  GW_SEGMENTER_GET_IFACE (self)->set_text (self, text);
}

/**
 * gw_segmenter_segment:
 * @self: a #GwSegmenter
 * @callback: (): callback to call when the operation is done
 * @cancellable: (nullable): cancellable to cancel the operation
 * @user_data:
 *
 * Segments the text stored in @self asynchronously.
 *
 * Since: 0.1.0
 */
void
gw_segmenter_segment (GwSegmenter         *self,
                      GAsyncReadyCallback  callback,
                      GCancellable        *cancellable,
                      gpointer             user_data)
{
  GTask *task;

  g_return_if_fail (GW_IS_SEGMENTER (self));
  g_return_if_fail (gw_segmenter_get_text (self) != NULL);

  task = g_task_new (self, cancellable, callback, user_data);

  g_task_run_in_thread (task, segment_in_thread_cb);
}

/**
 * gw_segmenter_segment_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes the segmentation operation started by gw_segmenter_segment().
 *
 * Returns: %TRUE for success or %FALSE for failure.
 *
 * Since: 0.1.0
 */
gboolean
gw_segmenter_segment_finish (GAsyncResult  *result,
                             GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * gw_segmenter_segment:
 * @self: a #GwSegmenter
 * @cancellable: (nullable): a #GCancellable
 * @error: (nullable): return location for the error
 *
 * Segments the text. This is a blocking method and should
 * run inside a @GTask.
 *
 * See gw_segmenter_segment() for the asynchronous version.
 *
 * Returns: %TRUE on success or %FALSE on failure.
 *
 * Since: 0.1.0
 */
gboolean
gw_segmenter_segment_sync (GwSegmenter   *self,
                           GCancellable  *cancellable,
                           GError       **error)
{
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (GW_IS_SEGMENTER (self), FALSE);
  g_return_val_if_fail (GW_SEGMENTER_GET_IFACE (self)->segment, FALSE);

  return GW_SEGMENTER_GET_IFACE (self)->segment (self, cancellable, error);
}

/**
 * gw_segmenter_get_words:
 * @self: a #GwSegmenter
 *
 * Retrieves the list of words segmented by gw_segmenter_segment().
 *
 * Returns: (transfer none)(nullable): the %NULL-terminated list of gw. Do
 * not free.
 *
 * Since: 0.1.0
 */
GStrv
gw_segmenter_get_words (GwSegmenter *self)
{
  g_return_val_if_fail (GW_IS_SEGMENTER (self), NULL);
  g_return_val_if_fail (GW_SEGMENTER_GET_IFACE (self)->get_words, NULL);

  return GW_SEGMENTER_GET_IFACE (self)->get_words (self);
}
