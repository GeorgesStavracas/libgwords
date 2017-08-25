/*
 * Copyright (C) 2016 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef GW_STRING_H
#define GW_STRING_H

#include <glib.h>

G_BEGIN_DECLS

typedef gchar GwString;

/**
 * GwStringDebugFlags:
 * @GW_STRING_DEBUG_NONE:    No detailed debugging
 * @GW_STRING_DEBUG_DEDUPED: Show detailed dedupe stats
 * @GW_STRING_DEBUG_DUPES:   Show detailed duplication stats
 *
 * The debug type flags.
 **/
typedef enum {
  GW_STRING_DEBUG_NONE  = 0,    /* Since: 0.6.16 */
  GW_STRING_DEBUG_DEDUPED  = 1 << 0,  /* Since: 0.6.16 */
  GW_STRING_DEBUG_DUPES  = 1 << 1,  /* Since: 0.6.16 */
  /*< private >*/
  GW_STRING_DEBUG_LAST
} GwStringDebugFlags;

#define              gw_string_new_static(o)                     (GwString *) (("\xff\xff\xff\xff" o) + 4)

GwString*            gw_string_new                               (const gchar        *str);

GwString*            gw_string_new_with_length                   (const gchar        *str,
                                                                  gsize               len);

GwString*            gw_string_new_copy                          (const gchar        *str);

GwString*            gw_string_new_copy_with_length              (const gchar        *str,
                                                                  gsize               len);

GwString*            gw_string_ref                               (GwString           *rstr);

GwString*            gw_string_unref                             (GwString           *rstr);

void                 gw_string_assign                            (GwString          **rstr_ptr,
                                                                  GwString           *rstr);

void                 gw_string_assign_safe                       (GwString          **rstr_ptr,
                                                                  const gchar        *str);

gchar*               gw_string_debug                             (GwStringDebugFlags flags);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(GwString, gw_string_unref)

G_END_DECLS

#endif /* GW_STRING_H */
