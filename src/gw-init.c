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

#include "gconstructor.h"
#include "gw-document.h"
#include "gw-extension-points.h"
#include "gw-segmenter.h"

#include "gw-init-dictionaries.h"
#include "gw-init-documents.h"
#include "gw-init-segmenters.h"

#include <gio/gio.h>

static void
init_dictionaries (void)
{
  GIOExtensionPoint *extension_point;

  /* Extension point for GwDictionary */
  extension_point = g_io_extension_point_register (GW_EXTENSION_POINT_DICTIONARY);
  g_io_extension_point_set_required_type (extension_point, GW_TYPE_DICTIONARY);

#define implement_dictionary(T,lang,priority) g_io_extension_point_implement (GW_EXTENSION_POINT_DICTIONARY, T, lang, priority)

  implement_dictionary (GW_TYPE_DICTIONARY_FALLBACK, "fallback", 10);
  implement_dictionary (GW_TYPE_DICTIONARY_PT_BR,    "pt_BR",    10);

#undef implement_document
}

static void
init_documents (void)
{
  GIOExtensionPoint *extension_point;

  /* Extension point for GwDocument */
  extension_point = g_io_extension_point_register (GW_EXTENSION_POINT_DOCUMENT);
  g_io_extension_point_set_required_type (extension_point, GW_TYPE_DOCUMENT);

#define implement_document(T,lang,priority) g_io_extension_point_implement (GW_EXTENSION_POINT_DOCUMENT, T, lang, priority)

  implement_document (GW_TYPE_DOCUMENT_FALLBACK, "fallback", 10);

#undef implement_document
}

static void
init_segmenters (void)
{
  GIOExtensionPoint *extension_point;

  /* Extension point for GwSegmenter */
  extension_point = g_io_extension_point_register (GW_EXTENSION_POINT_SEGMENTER);
  g_io_extension_point_set_required_type (extension_point, GW_TYPE_SEGMENTER);

#define implement_segmenter(T,lang,priority) g_io_extension_point_implement (GW_EXTENSION_POINT_SEGMENTER, T, lang, priority)

  implement_segmenter (GW_TYPE_SEGMENTER_FALLBACK, "fallback", 10);
  implement_segmenter (GW_TYPE_SEGMENTER_PT_BR,    "pt_BR",    10);

#undef implement_segmenter
}

/* Main constructor */
#ifdef G_DEFINE_CONSTRUCTOR_NEEDS_PRAGMA
  #pragma G_DEFINE_CONSTRUCTOR_PRAGMA_ARGS (_gw_init)
#endif

G_DEFINE_CONSTRUCTOR (_gw_init)

static void
_gw_init (void)
{
  init_dictionaries ();
  init_documents ();
  init_segmenters ();
}
