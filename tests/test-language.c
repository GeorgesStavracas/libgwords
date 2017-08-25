/* test-language.c
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

/**************************************************************************************************/

static void
language_loaded_cb (GObject      *source_object,
                    GAsyncResult *res,
                    gpointer      user_data)
{
  GwLanguage *language;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  mainloop = user_data;
  language = gw_language_new_finish (res, &error);

  g_message ("Language: %s", gw_language_get_language_code (language));

  g_assert_nonnull (language);
  g_assert (!error);

  g_main_loop_quit (mainloop);

  g_clear_object (&language);
}

static void
get_language_async (void)
{
  GMainLoop *mainloop;

  mainloop = g_main_loop_new (NULL, FALSE);

  gw_language_new ("en_GB",
                   NULL,
                   language_loaded_cb,
                   mainloop);

  g_main_loop_run (mainloop);
}


/**************************************************************************************************/

static void
get_language (void)
{
  GwLanguage *language;
  GError *error;

  error = NULL;
  language = gw_language_new_sync ("en_GB", NULL, &error);

  g_assert_nonnull (language);
  g_assert (!error);

  g_clear_object (&language);
}


/**************************************************************************************************/

static void
system_language_loaded_cb (GObject      *source_object,
                           GAsyncResult *res,
                           gpointer      user_data)
{
  GwLanguage *language;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  mainloop = user_data;
  language = gw_language_new_finish (res, &error);

  g_message ("Language: %s", gw_language_get_language_code (language));

  g_assert_nonnull (language);
  g_assert (!error);

  g_main_loop_quit (mainloop);

  g_clear_object (&language);
}

static void
get_system_language_async (void)
{
  GMainLoop *mainloop;

  mainloop = g_main_loop_new (NULL, FALSE);

  gw_language_new (NULL,
                   NULL,
                   system_language_loaded_cb,
                   mainloop);

  g_main_loop_run (mainloop);
}


/**************************************************************************************************/

static void
get_system_language (void)
{
  GwLanguage *language;
  GError *error;

  error = NULL;
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_nonnull (language);
  g_assert (!error);

  g_clear_object (&language);
}


/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  gw_init ();

  g_test_add_func ("/language/get_language", get_language);
  g_test_add_func ("/language/get_language_async", get_language_async);
  g_test_add_func ("/language/get_system_language", get_system_language);
  g_test_add_func ("/language/get_system_language_async", get_system_language_async);

  return g_test_run ();
}
