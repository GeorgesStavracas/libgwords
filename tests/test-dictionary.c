/* test-dictionary.c
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
dictionary_init (void)
{
  WordsDictionary *dict;

  dict = words_dictionary_new ();

  g_clear_object (&dict);
}

static void
dictionary_insert (void)
{
  WordsDictionary *dict;
  gint i;

  dict = words_dictionary_new ();

  for (i = 0; i < 100000; i++)
    {
      gchar *str = g_strdup_printf ("test%d", i);

      words_dictionary_insert (dict, str);

      g_free (str);
    }

  g_clear_object (&dict);
}

static void
dictionary_remove (void)
{
  WordsDictionary *dict;
  gint i;

  dict = words_dictionary_new ();

  for (i = 0; i < 100000; i++)
    {
      gchar *str = g_strdup_printf ("test%d", i);

      words_dictionary_insert (dict, str);

      g_free (str);
    }

  for (i = 0; i < 100000; i++)
    {
      gchar *str = g_strdup_printf ("test%d", i);

      words_dictionary_remove (dict, str);

      g_free (str);
    }

  g_clear_object (&dict);
}

static void
dictionary_load_file (void)
{
  WordsDictionary *dict;
  GError *error;
  gboolean success;

  dict = words_dictionary_new ();
  error = NULL;

  success = words_dictionary_load_from_file_sync (dict,
                                                  "english-words.txt",
                                                  NULL,
                                                  NULL,
                                                  &error);

  g_assert ((success && !error) || (!success && error));

  g_clear_object (&dict);
}

static void
file_loaded_cb (GObject      *source_object,
                GAsyncResult *res,
                gpointer      user_data)
{
  WordsDictionary *dict;
  GError *error;
  gboolean success;

  dict = WORDS_DICTIONARY (source_object);
  error = NULL;
  success = words_dictionary_load_from_file_finish (res, &error);

  g_assert ((success && !error) || (!success && error));

  g_main_loop_quit (user_data);

  g_clear_object (&dict);
}

static void
dictionary_load_file_async (void)
{
  WordsDictionary *dict;
  GMainLoop *mainloop;

  dict = words_dictionary_new ();
  mainloop = g_main_loop_new (NULL, FALSE);

  words_dictionary_load_from_file (dict,
                                   "english-words.txt",
                                   NULL,
                                   file_loaded_cb,
                                   NULL,
                                   mainloop);

  g_main_loop_run (mainloop);
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/dictionary/init", dictionary_init);
  g_test_add_func ("/dictionary/insert", dictionary_insert);
  g_test_add_func ("/dictionary/remove", dictionary_remove);
  g_test_add_func ("/dictionary/load_file", dictionary_load_file);
  g_test_add_func ("/dictionary/load_file_async", dictionary_load_file_async);

  return g_test_run ();
}
