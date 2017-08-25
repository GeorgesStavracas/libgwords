/* gw-group.h
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

#ifndef GW_GROUP_H
#define GW_GROUP_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GW_TYPE_GROUP (gw_group_get_type())

typedef struct _GwGroup GwGroup;

GType                gw_group_get_type                           (void) G_GNUC_CONST;

GwGroup*             gw_group_new                                (const gchar        *group_id);

GwGroup*             gw_group_copy                               (GwGroup            *self);

GwGroup*             gw_group_ref                                (GwGroup            *self);

void                 gw_group_unref                              (GwGroup            *self);

void                 gw_group_insert_word                        (GwGroup            *self,
                                                                  const gchar        *word,
                                                                  gsize               word_length);

void                 gw_group_remove_word                        (GwGroup            *self,
                                                                  const gchar        *word,
                                                                  gsize               word_length);

gboolean             gw_group_contains_word                      (GwGroup            *self,
                                                                  const gchar        *word,
                                                                  gsize               word_length);

GStrv                gw_group_get_gw                             (GwGroup            *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GwGroup, gw_group_unref)

G_END_DECLS

#endif /* GW_GROUP_H */

