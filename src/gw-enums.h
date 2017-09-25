/* gw-enums.h
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

#ifndef GW_ENUMS_H
#define GW_ENUMS_H

#include <glib.h>

G_BEGIN_DECLS

typedef enum
{
  GW_GRAMMAR_CLASS_INVALID      = -1,
  GW_GRAMMAR_CLASS_ADJECTIVE    =  1 << 0,
  GW_GRAMMAR_CLASS_ADPOSITION   =  1 << 1,
  GW_GRAMMAR_CLASS_ADVERB       =  1 << 2,
  GW_GRAMMAR_CLASS_ARTICLE      =  1 << 3,
  GW_GRAMMAR_CLASS_CONJUNCTION  =  1 << 4,
  GW_GRAMMAR_CLASS_INTERJECTION =  1 << 5,
  GW_GRAMMAR_CLASS_NOUN         =  1 << 6,
  GW_GRAMMAR_CLASS_NUMERAL      =  1 << 7,
  GW_GRAMMAR_CLASS_PRONOUN      =  1 << 8,
  GW_GRAMMAR_CLASS_VERB         =  1 << 9,
  GW_GRAMMAR_CLASS_OTHER        =  1 << 10,

  /* Special value for dictionary lookup */
  GW_GRAMMAR_CLASS_ALL          = 1 << 11,
} GwGrammarClass;

typedef enum
{
  GW_WORD_PERIOD_PAST,
  GW_WORD_PERIOD_PRESENT,
  GW_WORD_PERIOD_FUTURE,
  GW_WORD_PERIOD_INDETERMINED,
} GwWordPeriod;

typedef enum
{
  GW_WORD_STATE_CONDITIONAL,
  GW_WORD_STATE_COMPLETED,
  GW_WORD_STATE_ONGOING,
} GwWordState;

typedef enum
{
  GW_VERB_MODE_
} GwVerbMode;

G_END_DECLS

#endif /* GW_ENUMS_H */
