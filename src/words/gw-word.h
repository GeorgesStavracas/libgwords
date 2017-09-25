/* gw-word.h
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

#ifndef GW_WORD_H
#define GW_WORD_H

#include "gw-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GW_TYPE_WORD (gw_word_get_type())

typedef struct _GwWord GwWord;

GwWord*              gw_word_new                                 (GwString           *word,
                                                                  GwGrammarClass      word_class,
                                                                  GwWordPeriod        period,
                                                                  GwWordState         state);

GwWord*              gw_word_copy                                (GwWord             *self);

GwWord*              gw_word_ref                                 (GwWord             *self);

void                 gw_word_unref                               (GwWord             *self);

GwWordPeriod         gw_word_get_period                          (GwWord             *self);

GwWordState          gw_word_get_state                           (GwWord             *self);

GwString*            gw_word_get_string                          (GwWord             *self);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GwWord, gw_word_unref)

G_END_DECLS

#endif /* GW_WORD_H */

