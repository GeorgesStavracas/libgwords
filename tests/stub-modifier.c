/* stub-modifier.c
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

#include "../src/gwords.h"

#include "stub-modifier.h"

#include <string.h>

struct _StubModifier
{
  GObject             parent;
};

static void          stub_modifier_interface_init                (GwModifierInterface *iface);

G_DEFINE_TYPE_WITH_CODE (StubModifier, stub_modifier, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GW_TYPE_MODIFIER, stub_modifier_interface_init))

/*
 * GwModifier iface
 */

static const struct
{
  const gchar *word;
  const gchar *replacement;
} map[] = {
    { "I",       "You"  },
    { "am",      "are"  },
    { "fat",     "fish" },
    { "blue",    "red"  },
    { "planeta", "lua"  },
    { NULL, }
};

static gboolean
stub_modifier_modify_word (GwModifier      *self,
                           const GwString  *word,
                           gsize            len,
                           GwString       **new_word,
                           gsize           *new_len)
{
  guint i;

  for (i = 0; map[i].word; i++)
    {
      if (g_strcmp0 (word, map[i].word) == 0)
        {
          *new_word = gw_string_new (map[i].replacement);
          *new_len = strlen (map[i].replacement);
          return TRUE;
        }
    }

  return FALSE;
}

static void
stub_modifier_interface_init (GwModifierInterface *iface)
{
  iface->modify_word = stub_modifier_modify_word;
}

static void
stub_modifier_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
stub_modifier_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
stub_modifier_class_init (StubModifierClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = stub_modifier_get_property;
  object_class->set_property = stub_modifier_set_property;
}

static void
stub_modifier_init (StubModifier *self)
{
}

StubModifier*
stub_modifier_new (void)
{
  return g_object_new (STUB_TYPE_MODIFIER, NULL);
}
