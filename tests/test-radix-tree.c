/* test-radix-tree.c
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

#include "words.h"

static void
radix_tree_init (void)
{
  WordsRadixTree *tree;

  tree = words_radix_tree_new ();

  g_clear_object (&tree);
}

static void
radix_tree_low_entries (void)
{
  WordsRadixTree *tree;

  tree = words_radix_tree_new ();

  words_radix_tree_insert (tree, "Albums", -1, NULL);
  words_radix_tree_insert (tree, "Music", -1, NULL);
  words_radix_tree_insert (tree, "Test", -1, NULL);
  words_radix_tree_insert (tree, "Firefox", -1, NULL);
  words_radix_tree_insert (tree, "Yay", -1, NULL);

  g_clear_object (&tree);
}

static void
radix_tree_many_entries (void)
{
  WordsRadixTree *tree;
  gint i;

  tree = words_radix_tree_new ();

  for (i = 0; i < 1000000; i++)
    {
      gchar *key;

      key = g_strdup_printf ("Test%d", i);
      words_radix_tree_insert (tree, key, -1, NULL);

      g_free (key);
    }

  g_clear_object (&tree);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/radix-tree/init", radix_tree_init);
  g_test_add_func ("/radix-tree/low_entries", radix_tree_low_entries);
  g_test_add_func ("/radix-tree/many_entries", radix_tree_many_entries);

  return g_test_run ();
}
