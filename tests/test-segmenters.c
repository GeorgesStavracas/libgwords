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

  str = gw_string_new_static ("This segmenter should break only by whitespaces, commas,\n periods and new lines.");
  language = gw_language_new_sync ("invalid", NULL, NULL);

  /* Setup the segmenter */
  segmenter = gw_language_get_segmenter (language);

  g_assert_nonnull (segmenter);
  g_assert_cmpstr (g_type_name (G_OBJECT_TYPE (segmenter)), ==, "GwSegmenterFallback");

  words = gw_segmenter_segment_sync (segmenter, str, NULL, NULL);

  g_assert_nonnull (words);
  g_assert_cmpuint (g_strv_length (words), ==, 12);

  /* NULL string */
  words = gw_segmenter_segment_sync (segmenter, NULL, NULL, NULL);

  g_assert_nonnull (words);
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

  str = gw_string_new_static ("Oi, eu sou o Goku! E essas frases devem ser quebradas corretamente - por exemplo,\n"
                              "mesmo que eu adicione novas linhas e hífens, ainda deve funcionar!");
  language = gw_language_new_sync ("pt_BR", NULL, NULL);

  /* Setup the segmenter */
  segmenter = gw_language_get_segmenter (language);

  g_assert_nonnull (segmenter);
  g_assert_cmpstr (g_type_name (G_OBJECT_TYPE (segmenter)), ==, "GwSegmenterPtBr");

  words = gw_segmenter_segment_sync (segmenter, str, NULL, NULL);

  g_assert_nonnull (segmenter);
  g_assert_cmpuint (g_strv_length (words), ==, 25);

  /* NULL string */
  words = gw_segmenter_segment_sync (segmenter, NULL, NULL, NULL);

  g_assert_nonnull (segmenter);
  g_assert_cmpuint (g_strv_length (words), ==, 0);
}

/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/segmenters/fallback", fallback);

  g_test_add_func ("/segmenters/pt_BR", pt_BR);

  return g_test_run ();
}
