/* words-segmenter.h
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

#ifndef WORDS_SEGMENTER_H
#define WORDS_SEGMENTER_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define WORDS_TYPE_SEGMENTER (words_segmenter_get_type ())

G_DECLARE_INTERFACE (WordsSegmenter, words_segmenter, WORDS, SEGMENTER, GObject)

struct _WordsSegmenterInterface
{
  GTypeInterface parent;

  const gchar*       (*get_text)                                 (WordsSegmenter     *self);

  void               (*set_text)                                 (WordsSegmenter     *self,
                                                                  const gchar        *text);

  gboolean           (*segment)                                  (WordsSegmenter     *self,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

  GStrv              (*get_words)                                (WordsSegmenter     *self);
};

const gchar*         words_segmenter_get_text                    (WordsSegmenter     *self);

void                 words_segmenter_set_text                    (WordsSegmenter     *self,
                                                                  const gchar        *text);

void                 words_segmenter_segment                     (WordsSegmenter     *self,
                                                                  GAsyncReadyCallback callback,
                                                                  GCancellable       *cancellable,
                                                                  gpointer            user_data);

gboolean             words_segmenter_segment_finish              (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             words_segmenter_segment_sync                (WordsSegmenter     *self,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

GStrv                words_segmenter_get_words                   (WordsSegmenter     *self);

G_END_DECLS

#endif /* WORDS_SEGMENTER_H */
