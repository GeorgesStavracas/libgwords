/* test-document.c
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


#include "../src/gwords.h"
#include "stub-modifier.h"

/*****************************************************************************/

static void
document_new (void)
{
  g_autoptr (GwLanguage) language;
  g_autoptr (GwDocument) doc;
  GError *error;

  error = NULL;
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (language);

  doc = gw_language_create_document_sync (language, NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (doc);

  g_assert (gw_document_get_language (doc) == language);
}

/*****************************************************************************/

static void
document_created_cb (GObject      *source_object,
                     GAsyncResult *res,
                     gpointer      user_data)
{
  g_autoptr (GwDocument) doc;
  GMainLoop *mainloop;
  GError *error;

  mainloop = user_data;
  error = NULL;
  doc = gw_language_create_document_finish (res, &error);

  g_assert_no_error (error);
  g_assert_nonnull (doc);

  g_assert (gw_document_get_language (doc) == GW_LANGUAGE (source_object));

  g_main_loop_quit (mainloop);
}

static void
document_new_async (void)
{
  g_autoptr (GwLanguage) language;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  mainloop = g_main_loop_new (NULL, FALSE);
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (language);

  gw_language_create_document (language,
                               NULL,
                               NULL,
                               document_created_cb,
                               mainloop);

  g_main_loop_run (mainloop);
}

/*****************************************************************************/

static void
document_new_from_string (void)
{
  g_autoptr (GwLanguage) language;
  g_autoptr (GwDocument) doc;
  g_autoptr (GwString) string;
  GError *error;

  error = NULL;
  string = gw_string_new ("Questions of science... science and progress.");
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (language);

  doc = gw_language_create_document_sync (language, string, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (doc);

  g_assert (gw_document_get_language (doc) == language);
  g_assert_cmpstr (gw_document_get_text (doc), ==, "Questions of science... science and progress.");
}

/*****************************************************************************/

static void
document_created_from_string_cb (GObject      *source_object,
                                 GAsyncResult *res,
                                 gpointer      user_data)
{
  g_autoptr (GwDocument) doc;
  GMainLoop *mainloop;
  GError *error;

  mainloop = user_data;
  error = NULL;
  doc = gw_language_create_document_finish (res, &error);

  g_assert_no_error (error);
  g_assert_nonnull (doc);

  g_assert (gw_document_get_language (doc) == GW_LANGUAGE (source_object));
  g_assert_cmpstr (gw_document_get_text (doc), ==, "Questions of science... science and progress.");

  g_main_loop_quit (mainloop);
}

static void
document_new_from_string_async (void)
{
  g_autoptr (GwLanguage) language;
  g_autoptr (GwString) string;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  string = gw_string_new ("Questions of science... science and progress.");
  mainloop = g_main_loop_new (NULL, FALSE);
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (language);

  gw_language_create_document (language,
                               gw_string_ref (string),
                               NULL,
                               document_created_from_string_cb,
                               mainloop);

  g_main_loop_run (mainloop);
}

/*****************************************************************************/

static void
document_modify (void)
{
  g_autoptr (GwLanguage) language;
  g_autoptr (GwDocument) doc;
  g_autoptr (GwModifier) mod;
  g_autoptr (GwString) string;
  GError *error;
  gboolean result;

  error = NULL;
  string = gw_string_new ("I am a fat computer living in a blue sky. Did you know? Planet in Portuguese is 'planeta'.");
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (language);

  doc = gw_language_create_document_sync (language, string, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (doc);

  /* Modify the document */
  mod = GW_MODIFIER (stub_modifier_new ());

  result = gw_document_modify_sync (doc, mod, NULL, &error);

  g_assert_no_error (error);
  g_assert_true (result);

  g_assert_cmpstr (gw_document_get_text (doc),
                   ==,
                   "You are a fish computer living in a red sky. Did you know? Planet in Portuguese is 'lua'.");
}

/*****************************************************************************/

static void
document_modified_cb (GObject      *source_object,
                      GAsyncResult *res,
                      gpointer      user_data)
{
  g_autoptr (GwDocument) doc;
  GMainLoop *mainloop;
  GError *error;
  gboolean result;

  doc = GW_DOCUMENT (source_object);
  error = NULL;
  mainloop = user_data;
  result = gw_document_modify_finish (res, &error);

  g_assert_no_error (error);
  g_assert_true (result);

  g_assert_cmpstr (gw_document_get_text (doc),
                   ==,
                   "You are a fish computer living in a red sky. Did you know? Planet in Portuguese is 'lua'.");

  g_main_loop_quit (mainloop);
}

static void
document_modify_async (void)
{
  g_autoptr (GwLanguage) language;
  g_autoptr (GwDocument) doc;
  g_autoptr (GwModifier) mod;
  g_autoptr (GwString) string;
  GMainLoop *mainloop;
  GError *error;

  error = NULL;
  string = gw_string_new ("I am a fat computer living in a blue sky. Did you know? Planet in Portuguese is 'planeta'.");
  mainloop = g_main_loop_new (NULL, FALSE);
  language = gw_language_new_sync (NULL, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (language);

  doc = gw_language_create_document_sync (language, string, NULL, &error);

  g_assert_no_error (error);
  g_assert_nonnull (doc);

  /* Modify the document */
  mod = GW_MODIFIER (stub_modifier_new ());

  gw_document_modify (doc,
                      mod,
                      NULL,
                      document_modified_cb,
                      mainloop);

  g_main_loop_run (mainloop);
}
/*****************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/document/new", document_new);
  g_test_add_func ("/document/new_async", document_new_async);
  g_test_add_func ("/document/new_from_string", document_new_from_string);
  g_test_add_func ("/document/new_from_string_async", document_new_from_string_async);
  g_test_add_func ("/document/modify", document_modify);
  g_test_add_func ("/document/modify_async", document_modify_async);

  return g_test_run ();
}
