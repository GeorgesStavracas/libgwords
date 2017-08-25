/* words-language.h
 *
 * Copyright (C) 2017 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WORDS_LANGUAGE_H
#define WORDS_LANGUAGE_H

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * WordsLanguageError:
 * @WORDS_LANGUAGE_ERROR_INVALID: indicated an invalid language
 *
 * Errors that #WordsLanguage can generate.
 */
typedef enum
{
  WORDS_LANGUAGE_ERROR_INVALID,
} WordsLanguageError;

#define WODS_LANGUAGE_ERROR (words_language_error_quark ())

#define WORDS_TYPE_LANGUAGE (words_language_get_type())

G_DECLARE_FINAL_TYPE (WordsLanguage, words_language, WORDS, LANGUAGE, GObject)

GQuark               words_language_error_quark                  (void);

void                 words_language_new                          (const gchar        *language,
                                                                  GCancellable       *cancellable,
                                                                  GAsyncReadyCallback callback,
                                                                  gpointer            user_data);

WordsLanguage*       words_language_new_finish                   (GAsyncResult       *result,
                                                                  GError            **error);

WordsLanguage*       words_language_new_sync                     (const gchar        *language,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* WORDS_LANGUAGE_H */

