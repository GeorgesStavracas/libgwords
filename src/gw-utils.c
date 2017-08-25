/* gw-utils.c
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

#include "gw-utils.h"

gboolean
gw_get_system_language (gchar **language,
                        gchar **region)
{
  const gchar *value = NULL;
  gchar lang[3] = { '\0', };
  gchar reg[3] = { '\0', };

  value = g_getenv ("LANGUAGE");

  if (value && *value)
    goto out;

  value = g_getenv ("LC_ALL");

  if (value && *value)
    goto out;


  value = g_getenv ("LANG");

  if (value && *value)
    goto out;

  value = g_getenv ("LC_MESSAGES");

out:

  if (value && *value)
    {
      lang[0] = value[0];
      lang[1] = value[1];
      reg[0] = value[3];
      reg[1] = value[4];
    }

  if (language)
    *language = value ? g_strdup (lang) : NULL;

  if (region)
    *region = value ? g_strdup (reg) : NULL;

  return value != NULL;
}
