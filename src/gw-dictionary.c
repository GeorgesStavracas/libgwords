/* gw-dictionary.c
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

#include "gw-dictionary.h"
#include "gw-language.h"

/**
 * SECTION:gw-dictionary
 * @short_title:a dictionary of words
 * @title:GwDictionary
 * @stability:Unstable
 *
 * The #GwDictionary class is a wrapper around a #GwRadixTree specialized in
 * dealing with words.
 */

typedef struct
{
  GwLanguage         *language;
} GwDictionaryPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GwDictionary, gw_dictionary, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_LANGUAGE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

static void
gw_dictionary_finalize (GObject *object)
{
  GwDictionary *self = (GwDictionary *)object;
  GwDictionaryPrivate *priv = gw_dictionary_get_instance_private (self);

  g_clear_object (&priv->language);

  G_OBJECT_CLASS (gw_dictionary_parent_class)->finalize (object);
}

static void
gw_dictionary_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GwDictionary *self = GW_DICTIONARY (object);
  GwDictionaryPrivate *priv = gw_dictionary_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_value_set_object (value, priv->language);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_dictionary_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GwDictionary *self = GW_DICTIONARY (object);
  GwDictionaryPrivate *priv = gw_dictionary_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      priv->language = g_value_dup_object (value);
      g_object_notify_by_pspec (object, properties[PROP_LANGUAGE]);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_dictionary_class_init (GwDictionaryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gw_dictionary_finalize;
  object_class->get_property = gw_dictionary_get_property;
  object_class->set_property = gw_dictionary_set_property;

  properties[PROP_LANGUAGE] = g_param_spec_object ("language",
                                                   "Language of the dictionary",
                                                   "The language of the dictionary",
                                                   GW_TYPE_LANGUAGE,
                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gw_dictionary_init (GwDictionary *self)
{
}

gboolean
gw_dictionary_load (GwDictionary *self)
{
  g_return_val_if_fail (GW_IS_DICTIONARY (self), FALSE);

  return GW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self))->load (self);
}

gboolean
gw_dictionary_unload (GwDictionary *self)
{
  g_return_val_if_fail (GW_IS_DICTIONARY (self), FALSE);

  return GW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self))->unload (self);
}

/**
 * gw_dictionary_lookup:
 * @self: a #GwDictionary
 * @word_class: the grammar class of the word being searched
 * @word: the word (in any form) to be looked up
 * @word_length: the length of @word, or -1
 *
 * Looks up @word from @self. This created a #GwWord instance,
 * which is returned - or %NULL if the word was not found.
 *
 * Returns: (transfer full)(nullable): a #GwWord.
 *
 * Since: 0.1
 */
GwWord*
gw_dictionary_lookup (GwDictionary   *self,
                      GwGrammarClass  word_class,
                      GwString       *word,
                      gsize           word_length)
{
  g_return_val_if_fail (GW_IS_DICTIONARY (self), NULL);
  g_return_val_if_fail (GW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self))->lookup, NULL);

  return GW_DICTIONARY_CLASS (G_OBJECT_GET_CLASS (self))->lookup (self,
                                                                  word_class,
                                                                  word,
                                                                  word_length);
}
