/* words-segmenter.c
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


#include "words-segmenter.h"

G_DEFINE_INTERFACE (WordsSegmenter, words_segmenter, G_TYPE_OBJECT)

static void
words_segmenter_default_init (WordsSegmenterInterface *iface)
{
}

/**
 * words_segmenter_get_text:
 * @self: a #WordsSegmenter
 *
 * Retrieves the current text being segmented by @self.
 *
 * Returns: (transfer none)(nullable): the text set with words_segmenter_set_text()
 *
 * Since: 0.1.0
 */
const gchar*
words_segmenter_get_text (WordsSegmenter *self)
{
  g_return_val_if_fail (WORDS_IS_SEGMENTER (self), NULL);
  g_return_val_if_fail (WORDS_SEGMENTER_GET_IFACE (self)->get_text, NULL);

  return WORDS_SEGMENTER_GET_IFACE (self)->get_text (self);
}

/**
 * words_segmenter_set_text:
 * @self: a #WordsSegmenter
 * @text: (nullable): a text to segment
 *
 * Sets the text to be segmented by @self to @text.
 *
 * Since: 0.1.0
 */
void
words_segmenter_set_text (WordsSegmenter *self,
                          const gchar    *text)
{
  g_return_if_fail (WORDS_IS_SEGMENTER (self));
  g_return_if_fail (WORDS_SEGMENTER_GET_IFACE (self)->set_text);

  WORDS_SEGMENTER_GET_IFACE (self)->set_text (self, text);
}

/**
 * words_segmenter_segment:
 * @self: a #WordsSegmenter
 *
 * Segments the text. This is a blocking method and should
 * run inside a @GTask.
 *
 * Since: 0.1.0
 */
gboolean
words_segmenter_segment (WordsSegmenter  *self,
                         GError         **error)
{
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (WORDS_IS_SEGMENTER (self), FALSE);
  g_return_val_if_fail (WORDS_SEGMENTER_GET_IFACE (self)->segment, FALSE);

  return WORDS_SEGMENTER_GET_IFACE (self)->segment (self, error);
}

/**
 * words_segmenter_get_words:
 * @self: a #WordsSegmenter
 *
 * Retrieves the list of words segmented by words_segmenter_segment().
 *
 * Returns: (transfer none)(nullable): the %NULL-terminated list of words. Do
 * not free.
 *
 * Since: 0.1.0
 */
GStrv
words_segmenter_get_words (WordsSegmenter *self)
{
  g_return_val_if_fail (WORDS_IS_SEGMENTER (self), NULL);
  g_return_val_if_fail (WORDS_SEGMENTER_GET_IFACE (self)->get_words, NULL);

  return WORDS_SEGMENTER_GET_IFACE (self)->get_words (self);
}
