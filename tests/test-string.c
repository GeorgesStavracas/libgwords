/* test-string.c
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
string_init (void)
{
  GwString *string;
  gchar *text;

  text = g_strdup ("BLABLABLABLABLABLA");
  string = gw_string_new (text);

  gw_string_ref (string);

  gw_string_unref (string);
  gw_string_unref (string);
}

/**************************************************************************************************/

static void
static_string_init (void)
{
  GwString *string;

  string = gw_string_new_static ("this is a static string");

  gw_string_ref (string);

  gw_string_unref (string);
  gw_string_unref (string);
}

/**************************************************************************************************/

static void
string_printf (void)
{
  g_autoptr (GwString) string;

  string = gw_string_new_printf ("I'm %d, %d or even 0x%x!", 1, 2, 0xdeadbeef);

  g_assert_cmpstr (string, ==, "I'm 1, 2 or even 0xdeadbeef!");
}
/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/string/init", string_init);
  g_test_add_func ("/string/static_init", static_string_init);
  g_test_add_func ("/string/printf", string_printf);

  return g_test_run ();
}
