/* test-segmenters.c
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
fallback (void)
{
  g_autoptr (GwSegmenter) segmenter;
  g_autoptr (GwString) str;
  GwLanguage *language;
  GStrv words;
  gboolean success;

  str = gw_string_new_static ("This segmenter should break only by whitespaces, commas,\n periods and new lines.");
  language = gw_language_new_sync ("invalid", NULL, NULL);

  /* Setup the segmenter */
  segmenter = gw_language_get_segmenter (language);

  g_assert_nonnull (segmenter);
  g_assert_cmpstr (g_type_name (G_OBJECT_TYPE (segmenter)), ==, "GwSegmenterFallback");

  gw_segmenter_set_text (segmenter, str);
  g_assert (gw_segmenter_get_text (segmenter) == str);

  success = gw_segmenter_segment_sync (segmenter, NULL, NULL);
  g_assert_true (success);

  words = gw_segmenter_get_words (segmenter);
  g_assert_cmpuint (g_strv_length (words), ==, 12);

  /* NULL string */
  gw_segmenter_set_text (segmenter, NULL);
  g_assert_null (gw_segmenter_get_text (segmenter));

  success = gw_segmenter_segment_sync (segmenter, NULL, NULL);
  g_assert_true (success);

  words = gw_segmenter_get_words (segmenter);
  g_assert_cmpuint (g_strv_length (words), ==, 0);
}

/**************************************************************************************************/

static void
pt_BR (void)
{
  g_autoptr (GwSegmenter) segmenter;
  g_autoptr (GwString) str;
  GwLanguage *language;
  GStrv words;
  gboolean success;

  str = gw_string_new_static ("Oi, eu sou o Goku! E essas frases devem ser quebradas corretamente - por exemplo,\n"
                              "mesmo que eu adicione novas linhas e hífens, ainda deve funcionar!");
  language = gw_language_new_sync ("pt_BR", NULL, NULL);

  /* Setup the segmenter */
  segmenter = gw_language_get_segmenter (language);

  g_assert_nonnull (segmenter);
  g_assert_cmpstr (g_type_name (G_OBJECT_TYPE (segmenter)), ==, "GwSegmenterPtBr");

  gw_segmenter_set_text (segmenter, str);
  g_assert (gw_segmenter_get_text (segmenter) == str);

  success = gw_segmenter_segment_sync (segmenter, NULL, NULL);
  g_assert_true (success);

  words = gw_segmenter_get_words (segmenter);
  g_assert_cmpuint (g_strv_length (words), ==, 25);

  /* NULL string */
  gw_segmenter_set_text (segmenter, NULL);
  g_assert_null (gw_segmenter_get_text (segmenter));

  success = gw_segmenter_segment_sync (segmenter, NULL, NULL);
  g_assert_true (success);

  words = gw_segmenter_get_words (segmenter);
  g_assert_cmpuint (g_strv_length (words), ==, 0);
}

/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  gw_init ();

  g_test_add_func ("/segmenters/fallback", fallback);

  g_test_add_func ("/segmenters/pt_BR", pt_BR);

  return g_test_run ();
}
