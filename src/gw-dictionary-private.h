/* gw-dictionary-private.h
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

#ifndef GW_DICTIONARY_PRIVATE_H
#define GW_DICTIONARY_PRIVATE_H

#include <glib.h>

G_BEGIN_DECLS

gboolean             gw_dictionary_load                          (GwDictionary       *self);

gboolean             gw_dictionary_unload                        (GwDictionary       *self);


G_END_DECLS

#endif /* GW_DICTIONARY_PRIVATE_H */
