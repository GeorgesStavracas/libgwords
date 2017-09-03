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

#include "gwords.h"

#include <time.h>

/**************************************************************************************************/

static void
radix_tree_init (void)
{
  GwRadixTree *tree;

  tree = gw_radix_tree_new ();

  g_clear_object (&tree);
}

/**************************************************************************************************/

static void
radix_tree_low_entries (void)
{
  GwRadixTree *tree;

  tree = gw_radix_tree_new ();

  gw_radix_tree_insert (tree, "Albums", -1, NULL);
  gw_radix_tree_insert (tree, "Music", -1, NULL);
  gw_radix_tree_insert (tree, "Test", -1, NULL);
  gw_radix_tree_insert (tree, "Firefox", -1, NULL);
  gw_radix_tree_insert (tree, "Yay", -1, NULL);

  g_clear_object (&tree);
}

/**************************************************************************************************/

static void
radix_tree_n_entries (guint n)
{
  GwRadixTree *tree;
  gdouble sum;
  gchar key[20] = { '\0', };
  gint i, n_iterations;

  tree = gw_radix_tree_new ();
  sum = 0;
  n_iterations = 1000000 / n;

  for (i = 0; i < n_iterations; i++)
    {
      clock_t begin, end;
      gdouble spent;
      gint j;

      begin = clock ();

      for (j = 0; j < n; j++)
        {
          g_snprintf (key, 20, "test%d", j);
          gw_radix_tree_insert (tree, key, -1, NULL);
        }

      gw_radix_tree_clear (tree);

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
    1000000
  };

  g_print ("\n");

  for (i = 0; i < G_N_ELEMENTS (entries); i++)
    radix_tree_n_entries (entries[i]);
}

/**************************************************************************************************/

static gboolean
iter_cb (const gchar *key,
         gsize        key_length,
         gpointer     value,
         gpointer     user_data)
{
  const gchar* const *entries = user_data;

  g_assert (g_strv_contains (entries, key));

  return GW_RADIX_TREE_ITER_CONTINUE;
}

static void
radix_tree_iteration (void)
{
  GwRadixTree *tree;
  gint i;

  const gchar* entries[] = {
    "firefox",
    "planet",
    "ore monogatari",
    "ergo proxy",
    "space cowboy",
    "test",
    NULL
  };

  tree = gw_radix_tree_new ();

  for (i = 0; entries[i]; i++)
    gw_radix_tree_insert (tree, entries[i], -1, NULL);

  gw_radix_tree_iter (tree, iter_cb, entries);

  g_clear_object (&tree);
}

/**************************************************************************************************/

static gboolean
utf8_iter_cb (const gchar *key,
              gsize        key_length,
              gpointer     value,
              gpointer     user_data)
{
  const gchar* const *entries = user_data;

  g_assert (g_strv_contains (entries, key));

  return GW_RADIX_TREE_ITER_CONTINUE;
}

static void
radix_tree_utf8 (void)
{
  GwRadixTree *tree;
  gint i;

  const gchar* entries[] = {
    "Ħểłłỡ שׂờŕłđ",
    "子犬",
    "中文",
    "Tiếng Việt, còn gọi",
    "В'єтнамська мова",
    NULL
  };

  tree = gw_radix_tree_new ();

  for (i = 0; entries[i]; i++)
    gw_radix_tree_insert (tree, entries[i], -1, NULL);

  gw_radix_tree_iter (tree, utf8_iter_cb, entries);

  g_clear_object (&tree);
}

/**************************************************************************************************/

static void
radix_tree_get_keys (void)
{
  GwRadixTree *tree;
  GStrv keys;
  gint i;

  const gchar* entries[] = {
    "Ħểłłỡ שׂờŕłđ",
    "子犬",
    "中文",
    "Tiếng Việt, còn gọi",
    "В'єтнамська мова"
  };

  tree = gw_radix_tree_new ();

  for (i = 0; i < G_N_ELEMENTS (entries); i++)
    gw_radix_tree_insert (tree, entries[i], -1, GINT_TO_POINTER (i));

  keys = gw_radix_tree_get_keys (tree);

  g_assert_nonnull (keys);
  g_assert_cmpuint (g_strv_length (keys), ==, G_N_ELEMENTS (entries));

  g_clear_pointer (&keys, g_strfreev);
  g_clear_object (&tree);
}

/**************************************************************************************************/

static void
radix_tree_get_values (void)
{
  g_autoptr (GPtrArray) values;
  GwRadixTree *tree;
  gint i;

  const gchar* entries[] = {
    "Ħểłłỡ שׂờŕłđ",
    "子犬",
    "中文",
    "Tiếng Việt, còn gọi",
    "В'єтнамська мова"
  };

  tree = gw_radix_tree_new ();

  for (i = 0; i < G_N_ELEMENTS (entries); i++)
    gw_radix_tree_insert (tree, entries[i], -1, GINT_TO_POINTER (i));

  values = gw_radix_tree_get_values (tree);

  g_assert_nonnull (values);
  g_assert_cmpuint (values->len, ==, G_N_ELEMENTS (entries));

  g_clear_object (&tree);
}

/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/radix-tree/init", radix_tree_init);
  g_test_add_func ("/radix-tree/low_entries", radix_tree_low_entries);
  g_test_add_func ("/radix-tree/many_entries", radix_tree_many_entries);
  g_test_add_func ("/radix-tree/iteration", radix_tree_iteration);
  g_test_add_func ("/radix-tree/utf8", radix_tree_utf8);
  g_test_add_func ("/radix-tree/get_keys", radix_tree_get_keys);
  g_test_add_func ("/radix-tree/get_values", radix_tree_get_values);

  return g_test_run ();
}
