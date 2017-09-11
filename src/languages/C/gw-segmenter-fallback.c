/* gw-segmente-fallback.c
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

#include "../../gwords.h"

#include "gw-segmenter-fallback.h"

#include <string.h>

struct _GwSegmenterFallback
{
  GObject             parent;

  GwLanguage         *language;
};

static void          gw_segmenter_segmenter_iface_init           (GwSegmenterInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GwSegmenterFallback, gw_segmenter_fallback, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GW_TYPE_SEGMENTER, gw_segmenter_segmenter_iface_init))

enum
{
  PROP_0,
  PROP_LANGUAGE,
  N_PROPS
};


/*
 * GwSegmenter implemntation
 */

static gboolean
gw_segmenter_fallback_is_word_character (GwSegmenter *self,
                                         gunichar     character,
                                         gsize        index,
                                         GwString    *text,
                                         gsize        len)
{
  return character != '\0' && !g_unichar_ispunct (character) && !g_unichar_isspace (character);
}

static GStrv
gw_segmenter_fallback_segment (GwSegmenter   *segmenter,
                               GwString      *text,
                               GCancellable  *cancellable,
                               GError       **error)
{
  GPtrArray *words;
  gboolean was_word, is_word;
  gchar *aux, *start, *end;
  gsize len;

  g_return_val_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter), NULL);

  is_word = was_word = FALSE;
  start = end = NULL;

  words = g_ptr_array_new ();

  /* Don't attempt to segment NULL texts */
  if (!text)
    goto out;

  aux = text;
  len = strlen (text);

  /*
   * The fallback segmenter is pretty stupid, but it's the best we
   * can do without language-specific knowledge. It iterates through
   * the string and checks for 2 relevant cases, for example:
   *
   * A       B
   * ↓       ↓
   * I should break in six words.
   *
   * In (B), we reached the end of the word. In (A), we just entered
   * the word. Thus, (A) is the start and (B) is the end of the segment.
   */
  do
    {
      gunichar c;

      c = g_utf8_get_char (aux);

      was_word = is_word;
      is_word = gw_segmenter_is_word_character (segmenter, c, text - aux, text, len);

      if (!was_word && is_word)
        {
          start = aux;
        }
      else if (was_word && !is_word)
        {
          gsize length;
          gchar *substring;

          end = aux;
          length = end - start;
          substring = g_malloc (length + 1);

          memcpy (substring, start, length);
          substring[length] = '\0';

          g_ptr_array_add (words, substring);
        }

      aux = g_utf8_next_char (aux);
    }
  while (aux && *aux);

out:
  g_ptr_array_add (words, NULL);

  return (GStrv) g_ptr_array_free (words, FALSE);
}

static void
gw_segmenter_segmenter_iface_init (GwSegmenterInterface *iface)
{
  iface->is_word_character = gw_segmenter_fallback_is_word_character;
  iface->segment = gw_segmenter_fallback_segment;
}

/*
 * GObject overrides
 */

static void
gw_segmenter_fallback_dispose (GObject *object)
{
  GwSegmenterFallback *self = GW_SEGMENTER_FALLBACK (object);

  g_clear_object (&self->language);

  G_OBJECT_CLASS (gw_segmenter_fallback_parent_class)->dispose (object);
}

static void
gw_segmenter_fallback_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  GwSegmenterFallback *self = GW_SEGMENTER_FALLBACK (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_value_set_object (value, self->language);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_segmenter_fallback_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  GwSegmenterFallback *self = GW_SEGMENTER_FALLBACK (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      self->language = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_segmenter_fallback_class_init (GwSegmenterFallbackClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gw_segmenter_fallback_dispose;
  object_class->get_property = gw_segmenter_fallback_get_property;
  object_class->set_property = gw_segmenter_fallback_set_property;

  g_object_class_override_property (object_class, PROP_LANGUAGE, "language");
}

static void
gw_segmenter_fallback_init (GwSegmenterFallback *self)
{
}
