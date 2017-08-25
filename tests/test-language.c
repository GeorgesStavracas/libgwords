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
get_system_language (void)
{
  gboolean result;
  gchar *language, *region;

  result = gw_get_system_language (&language, &region);

  g_assert (result);

  g_message ("language: %s", language);
  g_message ("region: %s", region);

  g_clear_pointer (&language, g_free);
}


/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/language/get_system_language", get_system_language);

  return g_test_run ();
}
