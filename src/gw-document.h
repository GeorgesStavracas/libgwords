/* gw-document.h
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

#ifndef GW_DOCUMENT_H
#define GW_DOCUMENT_H

#if !defined(GW_INSIDE) && !defined(GW_COMPILATION)
# error "Only <gwords.h> can be included directly."
#endif

#include "gw-types.h"

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GW_TYPE_DOCUMENT (gw_document_get_type())

G_DECLARE_DERIVABLE_TYPE (GwDocument, gw_document, GW, DOCUMENT, GObject)

struct _GwDocumentClass
{
  GObjectClass        parent;

  GwString*          (*realize)                                  (GwDocument         *self,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

  gpointer            _reserved[20];
};

GwLanguage*          gw_document_get_language                    (GwDocument         *self);

GwString*            gw_document_get_text                        (GwDocument         *self);

void                 gw_document_modify                          (GwDocument         *self,
                                                                  GwModifier         *modifier,
                                                                  GCancellable       *cancellable,
                                                                  GAsyncReadyCallback callback,
                                                                  gpointer            user_data);

gboolean             gw_document_modify_finish                   (GAsyncResult       *result,
                                                                  GError            **error);

gboolean             gw_document_modify_sync                     (GwDocument         *self,
                                                                  GwModifier         *modifier,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

void                 gw_document_realize                         (GwDocument         *self,
                                                                  GCancellable       *cancellable,
                                                                  GAsyncReadyCallback callback,
                                                                  gpointer            user_data);

GwString*            gw_document_realize_finish                  (GAsyncResult       *result,
                                                                  GError            **error);

GwString*            gw_document_realize_sync                    (GwDocument         *self,
                                                                  GCancellable       *cancellable,
                                                                  GError            **error);

G_END_DECLS

#endif /* GW_DOCUMENT_H */

