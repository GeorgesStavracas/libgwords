/* test-string-editor.c
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


#include "gwords.h"

#include <time.h>

#define BENCHMARK_ITERATIONS 1000000
#define BENCHMARK_N_STRINGS  100

/**************************************************************************************************/

static void
string_editor_new_empty (void)
{
  g_autoptr (GwStringEditor) editor;

  editor = gw_string_editor_new (NULL);

  g_assert_nonnull (editor);
}

/**************************************************************************************************/

static void
string_editor_new (void)
{
  g_autoptr (GwStringEditor) editor;
  GwString *string;

  string = gw_string_new_static ("BLABLABLABLABLA");
  editor = gw_string_editor_new (string);

  g_assert_nonnull (editor);
}

/**************************************************************************************************/

static void
string_editor_to_string (void)
{
  g_autoptr (GwStringEditor) editor;
  g_autoptr (GwString) new_string;
  GwString *string;

  string = gw_string_new_static ("I should be like this!");
  editor = gw_string_editor_new (string);

  g_assert_nonnull (editor);

  new_string = gw_string_editor_to_string (editor);

  g_assert_nonnull (new_string);
  g_assert_cmpstr (new_string, ==, "I should be like this!");
}

/**************************************************************************************************/

static void
string_editor_insert (void)
{
  g_autoptr (GwStringEditor) editor;
  g_autoptr (GwString) new_string;
  GwString *string;

  string = gw_string_new_static ("a");
  editor = gw_string_editor_new (string);

  g_assert_nonnull (editor);

  gw_string_editor_insert (editor, "b", 1);
  gw_string_editor_insert (editor, "c", 0);

  new_string = gw_string_editor_to_string (editor);

  g_assert_nonnull (new_string);
  g_assert_cmpstr (new_string, ==, "cab");
}

/**************************************************************************************************/

static void
string_editor_delete (void)
{
  g_autoptr (GwStringEditor) editor;
  g_autoptr (GwString) new_string;
  GwString *string;

  string = gw_string_new_static ("abcdefghij");
  editor = gw_string_editor_new (string);

  g_assert_nonnull (editor);

  gw_string_editor_delete (editor, 0, 3);
  gw_string_editor_delete (editor, 2, 3);

  new_string = gw_string_editor_to_string (editor);

  g_assert_nonnull (new_string);
  g_assert_cmpstr (new_string, ==, "deij");
}

/**************************************************************************************************/

static void
string_editor_modify (void)
{
  g_autoptr (GwStringEditor) editor;
  g_autoptr (GwString) new_string;
  GwString *string;

  string = gw_string_new_static ("aaaaaa");
  editor = gw_string_editor_new (string);

  g_assert_nonnull (editor);

  gw_string_editor_modify (editor, 3, 6, "bbb", -1);

  /* Keep size */
  new_string = gw_string_editor_to_string (editor);

  g_assert_nonnull (new_string);
  g_assert_cmpstr (new_string, ==, "aaabbb");

  /* Reduce size */
  gw_string_editor_modify (editor, 3, 6, "c", -1);

  new_string = gw_string_editor_to_string (editor);

  g_assert_nonnull (new_string);
  g_assert_cmpstr (new_string, ==, "aaac");

  /* Increase size */
  gw_string_editor_modify (editor, 3, 6, "bbb", -1);

  new_string = gw_string_editor_to_string (editor);

  g_assert_nonnull (new_string);
  g_assert_cmpstr (new_string, ==, "aaabbb");
}

/**************************************************************************************************/

static const gchar ascii_chars[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()[]{}<>?,./";

static void
random_ascii_string (GwString *buffer,
                     gsize     len)
{
  guint i;

  for (i = 0; i < len - 1; i++)
    buffer[i] = ascii_chars[g_random_int_range (0, G_N_ELEMENTS (ascii_chars) - 1)];

  buffer[len - 1] = '\0';
}

static void
string_editor_benchmark (void)
{
  g_autoptr (GRand) rand;
  gulong *random_values;
  GwString *strings[BENCHMARK_N_STRINGS];
  guint i;

  /* Set a constant seed to reproduce the same numbers */
  rand = g_rand_new_with_seed (132779);

  /* Initialize random strings */
  for (i = 0; i < BENCHMARK_N_STRINGS; i++)
    {
      gsize len;

      len = 1 + g_rand_int (rand) % 2;

      strings[i] = gw_string_new_with_size (len + 1);

      random_ascii_string (strings[i], len + 1);
    }

  /* We should pick the same random sequence each benchmark run */
  random_values = g_malloc (sizeof (gulong) * BENCHMARK_ITERATIONS);

  for (i = 0; i < BENCHMARK_ITERATIONS; i++)
    random_values[i] = g_rand_int (rand);

  for (i = 0; i < 5; i++)
    {
      g_autoptr (GwStringEditor) editor;
      clock_t begin, end;
      gdouble elapsed_time;
      guint iteration;
      gsize len;

      editor = gw_string_editor_new (NULL);
      g_assert_nonnull (editor);

      begin = clock ();

      for (iteration = 0; iteration < BENCHMARK_ITERATIONS; iteration++)
        {
          len = gw_string_editor_get_length (editor);

          if (len == 0 || iteration % 20 > 0)
            {
              // Insert (Inserts are way more common in practice than deletes.)
              GwString *str;

              str = strings[iteration % BENCHMARK_N_STRINGS];

              gw_string_editor_insert (editor, str, random_values[iteration] % (len + 1));
            }
          else
            {
              gsize pos, del_len;

              pos = random_values[iteration] % len;
              del_len = MIN (len - pos, 1 + (~random_values[i]) % 53);

              gw_string_editor_delete (editor, pos, del_len);
            }
        }

      end = clock ();

      elapsed_time = (gdouble) (end - begin) / CLOCKS_PER_SEC;

      g_debug ("Did %d iterations in %lfms: %ld iterations per second",
               BENCHMARK_ITERATIONS,
               elapsed_time * 1000,
               (glong) (BENCHMARK_ITERATIONS / elapsed_time));

      g_debug ("Final string length: %li", len);
    }

  for (i = 0; i < BENCHMARK_N_STRINGS; i++)
    gw_string_unref (strings[i]);
}

/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/string_editor/new", string_editor_new);
  g_test_add_func ("/string_editor/new_empty", string_editor_new_empty);
  g_test_add_func ("/string_editor/to_string", string_editor_to_string);
  g_test_add_func ("/string_editor/insert", string_editor_insert);
  g_test_add_func ("/string_editor/delete", string_editor_delete);
  g_test_add_func ("/string_editor/modify", string_editor_modify);
  g_test_add_func ("/string_editor/benchmark", string_editor_benchmark);

  return g_test_run ();
}
