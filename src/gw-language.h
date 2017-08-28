/* gw-language.h
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

#ifndef GW_LANGUAGE_H
#define GW_LANGUAGE_H

#include "gw-types.h"

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

/**
 * GwLanguageError:
 * @GW_LANGUAGE_ERROR_INVALID: indicated an invalid language
 *
 * Errors that #GwLanguage can generate.
 */
typedef enum
{
  GW_LANGUAGE_ERROR_INVALID,
} GwLanguageError;

#define GW_LANGUAGE_ERROR (gw_language_error_quark ())

#define GW_TYPE_LANGUAGE (gw_language_get_type())

G_DECLARE_FINAL_TYPE (GwLanguage, gw_language, GW, LANGUAGE, GObject)

GQuark               gw_language_error_quark                     (void);

void                 gw_language_new                             (const gchar        *language,
                                                                  GCancellable       *cancellable,
                                                                  GAsyncReadyCallback callback,
                                                                  gpointer            user_data);

GwLanguage*          gw_language_new_finish                      (GAsyncResult       *result,
                                                                  GError            **error);

GwLanguage*          gw_language_new_sync                        (const gchar        *language,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

const gchar*         gw_language_get_language_code               (GwLanguage         *self);

GwSegmenter*         gw_language_get_segmenter                   (GwLanguage         *self);

GwDictionary*        gw_language_get_dictionary                  (GwLanguage         *self);

G_END_DECLS

#endif /* GW_LANGUAGE_H */

