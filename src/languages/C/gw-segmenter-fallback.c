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

struct _GwSegmenterFallback
{
  GObject             parent;

  GwString           *text;
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
  self->text = gw_string_ref (text);
}

static gboolean
gw_segmenter_fallback_segment (GwSegmenter   *segmenter,
                               GCancellable  *cancellable,
                               GError       **error)
{
  /* TODO: implement gw_segmenter_fallback_segment */
  g_return_val_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter), FALSE);

  return TRUE;
}

static GStrv
gw_segmenter_fallback_get_words (GwSegmenter *segmenter)
{
  /* TODO: implement gw_segmenter_fallback_get_words */
  g_return_val_if_fail (GW_IS_SEGMENTER_FALLBACK (segmenter), NULL);

  return NULL;
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

GwSegmenterFallback *
gw_segmenter_fallback_new (void)
{
  return g_object_new (GW_TYPE_SEGMENTER_FALLBACK, NULL);
}

static void
gw_segmenter_fallback_finalize (GObject *object)
{
  GwSegmenterFallback *self = (GwSegmenterFallback *)object;

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
}
