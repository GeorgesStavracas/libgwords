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

#include "gw-types.h"

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define GW_TYPE_DICTIONARY (gw_dictionary_get_type())

G_DECLARE_DERIVABLE_TYPE (GwDictionary, gw_dictionary, GW, DICTIONARY, GObject)

struct _GwDictionaryClass
{
  GObjectClass        parent;

  gboolean           (*load)                                     (GwDictionary       *self);


  gboolean           (*unload)                                   (GwDictionary       *self);

  GwWord*            (*lookup)                                   (GwDictionary       *self,
                                                                  GwGrammarClass      word_class,
                                                                  GwString           *word,
                                                                  gsize               word_len);

};

GwLanguage*          gw_dictionary_get_language                  (GwDictionary       *self);

GwWord*              gw_dictionary_lookup                        (GwDictionary       *self,
                                                                  GwGrammarClass      word_class,
                                                                  GwString           *word,
                                                                  gsize               word_length);

G_END_DECLS

#endif /* GW_DICTIONARY_H */

