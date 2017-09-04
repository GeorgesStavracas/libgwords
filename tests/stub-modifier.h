/* stub-modifier.h
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

#ifndef STUB_MODIFIER_H
#define STUB_MODIFIER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define STUB_TYPE_MODIFIER (stub_modifier_get_type())

G_DECLARE_FINAL_TYPE (StubModifier, stub_modifier, STUB, MODIFIER, GObject)

StubModifier*        stub_modifier_new                           (void);

G_END_DECLS

#endif /* STUB_MODIFIER_H */

