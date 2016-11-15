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

#include <time.h>

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
radix_tree_n_entries (guint n)
{
  WordsRadixTree *tree;
  gdouble sum;
  gchar key[20] = { '\0', };
  gint i, n_iterations;

  tree = words_radix_tree_new ();
  sum = 0;
  n_iterations = 10000000 / n;

  for (i = 0; i < n_iterations; i++)
    {
      clock_t begin, end;
      gdouble spent;
      gint j;

      begin = clock ();

      for (j = 0; j < n; j++)
        {
          g_snprintf (key, 20, "test%d", j);
          words_radix_tree_insert (tree, key, -1, NULL);
        }

      words_radix_tree_clear (tree);

      end = clock ();
      spent = (gdouble) (end - begin) / CLOCKS_PER_SEC;

      sum += spent;
    }

  g_clear_object (&tree);

  sum = sum / n_iterations;

  g_print ("%8d elements - average execution time: %lf (%d iterations)\n", n, sum, n_iterations);
}

static void
radix_tree_many_entries (void)
{
  guint i;
  guint entries[] = {
    10,
    1000,
    100000,
    1000000,
    10000000
  };

  g_print ("\n");

  for (i = 0; i < G_N_ELEMENTS (entries); i++)
    radix_tree_n_entries (entries[i]);
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
