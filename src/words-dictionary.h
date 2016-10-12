/* words-dictionary.h
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

#ifndef WORDS_DICTIONARY_H
#define WORDS_DICTIONARY_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define WORDS_TYPE_DICTIONARY (words_dictionary_get_type())

G_DECLARE_DERIVABLE_TYPE (WordsDictionary, words_dictionary, WORDS, DICTIONARY, GObject)

struct _WordsDictionaryClass
{
  GObjectClass        parent;
};

WordsDictionary*     words_dictionary_new                        (void);

gboolean             words_dictionary_contains                   (WordsDictionary    *self,
                                                                  const gchar        *word);

void                 words_dictionary_insert                     (WordsDictionary    *self,
                                                                  const gchar        *word);

void                 words_dictionary_remove                     (WordsDictionary    *self,
                                                                  const gchar        *word);

void                 words_dictionary_load_from_file             (WordsDictionary     *self,
                                                                  const gchar         *file_path,
                                                                  const gchar         *delimiter,
                                                                  GAsyncReadyCallback  callback,
                                                                  GCancellable        *cancellable,
                                                                  gpointer             user_data);

gboolean             words_dictionary_load_from_file_finish      (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             words_dictionary_load_from_file_sync        (WordsDictionary    *self,
                                                                  const gchar        *file_path,
                                                                  const gchar        *delimiter,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);


void                 words_dictionary_load_from_resource         (WordsDictionary     *self,
                                                                  const gchar         *resource_path,
                                                                  const gchar         *delimiter,
                                                                  GAsyncReadyCallback  callback,
                                                                  GCancellable        *cancellable,
                                                                  gpointer             user_data);

gboolean             words_dictionary_load_from_resource_finish  (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             words_dictionary_load_from_resource_sync    (WordsDictionary    *self,
                                                                  const gchar        *resource_path,
                                                                  const gchar        *delimiter,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);


void                 words_dictionary_load_from_gfile            (WordsDictionary     *self,
                                                                  GFile               *file,
                                                                  const gchar         *delimiter,
                                                                  GAsyncReadyCallback  callback,
                                                                  GCancellable        *cancellable,
                                                                  gpointer             user_data);

gboolean             words_dictionary_load_from_gfile_finish     (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             words_dictionary_load_from_gfile_sync       (WordsDictionary    *self,
                                                                  GFile              *file,
                                                                  const gchar        *delimiter,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* WORDS_DICTIONARY_H */

