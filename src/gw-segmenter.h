/* gw-segmenter.h
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

#ifndef GW_SEGMENTER_H
#define GW_SEGMENTER_H

#include "gw-types.h"

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GW_TYPE_SEGMENTER (gw_segmenter_get_type ())

G_DECLARE_INTERFACE (GwSegmenter, gw_segmenter, GW, SEGMENTER, GObject)

struct _GwSegmenterInterface
{
  GTypeInterface parent;

  gboolean           (*is_word_character)                        (GwSegmenter        *self,
                                                                  gunichar            character,
                                                                  gsize               index,
                                                                  GwString           *text,
                                                                  gsize               len);

  GStrv              (*segment)                                  (GwSegmenter        *self,
                                                                  GwString           *text,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);
};

GwLanguage*          gw_segmenter_get_language                   (GwSegmenter        *self);

gboolean             gw_segmenter_is_word_character              (GwSegmenter        *self,
                                                                  gunichar            character,
                                                                  gsize               index,
                                                                  GwString           *text,
                                                                  gsize               len);

void                 gw_segmenter_segment                        (GwSegmenter        *self,
                                                                  GwString           *text,
                                                                  GAsyncReadyCallback callback,
                                                                  GCancellable       *cancellable,
                                                                  gpointer            user_data);

GStrv                gw_segmenter_segment_finish                 (GAsyncResult       *result,
                                                                  GError            **error);

GStrv                gw_segmenter_segment_sync                   (GwSegmenter        *self,
                                                                  GwString           *text,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* GW_SEGMENTER_H */
