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

#include "gw-language.h"
#include "gw-segmenter.h"
#include "gw-string.h"

G_DEFINE_INTERFACE (GwSegmenter, gw_segmenter, G_TYPE_OBJECT)

static void
segment_in_thread_cb (GTask        *task,
                      gpointer      source_object,
                      gpointer      task_data,
                      GCancellable *cancellable)
{
  GwString *text;
  GError *local_error;
  GStrv words;

  text = task_data;
  local_error = NULL;

  words = gw_segmenter_segment_sync (source_object, text, cancellable, &local_error);

  if (local_error)
    {
      g_task_return_error (task, local_error);
      return;
    }

  g_task_return_pointer (task, words, NULL);
}

static void
gw_segmenter_default_init (GwSegmenterInterface *iface)
{
  /**
   * GwSegmenter:language:
   *
   * The language of the segmenter, used to have access to language-specific
   * information such as word dictionaries and conjugators.
   *
   * Since: 0.1
   */
  g_object_interface_install_property (iface,
                                       g_param_spec_object ("language",
                                                            "Language",
                                                            "The language of the segmenter",
                                                            GW_TYPE_LANGUAGE,
                                                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));
}

GwLanguage*
gw_segmenter_get_language (GwSegmenter *self)
{
  GwLanguage *language;

  g_return_val_if_fail (GW_IS_SEGMENTER (self), NULL);

  g_object_get (self, "language", &language, NULL);

  /*
   * g_object_get() increases the refcount, but this function is
   * transfer-none, so undo the refcount increase
   */
  g_object_unref (language);

  return language;
}

/**
 * gw_segmenter_is_word_character:
 * @self: a #GwSegmenter
 * @character: the current character
 * @index: the index of @character in @text
 * @text: a #GwString
 * @len: length of @text
 *
 * Checks if the value of @character at @index is part of a word
 * or not. This function is used determine the word boundaries
 * in such a way that is reusable by other methods.
 *
 * Returns: %TRUE if @character at @index is part of a word, %FALSE
 *          if it's outside a word.
 *
 * Since: 0.1
 */
gboolean
gw_segmenter_is_word_character (GwSegmenter *self,
                                gunichar     character,
                                gsize        index,
                                GwString    *text,
                                gsize        len)
{
  g_return_val_if_fail (GW_IS_SEGMENTER (self), FALSE);
  g_return_val_if_fail (GW_SEGMENTER_GET_IFACE (self)->is_word_character, FALSE);

  return GW_SEGMENTER_GET_IFACE (self)->is_word_character (self,
                                                           character,
                                                           index,
                                                           text,
                                                           len);
}

/**
 * gw_segmenter_segment:
 * @self: a #GwSegmenter
 * @text: the text to segment
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
                      GwString            *text,
                      GAsyncReadyCallback  callback,
                      GCancellable        *cancellable,
                      gpointer             user_data)
{
  GTask *task;

  g_return_if_fail (GW_IS_SEGMENTER (self));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, gw_string_ref (text), (GDestroyNotify) gw_string_unref);

  g_task_run_in_thread (task, segment_in_thread_cb);
}

/**
 * gw_segmenter_segment_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes the segmentation operation started by gw_segmenter_segment().
 *
 * Returns: (transfer full)(nullable): a #GStrv with the segmented words.
 *
 * Since: 0.1.0
 */
GStrv
gw_segmenter_segment_finish (GAsyncResult  *result,
                             GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * gw_segmenter_segment:
 * @self: a #GwSegmenter
 * @text: the text to segment
 * @cancellable: (nullable): a #GCancellable
 * @error: (nullable): return location for the error
 *
 * Segments the text. This is a blocking method and should
 * run inside a @GTask.
 *
 * See gw_segmenter_segment() for the asynchronous version.
 *
 * Returns: (transfer full)(nullable): a #GStrv with the segmented words.
 *
 * Since: 0.1.0
 */
GStrv
gw_segmenter_segment_sync (GwSegmenter   *self,
                           GwString      *text,
                           GCancellable  *cancellable,
                           GError       **error)
{
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (GW_IS_SEGMENTER (self), FALSE);
  g_return_val_if_fail (GW_SEGMENTER_GET_IFACE (self)->segment, FALSE);

  return GW_SEGMENTER_GET_IFACE (self)->segment (self, text, cancellable, error);
}

