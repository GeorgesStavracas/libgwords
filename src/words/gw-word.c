/* gw-word.c
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

#include "gw-word.h"

struct _GwWord
{
  GNode               node;

  guint               ref_count;
};

G_DEFINE_BOXED_TYPE (GwWord, gw_word, gw_word_ref, gw_word_unref)

GwWord*
gw_word_copy (GwWord *self)
{
  GwWord *copy;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  copy = gw_word_new ();

  return copy;
}

static void
gw_word_free (GwWord *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  g_slice_free (GwWord, self);
}

GwWord*
gw_word_ref (GwWord *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
gw_word_unref (GwWord *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    gw_word_free (self);
}


GwWord*
gw_word_new (void)
{
  GwWord *self;

  self = g_slice_new0 (GwWord);
  self->ref_count = 1;

  return self;
}

