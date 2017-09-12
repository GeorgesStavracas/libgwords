/* gw-dictionary.h
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

#ifndef GW_DICTIONARY_H
#define GW_DICTIONARY_H

#if !defined(GW_INSIDE) && !defined(GW_COMPILATION)
# error "Only <gwords.h> can be included directly."
#endif

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GW_TYPE_DICTIONARY (gw_dictionary_get_type())

G_DECLARE_DERIVABLE_TYPE (GwDictionary, gw_dictionary, GW, DICTIONARY, GObject)

struct _GwDictionaryClass
{
  GObjectClass        parent;
};

GwDictionary*        gw_dictionary_new                           (void);

gboolean             gw_dictionary_contains                      (GwDictionary       *self,
                                                                  const gchar        *word,
                                                                  gsize               word_length);

void                 gw_dictionary_insert                        (GwDictionary       *self,
                                                                  const gchar        *word,
                                                                  gsize               word_length);

void                 gw_dictionary_remove                        (GwDictionary       *self,
                                                                  const gchar        *word,
                                                                  gsize               word_length);

void                 gw_dictionary_load_from_file                (GwDictionary        *self,
                                                                  const gchar         *file_path,
                                                                  const gchar         *delimiter,
                                                                  GAsyncReadyCallback  callback,
                                                                  GCancellable        *cancellable,
                                                                  gpointer             user_data);

gboolean             gw_dictionary_load_from_file_finish         (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             gw_dictionary_load_from_file_sync           (GwDictionary       *self,
                                                                  const gchar        *file_path,
                                                                  const gchar        *delimiter,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);


void                 gw_dictionary_load_from_resource            (GwDictionary        *self,
                                                                  const gchar         *resource_path,
                                                                  const gchar         *delimiter,
                                                                  GAsyncReadyCallback  callback,
                                                                  GCancellable        *cancellable,
                                                                  gpointer             user_data);

gboolean             gw_dictionary_load_from_resource_finish     (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             gw_dictionary_load_from_resource_sync       (GwDictionary       *self,
                                                                  const gchar        *resource_path,
                                                                  const gchar        *delimiter,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);


void                 gw_dictionary_load_from_gfile               (GwDictionary        *self,
                                                                  GFile               *file,
                                                                  const gchar         *delimiter,
                                                                  GAsyncReadyCallback  callback,
                                                                  GCancellable        *cancellable,
                                                                  gpointer             user_data);

gboolean             gw_dictionary_load_from_gfile_finish        (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             gw_dictionary_load_from_gfile_sync          (GwDictionary       *self,
                                                                  GFile              *file,
                                                                  const gchar        *delimiter,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* GW_DICTIONARY_H */

