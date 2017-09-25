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
dict_pt_br (void)
{
  g_autoptr (GwLanguage) language;
  GwDictionary *dict;

  language = gw_language_new_sync ("pt_BR", NULL, NULL);

  dict = gw_language_get_dictionary (language);
  g_assert_nonnull (dict);
}

/*****************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/dictionary/lang/pt_BR", dict_pt_br);

  return g_test_run ();
}
