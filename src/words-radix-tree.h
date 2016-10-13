/* words-radix-tree.h
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

#ifndef WORDS_RADIX_TREE_H
#define WORDS_RADIX_TREE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define WORDS_TYPE_RADIX_TREE (words_radix_tree_get_type())

G_DECLARE_DERIVABLE_TYPE (WordsRadixTree, words_radix_tree, WORDS, RADIX_TREE, GObject)

struct _WordsRadixTreeClass
{
  GObjectClass        parent;
};

#define WORDS_RADIX_TREE_ITER_STOP     TRUE
#define WORDS_RADIX_TREE_ITER_CONTINUE FALSE

/**
 * Returns %TRUE to stop, %FALSE to continue.
 */
typedef gboolean     (*RadixTreeCb)                               (const gchar       *key,
                                                                   gpointer           value,
                                                                   gpointer           user_data);

WordsRadixTree*      words_radix_tree_new                        (void);

WordsRadixTree*      words_radix_tree_new_with_free_func         (GDestroyNotify      destroy_func);

gboolean             words_radix_tree_contains                   (WordsRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length);

gpointer             words_radix_tree_lookup                     (WordsRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length,
                                                                  gboolean           *found);

gboolean             words_radix_tree_insert                     (WordsRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length,
                                                                  gpointer            value);

void                 words_radix_tree_clear                      (WordsRadixTree     *tree);

gint                 words_radix_tree_get_size                   (WordsRadixTree     *tree);

gboolean             words_radix_tree_iter                       (WordsRadixTree     *tree,
                                                                  RadixTreeCb         callback,
                                                                  gpointer            user_data);

GStrv                words_radix_tree_get_keys                   (WordsRadixTree     *tree);

GPtrArray*           words_radix_tree_get_values                 (WordsRadixTree     *tree);

void                 words_radix_tree_remove                     (WordsRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length);

void                 words_radix_tree_steal                      (WordsRadixTree     *tree,
                                                                  const gchar        *key,
                                                                  gsize               key_length);

G_END_DECLS

#endif /* WORDS_RADIX_TREE_H */

