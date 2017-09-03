/* gw-modifier.c
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

#include "gw-modifier.h"

G_DEFINE_INTERFACE (GwModifier, gw_modifier, G_TYPE_OBJECT)

static void
gw_modifier_default_init (GwModifierInterface *iface)
{
}

/**
 * gw_modifier_modify_word:
 * @self: a #GwModifier
 * @word: a #GwString
 * @len: the word length
 * @new_word: return location for the modified word
 * @new_len: return location for the size of the modified word
 *
 * Potentially modifies or not @word. The return result must be passed
 * by @new_word.
 *
 * Returns: %TRUE if the word was modified, %FALSE otherwise.
 *
 * Since: 0.1
 */
gboolean
gw_modifier_modify_word (GwModifier      *self,
                         const GwString  *word,
                         gsize            len,
                         GwString       **new_word,
                         gsize           *new_len)
{
  g_return_val_if_fail (GW_IS_MODIFIER (self), FALSE);

  if (GW_MODIFIER_GET_IFACE (self)->modify_word)
    return GW_MODIFIER_GET_IFACE (self)->modify_word (self, word, len, new_word, new_len);

  return FALSE;
}
