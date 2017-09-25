/* gw-dictionary-pt-br.c
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

#include "gw-dictionary-pt-br.h"

struct _GwDictionaryPtBr
{
  GwDictionary        parent;
};

G_DEFINE_TYPE (GwDictionaryPtBr, gw_dictionary_pt_br, GW_TYPE_DICTIONARY)


/*
 * GwDictionary overrides
 */

static gboolean
gw_dictionary_pt_br_load (GwDictionary *dictionary)
{
  return TRUE;
}

static GwWord*
gw_dictionary_pt_br_lookup (GwDictionary   *dictionary,
                            GwGrammarClass  word_class,
                            GwString       *word,
                            gsize           word_len)
{
  return NULL;
}

static gboolean
gw_dictionary_pt_br_unload (GwDictionary *dictionary)
{
  return TRUE;
}

static void
gw_dictionary_pt_br_class_init (GwDictionaryPtBrClass *klass)
{
  GwDictionaryClass *dictionary_class = GW_DICTIONARY_CLASS (klass);

  dictionary_class->load = gw_dictionary_pt_br_load;
  dictionary_class->lookup = gw_dictionary_pt_br_lookup;
  dictionary_class->unload = gw_dictionary_pt_br_unload;
}

static void
gw_dictionary_pt_br_init (GwDictionaryPtBr *self)
{
}
