/* test-string-comparator.c
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
string_comparator_levenshtein (void)
{
  g_autoptr (GwString) str_a, str_b;
  gint result;

  str_a = gw_string_new_static ("The brown fox jumps over the lazy dog");
  str_b = gw_string_new_static ("The red dog jumps over the lazy fox");

  result = gw_string_comparator_get_distance (GW_COMPARATOR_LEVENSHTEIN, str_a, str_b);

  g_assert_cmpint (result, ==, 8);
}

/**************************************************************************************************/

gint
main (gint   argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/string_comparator/levenshtein", string_comparator_levenshtein);

  return g_test_run ();
}
