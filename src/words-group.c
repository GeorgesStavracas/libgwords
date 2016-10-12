/* words-group.c
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

#include "words-group.h"
#include "words-radix-tree.h"

struct _WordsGroup
{
  guint               ref_count;

  const gchar        *group_id;

  WordsRadixTree     *words;
};

G_DEFINE_BOXED_TYPE (WordsGroup, words_group, words_group_ref, words_group_unref)

WordsGroup*
words_group_new (const gchar *group_id)
{
  WordsGroup *self;

  self = g_slice_new0 (WordsGroup);
  self->group_id = group_id;
  self->ref_count = 1;
  self->words = words_radix_tree_new ();

  return self;
}

WordsGroup*
words_group_copy (WordsGroup *self)
{
  WordsGroup *copy;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  copy = words_group_new (self->group_id);

  return copy;
}

static void
words_group_free (WordsGroup *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_clear_object (&self->words);

  g_slice_free (WordsGroup, self);
}

WordsGroup*
words_group_ref (WordsGroup *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
words_group_unref (WordsGroup *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    words_group_free (self);
}

/**
 * words_group_insert_word:
 * @self: a #WordsGroup
 * @word: a word to be added to @self
 *
 * Inserts @word in @self. If the word is already there, this does nothing.
 *
 * Since: 0.1.0
 */
void
words_group_insert_word (WordsGroup  *self,
                         const gchar *word)
{
  words_radix_tree_insert (self->words, word, NULL);
}

/**
 * words_group_remove_word:
 * @self: a #WordsGroup
 * @word: a word to be removed from @self
 *
 * Removes @word from @self. If the word is not there, this does nothing.
 *
 * Since: 0.1.0
 */
void
words_group_remove_word (WordsGroup  *self,
                         const gchar *word)
{
  words_radix_tree_remove (self->words, word);
}

/**
 * words_group_contains_word:
 * @self: a #WordsGroup
 * @word: a word to be search in @self
 *
 * Checks if @word is in @self.
 *
 * Returns: %TRUE is @word is in the group, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_group_contains_word (WordsGroup  *self,
                           const gchar *word)
{
  return words_radix_tree_contains (self->words, word);
}

/**
 * words_group_get_words:
 * @self: a #WordsGroup
 *
 * Retrieves all the words in @self.
 *
 * Returns: (nullable)(transfer full): a list of words. Free with g_strfreev().
 *
 * Since: 0.1.0
 */
GStrv
words_group_get_words (WordsGroup *self)
{
  return words_radix_tree_get_keys (self->words);
}
