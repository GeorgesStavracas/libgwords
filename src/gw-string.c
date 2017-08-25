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

/**
 * SECTION:gw-string
 * @short_description: Reference counted strings
 * @include: appstream-glib.h
 * @stability: Unstable
 *
 * These functions are used to implement refcounted C strings.
 */

#include <string.h>

#include "gw-string.h"

typedef struct
{
  volatile gint       refcnt;
} GwStringHeader;

#define GW_PTR_TO_HEADER(o)    ((GwStringHeader *) ((void *) ((guint8 *) o - sizeof (GwStringHeader))))
#define GW_PTR_FROM_HEADER(o)  ((gpointer) (((guint8 *) o) + sizeof (GwStringHeader)))

static inline void
gw_string_unref_from_str (gchar *str)
{
  g_free (GW_PTR_TO_HEADER (str));
}

static GHashTable *gw_string_hash = NULL;
static GMutex gw_string_mutex;

static GHashTable *
gw_string_get_hash_safe (void)
{
  if (!gw_string_hash)
    {
      /* gpointer to GwStringHeader */
      gw_string_hash = g_hash_table_new_full (g_direct_hash,
                                              g_direct_equal,
                                              (GDestroyNotify) gw_string_unref_from_str,
                                              NULL);
    }

  return gw_string_hash;
}

static void __attribute__ ((destructor))
gw_string_destructor (void)
{
  g_clear_pointer (&gw_string_hash, g_hash_table_unref);
}

/**
 * gw_string_new_static:
 * @str: a string
 *
 * Returns a refcounted string from a static string. The static string cannot
 * be unloaded and freed.
 *
 * Returns: a %GwString
 *
 * Since: 0.6.6
 */

/**
 * gw_string_new_copy_with_length:
 * @str: a string
 * @len: length of @str, not including the NUL byte
 *
 * Returns a deep copied refcounted string. The returned string can be modified
 * without affecting other refcounted versions.
 *
 * Returns: a %GwString
 *
 * Since: 0.6.6
 */
GwString*
gw_string_new_copy_with_length (const gchar *str, gsize len)
{
  g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&gw_string_mutex);
  GwStringHeader *hdr;
  GwString *rstr_new;

  /* create object */
  hdr = g_malloc (len + sizeof (GwStringHeader) + 1);
  hdr->refcnt = 1;
  rstr_new = GW_PTR_FROM_HEADER (hdr);
  memcpy (rstr_new, str, len);
  rstr_new[len] = '\0';

  /* add */
  g_hash_table_add (gw_string_get_hash_safe (), rstr_new);

  /* return to data, not the header */
  return rstr_new;
}

/**
 * gw_string_new_copy:
 * @str: a string
 *
 * Returns a deep copied refcounted string. The returned string can be modified
 * without affecting other refcounted versions.
 *
 * Returns: a %GwString
 *
 * Since: 0.6.6
 */
GwString *
gw_string_new_copy (const gchar *str)
{
  g_return_val_if_fail (str != NULL, NULL);

  return gw_string_new_copy_with_length (str, strlen (str));
}

/**
 * gw_string_new_with_length:
 * @str: a string
 * @len: length of @str, not including the NUL byte
 *
 * Returns a immutable refcounted string. The returned string cannot be modified
 * without affecting other refcounted versions.
 *
 * Returns: a %GwString
 *
 * Since: 0.6.6
 */
GwString *
gw_string_new_with_length (const gchar *str, gsize len)
{
  GwStringHeader *hdr;
  g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&gw_string_mutex);

  g_return_val_if_fail (str != NULL, NULL);

  /* already in hash */
  if (g_hash_table_contains (gw_string_get_hash_safe (), str))
    {
      hdr = GW_PTR_TO_HEADER (str);

      if (hdr->refcnt < 0)
        return (GwString *) str;

      g_atomic_int_inc (&hdr->refcnt);

      return (GwString *) str;
    }

  g_clear_pointer (&locker, g_mutex_locker_free);

  return gw_string_new_copy_with_length (str, len);
}

/**
 * gw_string_new:
 * @str: a string
 *
 * Returns a immutable refcounted string. The returned string cannot be modified
 * without affecting other refcounted versions.
 *
 * Returns: a %GwString
 *
 * Since: 0.6.6
 */
GwString *
gw_string_new (const gchar *str)
{
  g_return_val_if_fail (str != NULL, NULL);

  return gw_string_new_with_length (str, strlen (str));
}

/**
 * gw_string_ref:
 * @rstr: a #GwString
 *
 * Adds a reference to the string.
 *
 * Returns: the same %GwString
 *
 * Since: 0.6.6
 */
GwString *
gw_string_ref (GwString *rstr)
{
  GwStringHeader *hdr;

  g_return_val_if_fail (rstr != NULL, NULL);

  hdr = GW_PTR_TO_HEADER (rstr);

  if (hdr->refcnt < 0)
    return rstr;

  g_atomic_int_inc (&hdr->refcnt);

  return rstr;
}

/**
 * gw_string_unref:
 * @rstr: a #GwString
 *
 * Removes a reference to the string.
 *
 * Returns: the same %GwString, or %NULL if the refcount dropped to zero
 *
 * Since: 0.6.6
 */
GwString *
gw_string_unref (GwString *rstr)
{
  GwStringHeader *hdr;

  g_return_val_if_fail (rstr != NULL, NULL);

  hdr = GW_PTR_TO_HEADER (rstr);

  if (hdr->refcnt < 0)
    return rstr;

  if (g_atomic_int_dec_and_test (&hdr->refcnt))
    {
      g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&gw_string_mutex);

      g_hash_table_remove (gw_string_get_hash_safe (), rstr);

      return NULL;
    }

  return rstr;
}

