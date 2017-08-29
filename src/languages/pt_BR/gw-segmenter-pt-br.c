/* gw-segmenter-pt-br.c
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

#include "gw-segmenter-pt-br.h"

#include <string.h>

struct _GwSegmenterPtBr
{
  GObject             parent;
};

static void          gw_segmenter_pt_br_iface_init               (GwSegmenterInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GwSegmenterPtBr, gw_segmenter_pt_br, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GW_TYPE_SEGMENTER, gw_segmenter_pt_br_iface_init))

/*
 * GwSegmenter implemntation
 */

static GStrv
gw_segmenter_pt_br_segment (GwSegmenter   *segmenter,
                            GwString      *text,
                            GCancellable  *cancellable,
                            GError       **error)
{
  GPtrArray *words;
  gboolean was_word, is_word;
  gchar *aux, *start, *end;

  /* TODO: add language-specific code to gw_segmenter_pt_br_segment */

  g_return_val_if_fail (GW_IS_SEGMENTER_PT_BR (segmenter), NULL);

  is_word = was_word = FALSE;
  start = end = NULL;

  words = g_ptr_array_new ();

  /* Don't attempt to segment NULL texts */
  if (!text)
    goto out;

  aux = text;

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
gw_segmenter_pt_br_iface_init (GwSegmenterInterface *iface)
{
  iface->segment = gw_segmenter_pt_br_segment;
}

static void
gw_segmenter_pt_br_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_segmenter_pt_br_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_segmenter_pt_br_class_init (GwSegmenterPtBrClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = gw_segmenter_pt_br_get_property;
  object_class->set_property = gw_segmenter_pt_br_set_property;
}

static void
gw_segmenter_pt_br_init (GwSegmenterPtBr *self)
{
}
