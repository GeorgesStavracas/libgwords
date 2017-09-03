/* gw-document-fallback.c
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

#include "gw-document-fallback.h"

struct _GwDocumentFallback
{
  GwDocument          parent;
};

G_DEFINE_TYPE (GwDocumentFallback, gw_document_fallback, GW_TYPE_DOCUMENT)

/*
 * GwDocument overrides
 */
static GwString*
gw_document_fallback_realize (GwDocument    *self,
                              GCancellable  *cancellable,
                              GError       **error)
{
  /* TODO: implement me */
  return NULL;
}


/*
 * GObject overrides
 */

static void
gw_document_fallback_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_document_fallback_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_document_fallback_class_init (GwDocumentFallbackClass *klass)
{
  GwDocumentClass *document_class = GW_DOCUMENT_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = gw_document_fallback_get_property;
  object_class->set_property = gw_document_fallback_set_property;

  document_class->realize = gw_document_fallback_realize;
}

static void
gw_document_fallback_init (GwDocumentFallback *self)
{
}
