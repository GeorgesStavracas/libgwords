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

#include "test-config.h"
#include "gwords.h"

/*****************************************************************************/

static void
dictionary_init (void)
{
  GwDictionary *dict;

  dict = gw_dictionary_new ();

  g_clear_object (&dict);
}

/*****************************************************************************/

static void
dictionary_insert (void)
{
  GwDictionary *dict;
  gint i;

  dict = gw_dictionary_new ();

  for (i = 0; i < 100000; i++)
    {
      gchar *str = g_strdup_printf ("test%d", i);

      gw_dictionary_insert (dict, str, -1);

      g_free (str);
    }

  g_clear_object (&dict);
}

/*****************************************************************************/

static void
dictionary_remove (void)
{
  GwDictionary *dict;
  gint i;

  dict = gw_dictionary_new ();

  for (i = 0; i < 100000; i++)
    {
      gchar *str = g_strdup_printf ("test%d", i);

      gw_dictionary_insert (dict, str, -1);

      g_free (str);
    }

  for (i = 0; i < 100000; i++)
    {
      gchar *str = g_strdup_printf ("test%d", i);

      gw_dictionary_remove (dict, str, -1);

      g_free (str);
    }

  g_clear_object (&dict);
}

/*****************************************************************************/

static void
dictionary_load_file_ok (void)
{
  GwDictionary *dict;
  GError *error;
  gboolean success;

  dict = gw_dictionary_new ();
  error = NULL;

  success = gw_dictionary_load_from_file_sync (dict,
                                               TESTDIR"/english-words.txt",
                                               NULL,
                                               NULL,
                                               &error);

  g_assert (success && !error);

  g_clear_object (&dict);
}

/*****************************************************************************/

static void
dictionary_load_file_error (void)
{
  GwDictionary *dict;
  GError *error;
  gboolean success;

  dict = gw_dictionary_new ();
  error = NULL;

  success = gw_dictionary_load_from_file_sync (dict,
                                               "i_shouldnt_exist",
                                               NULL,
                                               NULL,
                                               &error);

  g_assert (!success && error);

  g_clear_object (&dict);
}

/*****************************************************************************/

static void
file_loaded_cb (GObject      *source_object,
                GAsyncResult *res,
                gpointer      user_data)
{
  GwDictionary *dict;
  GError *error;
  gboolean success;

  dict = GW_DICTIONARY (source_object);
  error = NULL;
  success = gw_dictionary_load_from_file_finish (res, &error);

  g_assert (success && !error);

  g_main_loop_quit (user_data);

  g_clear_object (&dict);
}

static void
dictionary_load_file_ok_async (void)
{
  GwDictionary *dict;
  GMainLoop *mainloop;

  dict = gw_dictionary_new ();
  mainloop = g_main_loop_new (NULL, FALSE);

  gw_dictionary_load_from_file (dict,
                                TESTDIR"/english-words.txt",
                                NULL,
                                file_loaded_cb,
                                NULL,
                                mainloop);

  g_main_loop_run (mainloop);
}

/*****************************************************************************/

static void
file_loaded_error_cb (GObject      *source_object,
                      GAsyncResult *res,
                      gpointer      user_data)
{
  GwDictionary *dict;
  GError *error;
  gboolean success;

  dict = GW_DICTIONARY (source_object);
  error = NULL;
  success = gw_dictionary_load_from_file_finish (res, &error);

  g_assert (!success && error);

  g_main_loop_quit (user_data);

  g_clear_object (&dict);
}

static void
dictionary_load_file_error_async (void)
{
  GwDictionary *dict;
  GMainLoop *mainloop;

  dict = gw_dictionary_new ();
  mainloop = g_main_loop_new (NULL, FALSE);

  gw_dictionary_load_from_file (dict,
                                "i_shouldnt_exist",
                                NULL,
                                file_loaded_error_cb,
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
  g_test_add_func ("/dictionary/load_file_ok", dictionary_load_file_ok);
  g_test_add_func ("/dictionary/load_file_error", dictionary_load_file_error);
  g_test_add_func ("/dictionary/load_file_ok_async", dictionary_load_file_ok_async);
  g_test_add_func ("/dictionary/load_file_error_async", dictionary_load_file_error_async);

  return g_test_run ();
}
