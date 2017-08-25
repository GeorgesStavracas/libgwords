/* gw-group.c
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
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

#include "gw-group.h"
#include "gw-radix-tree.h"

struct _GwGroup
{
  guint               ref_count;

  const gchar        *group_id;

  GwRadixTree     *gw;
};

G_DEFINE_BOXED_TYPE (GwGroup, gw_group, gw_group_ref, gw_group_unref)

GwGroup*
gw_group_new (const gchar *group_id)
{
  GwGroup *self;

  self = g_slice_new0 (GwGroup);
  self->group_id = group_id;
  self->ref_count = 1;
  self->gw = gw_radix_tree_new ();

  return self;
}

GwGroup*
gw_group_copy (GwGroup *self)
{
  GwGroup *copy;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  copy = gw_group_new (self->group_id);

  return copy;
}

static void
gw_group_free (GwGroup *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_clear_object (&self->gw);

  g_slice_free (GwGroup, self);
}

GwGroup*
gw_group_ref (GwGroup *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
gw_group_unref (GwGroup *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    gw_group_free (self);
}

/**
 * gw_group_insert_word:
 * @self: a #GwGroup
 * @word: a word to be added to @self
 * @word_length: length of @word, or -1
 *
 * Inserts @word in @self. If the word is already there, this does nothing.
 *
 * Since: 0.1.0
 */
void
gw_group_insert_word (GwGroup     *self,
                      const gchar *word,
                      gsize        word_length)
{
  gw_radix_tree_insert (self->gw, word, word_length, NULL);
}

/**
 * gw_group_remove_word:
 * @self: a #GwGroup
 * @word: a word to be removed from @self
 * @word_length: length of @word, or -1
 *
 * Removes @word from @self. If the word is not there, this does nothing.
 *
 * Since: 0.1.0
 */
void
gw_group_remove_word (GwGroup     *self,
                      const gchar *word,
                      gsize        word_length)
{
  gw_radix_tree_remove (self->gw, word, word_length);
}

/**
 * gw_group_contains_word:
 * @self: a #GwGroup
 * @word: a word to be search in @self
 * @word_length: length of @word, or -1
 *
 * Checks if @word is in @self.
 *
 * Returns: %TRUE is @word is in the group, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_group_contains_word (GwGroup     *self,
                        const gchar *word,
                        gsize        word_length)
{
  return gw_radix_tree_contains (self->gw, word, word_length);
}

/**
 * gw_group_get_gw:
 * @self: a #GwGroup
 *
 * Retrieves all the gw in @self.
 *
 * Returns: (nullable)(transfer full): a list of gw. Free with g_strfreev().
 *
 * Since: 0.1.0
 */
GStrv
gw_group_get_gw (GwGroup *self)
{
  return gw_radix_tree_get_keys (self->gw);
}
