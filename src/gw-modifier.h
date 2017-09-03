/* gw-modifier.h
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

#ifndef GW_MODIFIER_H
#define GW_MODIFIER_H

#include "gw-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GW_TYPE_MODIFIER (gw_modifier_get_type ())

G_DECLARE_INTERFACE (GwModifier, gw_modifier, GW, MODIFIER, GObject)

struct _GwModifierInterface
{
  GTypeInterface      parent;

  gboolean           (*modify_word)                              (GwModifier         *self,
                                                                  const GwString     *word,
                                                                  gsize               len,
                                                                  GwString          **new_word,
                                                                  gsize              *new_len);

  gpointer           _reserved[15];
};

gboolean             gw_modifier_modify_word                     (GwModifier         *self,
                                                                  const GwString     *word,
                                                                  gsize               len,
                                                                  GwString          **new_word,
                                                                  gsize              *new_len);

G_END_DECLS

#endif /* GW_MODIFIER_H */
