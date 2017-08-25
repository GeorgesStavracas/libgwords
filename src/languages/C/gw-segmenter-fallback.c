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

  GwString           *text;
  GPtrArray          *words;
};

static void          gw_segmenter_segmenter_iface_init           (GwSegmenterInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GwSegmenterFallback, gw_segmenter_fallback, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GW_TYPE_SEGMENTER, gw_segmenter_segmenter_iface_init))

/*
 * GwSegmenter implemntation
 */

static GwString*
gw_segmenter_fallback_get_text (GwSegmenter *segmenter)
{
  GwSegmenterFallback *self;

  g_return_val_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter), NULL);

  self = GW_SEGMENTER_FALLBACK (segmenter);

  return self->text;
}

static void
gw_segmenter_fallback_set_text (GwSegmenter *segmenter,
                                GwString    *text)
{
  GwSegmenterFallback *self;

  g_return_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter));

  self = GW_SEGMENTER_FALLBACK (segmenter);

  g_clear_pointer (&self->text, gw_string_unref);
  self->text = text ? gw_string_ref (text) : NULL;

  g_ptr_array_remove_range (self->words, 0, self->words->len);
}

static gboolean
gw_segmenter_fallback_segment (GwSegmenter   *segmenter,
                               GCancellable  *cancellable,
                               GError       **error)
{
  GwSegmenterFallback *self;
  gboolean was_word, is_word;
  gchar *aux, *start, *end;

  g_return_val_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter), FALSE);

  self = GW_SEGMENTER_FALLBACK (segmenter);
  is_word = was_word = FALSE;
  start = end = NULL;

  /* Always cleanup */
  g_ptr_array_remove_range (self->words, 0, self->words->len);

  /* Don't attempt to segment NULL texts */
  if (!self->text)
    goto out;

  aux = self->text;

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

      if (g_unichar_ispunct (c) || g_unichar_isspace (c))
        is_word = FALSE;
      else
        is_word = TRUE;

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

          g_ptr_array_add (self->words, substring);
        }

      aux = g_utf8_next_char (aux);
    }
  while (aux && *aux);

out:
  g_ptr_array_add (self->words, NULL);

  return TRUE;
}

static GStrv
gw_segmenter_fallback_get_words (GwSegmenter *segmenter)
{
  GwSegmenterFallback *self;

  g_return_val_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter), NULL);

  self = GW_SEGMENTER_FALLBACK (segmenter);

  return (GStrv) self->words->pdata;
}

static void
gw_segmenter_segmenter_iface_init (GwSegmenterInterface *iface)
{
  iface->get_text = gw_segmenter_fallback_get_text;
  iface->set_text = gw_segmenter_fallback_set_text;
  iface->segment = gw_segmenter_fallback_segment;
  iface->get_words = gw_segmenter_fallback_get_words;
}

/*
 * GObject overrides
 */

static void
gw_segmenter_fallback_finalize (GObject *object)
{
  GwSegmenterFallback *self = (GwSegmenterFallback *)object;

  g_clear_pointer (&self->words, g_ptr_array_unref);
  g_clear_pointer (&self->text, gw_string_unref);

  G_OBJECT_CLASS (gw_segmenter_fallback_parent_class)->finalize (object);
}

static void
gw_segmenter_fallback_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_segmenter_fallback_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_segmenter_fallback_class_init (GwSegmenterFallbackClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gw_segmenter_fallback_finalize;
  object_class->get_property = gw_segmenter_fallback_get_property;
  object_class->set_property = gw_segmenter_fallback_set_property;
}

static void
gw_segmenter_fallback_init (GwSegmenterFallback *self)
{
  self->words = g_ptr_array_new_with_free_func (g_free);
}
