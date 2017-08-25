/* gw-radix-tree.h
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

#ifndef GW_RADIX_TREE_H
#define GW_RADIX_TREE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GW_TYPE_RADIX_TREE (gw_radix_tree_get_type())

G_DECLARE_DERIVABLE_TYPE (GwRadixTree, gw_radix_tree, GW, RADIX_TREE, GObject)

struct _GwRadixTreeClass
{
  GObjectClass        parent;
};

#define GW_RADIX_TREE_ITER_STOP     TRUE
#define GW_RADIX_TREE_ITER_CONTINUE FALSE

/**
 * Returns %TRUE to stop, %FALSE to continue.
 */
typedef gboolean     (*RadixTreeCb)                               (const gchar       *key,
                                                                   gsize              key_length,
                                                                   gpointer           value,
                                                                   gpointer           user_data);

GwRadixTree*         gw_radix_tree_new                           (void);

GwRadixTree*         gw_radix_tree_new_with_free_func            (GDestroyNotify      destroy_func);

gboolean             gw_radix_tree_contains                      (GwRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length);

gpointer             gw_radix_tree_lookup                        (GwRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length,
                                                                  gboolean           *found);

gboolean             gw_radix_tree_insert                        (GwRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length,
                                                                  gpointer            value);

void                 gw_radix_tree_clear                         (GwRadixTree     *tree);

gint                 gw_radix_tree_get_size                      (GwRadixTree     *tree);

gboolean             gw_radix_tree_iter                          (GwRadixTree     *tree,
                                                                  RadixTreeCb         callback,
                                                                  gpointer            user_data);

GStrv                gw_radix_tree_get_keys                      (GwRadixTree     *tree);

GPtrArray*           gw_radix_tree_get_values                    (GwRadixTree     *tree);

void                 gw_radix_tree_remove                        (GwRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length);

void                 gw_radix_tree_steal                         (GwRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length);

G_END_DECLS

#endif /* GW_RADIX_TREE_H */

