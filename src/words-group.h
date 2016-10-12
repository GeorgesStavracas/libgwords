/* words-group.h
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

#ifndef WORDS_GROUP_H
#define WORDS_GROUP_H

#include <glib-object.h>

G_BEGIN_DECLS

#define WORDS_TYPE_GROUP (words_group_get_type())

typedef struct _WordsGroup WordsGroup;

GType                words_group_get_type                        (void) G_GNUC_CONST;

WordsGroup*          words_group_new                             (const gchar        *group_id);

WordsGroup*          words_group_copy                            (WordsGroup         *self);

WordsGroup*          words_group_ref                             (WordsGroup         *self);

void                 words_group_unref                           (WordsGroup         *self);

void                 words_group_insert_word                     (WordsGroup         *self,
                                                                  const gchar        *word);

void                 words_group_remove_word                     (WordsGroup         *self,
                                                                  const gchar        *word);

gboolean             words_group_contains_word                   (WordsGroup         *self,
                                                                  const gchar        *word);

GStrv                words_group_get_words                       (WordsGroup         *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (WordsGroup, words_group_unref)

G_END_DECLS

#endif /* WORDS_GROUP_H */

