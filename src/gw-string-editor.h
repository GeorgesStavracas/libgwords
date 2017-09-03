/* gw-string-editor.h
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

#ifndef GW_STRING_EDITOR_H
#define GW_STRING_EDITOR_H

#include "gw-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GW_TYPE_STRING_EDITOR (gw_string_editor_get_type())

typedef struct _GwStringEditor GwStringEditor;

GwStringEditor*      gw_string_editor_new                        (GwString           *str);

GwStringEditor*      gw_string_editor_copy                       (GwStringEditor     *self);

GwStringEditor*      gw_string_editor_ref                        (GwStringEditor     *self);

void                 gw_string_editor_unref                      (GwStringEditor     *self);

gboolean             gw_string_editor_insert                     (GwStringEditor     *self,
                                                                  GwString           *str,
                                                                  gsize               position);

void                 gw_string_editor_delete                     (GwStringEditor    *self,
                                                                  gsize              position,
                                                                  gsize              length);

void                 gw_string_editor_modify                     (GwStringEditor    *self,
                                                                  gsize              position,
                                                                  gsize              length,
                                                                  GwString          *new_text,
                                                                  gsize              text_len);

gsize                gw_string_editor_get_length                 (GwStringEditor    *self);

GwString*            gw_string_editor_to_string                  (GwStringEditor     *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GwStringEditor, gw_string_editor_unref)

G_END_DECLS

#endif /* GW_STRING_EDITOR_H */

