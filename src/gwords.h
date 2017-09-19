/* gwords.h
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto
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

#ifndef GWORDS_H
#define GWORDS_H

#include <glib.h>

G_BEGIN_DECLS

#define GW_INSIDE
# include "gw-dictionary.h"
# include "gw-document.h"
# include "gw-extension-points.h"
# include "gw-group.h"
# include "gw-language.h"
# include "gw-modifier.h"
# include "gw-radix-tree.h"
# include "gw-segmenter.h"
# include "gw-string.h"
# include "gw-string-comparator.h"
# include "gw-string-editor.h"
# include "gw-utils.h"
# include "gw-version.h"
# include "gw-word.h"
#undef GW_INSIDE

G_END_DECLS

#endif /* GWORDS_H */