/**
 * gw_string_assign:
 * @rstr_ptr: (out): a #GwString
 * @rstr: a #GwString
 *
 * This function unrefs and clears @rstr_ptr if set, then sets @rstr if
 * non-NULL. If @rstr and @rstr_ptr are the same string the action is ignored.
 *
 * This function can ONLY be used when @str is guaranteed to be a
 * refcounted string and is suitable for use when getting strings from
 * methods without a fixed API.
 *
 * This function is slightly faster than gw_string_assign_safe() as no
 * hash table lookup is done on the @rstr pointer.
 *
 * Since: 0.6.6
 */
void
gw_string_assign (GwString **rstr_ptr, GwString *rstr)
{
  g_return_if_fail (rstr_ptr != NULL);

  if (*rstr_ptr == rstr)
    return;

  if (*rstr_ptr != NULL)
    {
      gw_string_unref (*rstr_ptr);
      *rstr_ptr = NULL;
    }

  if (rstr != NULL)
    *rstr_ptr = gw_string_ref (rstr);
}

/**
 * gw_string_assign_safe:
 * @rstr_ptr: (out): a #GwString
 * @str: a string, or a #GwString
 *
 * This function unrefs and clears @rstr_ptr if set, then sets @rstr if
 * non-NULL. If @rstr and @rstr_ptr are the same string the action is ignored.
 *
 * This function should be used when @str cannot be guaranteed to be a
 * refcounted string and is suitable for use in existing object setters.
 *
 * Since: 0.6.6
 */
void
gw_string_assign_safe (GwString **rstr_ptr, const gchar *str)
{
  g_return_if_fail (rstr_ptr != NULL);

  if (*rstr_ptr != NULL)
    {
      gw_string_unref (*rstr_ptr);
      *rstr_ptr = NULL;
    }

  if (str != NULL)
    *rstr_ptr = gw_string_new (str);
}

static gint
gw_string_sort_by_refcnt_cb (gconstpointer a, gconstpointer b)
{
  GwStringHeader *hdr1 = GW_PTR_TO_HEADER (a);
  GwStringHeader *hdr2 = GW_PTR_TO_HEADER (b);

  if (hdr1->refcnt > hdr2->refcnt)
    return -1;

  if (hdr1->refcnt < hdr2->refcnt)
    return 1;

  return 0;
}

/**
 * gw_string_debug:
 * @flags: some #GwStringDebugFlags, e.g. %GW_STRING_DEBUG_DUPES
 *
 * This function outputs some debugging information to a string.
 *
 * Returns: a string describing the current state of the dedupe hash.
 *
 * Since: 0.6.6
 */
gchar *
gw_string_debug (GwStringDebugFlags flags)
{
  GHashTable *hash = NULL;
  GString *tmp = g_string_new (NULL);
  g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&gw_string_mutex);

  /* overview */
  hash = gw_string_get_hash_safe ();
  g_string_append_printf (tmp, "Size of hash table: %u\n", g_hash_table_size (hash));

  /* success: deduped */
  if (flags & GW_STRING_DEBUG_DEDUPED)
    {
      GList *l;
      g_autoptr(GList) keys = g_hash_table_get_keys (hash);

      /* split up sections */
      if (tmp->len > 0)
        g_string_append (tmp, "\n\n");

      /* sort by refcount number */
      keys = g_list_sort (keys, gw_string_sort_by_refcnt_cb);

      g_string_append (tmp, "Deduplicated strings:\n");

      for (l = keys; l != NULL; l = l->next)
        {
          const gchar *str = l->data;
          GwStringHeader *hdr = GW_PTR_TO_HEADER (str);

          if (hdr->refcnt <= 1)
            continue;

          g_string_append_printf (tmp, "%i\t%s\n", hdr->refcnt, str);
        }
    }

  /* failed: duplicate */
  if (flags & GW_STRING_DEBUG_DUPES)
    {
      GList *l;
      GList *l2;
      g_autoptr(GHashTable) dupes = g_hash_table_new (g_direct_hash, g_direct_equal);
      g_autoptr(GList) keys = g_hash_table_get_keys (hash);

      /* split up sections */
      if (tmp->len > 0)
        g_string_append (tmp, "\n\n");

      g_string_append (tmp, "Duplicated strings:\n");

      for (l = keys; l != NULL; l = l->next)
        {
          GwStringHeader *header;
          const gchar *str;
          guint dupe_cnt = 0;

          str = l->data;
          header = GW_PTR_TO_HEADER (str);

          if (g_hash_table_contains (dupes, header))
            continue;

          g_hash_table_add (dupes, (gpointer) header);

          for (l2 = l; l2 != NULL; l2 = l2->next)
            {
              GwStringHeader *header2;
              const gchar *str2;

              str2 = l2->data;
              header2 = GW_PTR_TO_HEADER (str2);

              if (g_hash_table_contains (dupes, header2))
                continue;
              if (l == l2)
                continue;
              if (g_strcmp0 (str, str2) != 0)
                continue;
              g_hash_table_add (dupes, (gpointer) header2);
              dupe_cnt += 1;
            }

          if (dupe_cnt > 0)
            g_string_append_printf (tmp, "%u\t%s\n", dupe_cnt, str);
        }
    }

  return g_string_free (tmp, FALSE);
}
