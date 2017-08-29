/* gw-init.c
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

#include "gw-extension-points.h"
#include "gw-init-segmenters.h"
#include "gw-segmenter.h"

#include <gio/gio.h>

static void
init_segmenters (void)
{

#define implement_segmenter(T,lang,priority) g_io_extension_point_implement (GW_EXTENSION_POINT_SEGMENTER, T, lang, priority)

  implement_segmenter (GW_TYPE_SEGMENTER_FALLBACK, "fallback", 10);
  implement_segmenter (GW_TYPE_SEGMENTER_PT_BR,    "pt_BR",    10);

#undef implement_segmenter
}

__attribute__((constructor))
static void
_gw_init (void)
{
  GIOExtensionPoint *extension_point;

  /* Extension point for GwSegmenter */
  extension_point = g_io_extension_point_register (GW_EXTENSION_POINT_SEGMENTER);
  g_io_extension_point_set_required_type (extension_point, GW_TYPE_SEGMENTER);

  init_segmenters ();
}
