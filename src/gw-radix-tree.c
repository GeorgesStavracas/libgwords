/* gw-radix-tree.c
 *
 * Copyright (C) 2016 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
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

#include "gw-radix-tree.h"

#include <emmintrin.h>
#include <stdio.h>
#include <string.h>

/**
 * SECTION:gw-radix-tree
 * @short_title:adaptive radix tree designed for general usage
 * @title:GwRadixTree
 * @stability:Stable
 *
 * #GwRadixTree is an adaptive radix tree which performs very well memory- and
 * performance-wise. It works as a database-like data structure, optimize for string
 * keys.
 *
 * #GwRadixTree supports deleting keys with a destroy function, see gw_radix_tree_new_with_free_func().
 * The free function is called on the data assigned to the key only if it's non-%NULL.
 */

#define MAX_PREFIX_LEN 8

#define NODE_4         1
#define NODE_16        2
#define NODE_48        3
#define NODE_256       4

#define IS_LEAF(x)     (((guintptr) x & 1))
#define SET_LEAF(x)    ((gpointer)((guintptr) x | 1))
#define LEAF_RAW(x)    ((Leaf*)((gpointer)((guintptr) x & ~1)))
#define LEAF_KEY(x)    (G_STRUCT_MEMBER_P (x, G_STRUCT_OFFSET (Leaf, key)))

/*
 * This struct is included as part
 * of all the various node sizes
 */
typedef struct
{
  guint16             type;
  guint16             num_children;
  guint32             partial_len;
  guchar              partial[MAX_PREFIX_LEN];
} Node;

/*
 * Small node with only 4 children
 */
typedef struct __attribute__ ((packed))
{
  Node                n;
  guchar              keys[4];
  Node               *children[4];
} Node4;

/*
 * Node with 16 children
 */
typedef struct
{
  Node                n;
  guchar              keys[16];
  Node               *children[16];
} Node16;

/*
 * Node with 48 children, but
 * a full 256 byte field.
 */
typedef struct
{
  Node                n;
  guchar              keys[256];
  Node               *children[48];
} Node48;

/*
 * Full node with 256 children
 */
typedef struct
{
  Node                n;
  Node               *children[256];
} Node256;

/**
 * Represents a leaf. These are
 * of arbitrary size, as they include the key.
 */
typedef struct
{
  gpointer            value;
  guint32             key_len;
  guchar              key;
} Leaf;

G_STATIC_ASSERT (sizeof (Node)    == 16);
G_STATIC_ASSERT (sizeof (Node4)   == 52);
G_STATIC_ASSERT (sizeof (Node16)  == 160);
G_STATIC_ASSERT (sizeof (Node48)  == 656);
G_STATIC_ASSERT (sizeof (Node256) == 2064);


struct _GwRadixTree
{
  guint               ref_count;
  Node               *root;
  guint64             size;
  GDestroyNotify      destroy_func;
};

G_DEFINE_BOXED_TYPE (GwRadixTree, gw_radix_tree, gw_radix_tree_ref, gw_radix_tree_unref)

/*
 * Internal functions
 */

/* callbacks */
static inline gboolean
get_keys_cb (const gchar *key,
             gsize        key_length,
             gpointer     value,
             gpointer     user_data)
{
  GPtrArray *results = user_data;

  g_ptr_array_add (results, g_strdup (key));

  return GW_RADIX_TREE_ITER_CONTINUE;
}

static inline gboolean
get_values_cb (const gchar *key,
               gsize        key_length,
               gpointer     value,
               gpointer     user_data)
{
  GPtrArray *results = user_data;

  g_ptr_array_add (results, value);

  return GW_RADIX_TREE_ITER_CONTINUE;
}


/*
 * Node creation and destruction
 */
static gpointer
node_new (guint8 type)
{
  Node* n;

  switch (type)
    {
    case NODE_4:
      n = g_malloc0 (sizeof (Node4));
      break;

    case NODE_16:
      n = g_malloc0 (sizeof (Node16));
      break;

    case NODE_48:
      n = g_malloc0 (sizeof (Node48));
      break;

    case NODE_256:
      n = g_malloc0 (sizeof (Node256));
      break;

    default:
        g_assert_not_reached ();
    }

  n->type = type;

  return n;
}

static Leaf*
leaf_new (const guchar *key,
          gint          key_len,
          gpointer      value)
{
  Leaf *l;
  guchar *lkey;

  l = (Leaf*) g_malloc (sizeof (Leaf) + key_len * sizeof (guchar));
  l->value = value;
  l->key_len = key_len;

  lkey = LEAF_KEY (l);
  lkey[key_len] = '\0';

  memcpy (lkey, key, key_len);

  return l;
}

static void
destroy_node_recursive (Node *n)
{
  Node **children;
  guint i;

  if (!n)
    return;

  if (IS_LEAF (n))
    {
      g_free (LEAF_RAW (n));
      return;
    }

  children = NULL;

  switch (n->type)
    {
    case NODE_4:
      children = ((Node4*) n)->children;
      break;

    case NODE_16:
      children = ((Node16*) n)->children;
      break;

    case NODE_48:
      children = ((Node48*) n)->children;
      break;

    case NODE_256:
      children = ((Node256*) n)->children;
      break;

    default:
      g_assert_not_reached ();
    }

  if (children)
    {
      for (i = 0; i < n->num_children; i++)
        destroy_node_recursive (children[i]);
    }

  g_free (n);
}

/*
 * Auxiliary functions
 */
static Leaf*
minimum (const Node *n)
{
  gint i;

  if (!n)
    return NULL;

  if (IS_LEAF (n))
    return LEAF_RAW (n);

  switch (n->type)
    {
    case NODE_4:
      return minimum (((Node4*) n)->children[0]);

    case NODE_16:
      return minimum (((Node16*) n)->children[0]);

    case NODE_48:
      i = 0;

      while (!((Node48*) n)->keys[i])
        i++;

      i = ((Node48*) n)->keys[i] - 1;

      return minimum (((Node48*) n)->children[i]);

    case NODE_256:
      i = 0;

      while (!((Node256*) n)->children[i])
        i++;

      return minimum (((Node256*) n)->children[i]);

    default:
        g_assert_not_reached ();
    }

  return NULL;
}

static Node**
find_child (Node   *n,
            guchar  c)
{
  Node256 *n256;
  Node48 *n48;
  Node16 *n16;
  Node4 *n4;
  __m128i cmp;
  int i, mask, bitfield;

  switch (n->type)
    {
    case NODE_4:
        n4 = (Node4*) n;

        for (i = 0; i < n->num_children; i++)
          {
            if (n4->keys[i] == c)
              return &n4->children[i];
          }

        break;

    case NODE_16:
      n16 = (Node16*) n;

      cmp = _mm_cmpeq_epi8 (_mm_set1_epi8 (c), _mm_loadu_si128 ((__m128i*)n16->keys));
      mask = (1 << n->num_children) - 1;
      bitfield = _mm_movemask_epi8 (cmp) & mask;

      if (bitfield)
        return &n16->children[__builtin_ctz (bitfield)];

      break;

    case NODE_48:
      n48 = (Node48*) n;
      i = n48->keys[c];

      if (i > 0)
        return &n48->children[i - 1];

      break;

    case NODE_256:
      n256 = (Node256*) n;

      if (n256->children[c])
        return &n256->children[c];

      break;

    default:
      g_assert_not_reached ();
    }

  return NULL;
}

static gboolean
leaf_matches (const Leaf   *n,
              const guchar *key,
              gint          key_len)
{
  if (n->key_len != (guint32) key_len)
    return FALSE;

  return memcmp (LEAF_KEY (n), key, key_len) == 0;
}


static gint
longest_common_prefix (Leaf *l1,
                       Leaf *l2,
                       int   depth)
{
  gint max_cmp, i;

  max_cmp = MIN (l1->key_len, l2->key_len) - depth;

  for (i = 0; i < max_cmp; i++)
    {
      guchar *key1 = LEAF_KEY (l1);
      guchar *key2 = LEAF_KEY (l2);
      gint k = depth + i;

      if (key1[k] != key2[k])
          return i;
    }

  return i;
}

/*
 * Returns the number of prefix characters shared between
 * the key and node.
 */
static gint
check_prefix (const Node   *n,
              const guchar *key,
              gint          key_len,
              gint          depth)
{
  gint max_cmp, i;

  max_cmp = MIN (MIN (MAX_PREFIX_LEN, n->partial_len), key_len - depth);

  for (i = 0; i < max_cmp; i++)
    {
      if (n->partial[i] != key[depth + i])
        return i;
    }

  return i;
}


/*
 * Calculates the index at which the prefixes mismatch
 */
static gint
prefix_mismatch (const Node   *n,
                 const guchar *key,
                 gint          key_len,
                 int           depth)
{
  gint max_cmp;
  gint i;

  max_cmp = MIN (MIN (MAX_PREFIX_LEN, n->partial_len), key_len - depth);

  for (i = 0; i < max_cmp; i++)
    {
      if (n->partial[i] != key[depth + i])
        return i;
    }

  /* If the prefix is short we can avoid finding a leaf */
  if (n->partial_len > MAX_PREFIX_LEN)
    {
      Leaf *leaf;
      guchar *leaf_key;

      /* Prefix is longer than what we've checked, find a leaf */
      leaf = minimum (n);
      leaf_key = LEAF_KEY (leaf);
      max_cmp = MIN (leaf->key_len, key_len) - depth;

      for (; i < max_cmp; i++)
        {
          if (leaf_key[depth + i] != key[depth + i])
            return i;
        }
    }

  return i;
}

static void
copy_header (Node *dest,
             Node *src)
{
  dest->num_children = src->num_children;
  dest->partial_len = src->partial_len;

  memcpy (dest->partial,
          src->partial,
          MIN (MAX_PREFIX_LEN, src->partial_len));
}

static void
add_child_256 (Node256   *n,
               guchar     c,
               gpointer   child)
{
  n->n.num_children++;
  n->children[c] = child;
}

static void
add_child_48 (Node48    *n,
              Node     **ref,
              guchar     c,
              gpointer   child)
{
  if (n->n.num_children < 48)
    {
      gint pos = 0;

      while (n->children[pos])
        pos++;

      n->children[pos] = child;
      n->keys[c] = pos + 1;
      n->n.num_children++;
    }
  else
    {
      Node256 *new_node;

      new_node = node_new (NODE_256);

      for (int i=0;i<256;i++)
        {
          if (n->keys[i])
            new_node->children[i] = n->children[n->keys[i] - 1];
        }

      copy_header ((Node*) new_node, (Node*) n);

      *ref = (Node*) new_node;

      add_child_256 (new_node, c, child);

      g_free (n);
    }
}

static void
add_child_16 (Node16    *n,
              Node     **ref,
              guchar     c,
              gpointer   child)
{
  if (n->n.num_children < 16)
    {
      __m128i cmp;
      guint mask, bitfield, i;

      /* Compare the key to all 16 stored keys */
      cmp = _mm_cmplt_epi8 (_mm_set1_epi8 (c), _mm_loadu_si128 ((__m128i*) n->keys));

      /* Use a mask to ignore children that don't exist */
      mask = (1 << n->n.num_children) - 1;
      bitfield = _mm_movemask_epi8 (cmp) & mask;

      if (bitfield)
        {
          i = __builtin_ctz (bitfield);

          memmove (n->keys + i + 1,
                   n->keys + i,
                   n->n.num_children - i);

          memmove (n->children + i + 1,
                   n->children + i,
                  (n->n.num_children - i) * sizeof (gpointer));
        }
      else
        {
          i = n->n.num_children;
        }

      /* Set the child */
      n->keys[i] = c;
      n->children[i] = child;
      n->n.num_children++;
    }
  else
    {
      Node48 *new_node;
      guint i;

      new_node = node_new (NODE_48);

      /* Copy the child pointers and populate the key map */
      memcpy (new_node->children,
              n->children,
              n->n.num_children * sizeof (gpointer));

      for (i = 0; i < n->n.num_children; i++)
        new_node->keys[n->keys[i]] = i + 1;

      copy_header ((Node*) new_node, (Node*) n);

      *ref = (Node*) new_node;

      add_child_48 (new_node, ref, c, child);

      g_free (n);
   }
}

static void
add_child_4 (Node4     *n,
             Node     **ref,
             guchar     c,
             gpointer   child)
{
  if (n->n.num_children < 4)
    {
      gint i;

      for (i = 0; i < n->n.num_children; i++)
          if (c < n->keys[i])
            break;

      /* Shift to make room */
      memmove (n->keys + i + 1,
               n->keys + i,
               n->n.num_children - i);

      memmove (n->children + i + 1,
               n->children + i,
              (n->n.num_children - i) * sizeof (gpointer));

      /* Insert element */
      n->keys[i] = c;
      n->children[i] = child;
      n->n.num_children++;
    }
  else
    {
      Node16 *new_node;

      new_node = node_new (NODE_16);

      /* Copy the child pointers and the key map */
      memcpy (new_node->children,
              n->children,
              n->n.num_children * sizeof (gpointer));

      memcpy (new_node->keys,
              n->keys,
              n->n.num_children * sizeof (guchar));

      copy_header ((Node*) new_node, (Node*) n);

      *ref = (Node*)new_node;

      add_child_16 (new_node, ref, c, child);

      g_free (n);
    }
}

static void
add_child (Node      *n,
           Node     **ref,
           guchar     c,
           gpointer   child)
{
  switch (n->type)
    {
    case NODE_4:
      return add_child_4 ((Node4*) n, ref, c, child);

    case NODE_16:
      return add_child_16 ((Node16*) n, ref, c, child);

    case NODE_48:
      return add_child_48 ((Node48*) n, ref, c, child);

    case NODE_256:
      return add_child_256 ((Node256*) n, c, child);

    default:
      g_assert_not_reached ();
    }
}

static gpointer
insert_recursive (Node          *n,
                  Node         **ref,
                  const guchar  *key,
                  gint           key_len,
                  gpointer       value,
                  gint           depth,
                  gboolean      *old)
{
  Node **child;
  Leaf *l;

  /* If we are at a NULL node, inject a leaf */
  if (!n)
    {
      *ref = (Node*) SET_LEAF (leaf_new (key, key_len, value));
      return NULL;
    }

  /* If we are at a leaf, we need to replace it with a node */
  if (IS_LEAF (n))
    {
      Node4 *new_node;
      Leaf *leaf, *new_leaf;
      gint longest_prefix;
      guchar *leaf_key;

      leaf = LEAF_RAW (n);

      /* Check if we are updating an existing value */
      if (leaf_matches (leaf, key, key_len))
        {
          gpointer old_val;

          old_val = leaf->value;
          leaf->value = value;

          *old = TRUE;

          return old_val;
        }

      /* we must split the leaf into a Node4 */
      new_node = node_new (NODE_4);

      /* Create a new leaf */
      new_leaf = leaf_new (key, key_len, value);

      // Determine longest prefix
      longest_prefix = longest_common_prefix (leaf, new_leaf, depth);

      new_node->n.partial_len = longest_prefix;

      memcpy(new_node->n.partial,
             key + depth,
             MIN (MAX_PREFIX_LEN, longest_prefix));

      /* Add the leafs to the new Node4 */
      *ref = (Node*) new_node;

      leaf_key = LEAF_KEY (leaf);
      add_child_4 (new_node,
                   ref,
                   leaf_key[depth + longest_prefix],
                   SET_LEAF (leaf));

      leaf_key = LEAF_KEY (new_leaf);
      add_child_4 (new_node,
                   ref,
                   leaf_key[depth + longest_prefix],
                   SET_LEAF (new_leaf));

      return NULL;
    }

  /* Check if given node has a prefix */
  if (n->partial_len)
    {
      Node4 *new_node;
      Leaf *new_leaf;
      gint prefix_diff;

      /* Determine if the prefixes differ, since we need to split */
      prefix_diff = prefix_mismatch (n, key, key_len, depth);

      if ((guint32) prefix_diff >= n->partial_len)
        {
          depth += n->partial_len;
          goto recurse;
        }

      /* Create a new node */
      new_node = node_new (NODE_4);
      new_node->n.partial_len = prefix_diff;

      memcpy (new_node->n.partial,
              n->partial,
              MIN (MAX_PREFIX_LEN, prefix_diff));

      *ref = (Node*) new_node;

      /* Adjust the prefix of the old node */
      if (n->partial_len > MAX_PREFIX_LEN)
        {
          Leaf *leaf;
          guchar *leaf_key;

          n->partial_len -= prefix_diff + 1;
          leaf = minimum (n);
          leaf_key = LEAF_KEY (leaf);

          add_child_4 (new_node, ref, leaf_key[depth + prefix_diff], n);

          memcpy (n->partial,
                  leaf_key + depth + prefix_diff + 1,
                  MIN (MAX_PREFIX_LEN, n->partial_len));

        }
      else
        {
          add_child_4 (new_node, ref, n->partial[prefix_diff], n);

          n->partial_len -= prefix_diff + 1;

          memmove (n->partial,
                   n->partial + prefix_diff + 1,
                   MIN (MAX_PREFIX_LEN, n->partial_len));
        }

      /* Insert the new leaf */
      new_leaf = leaf_new (key, key_len, value);

      add_child_4 (new_node, ref, key[depth + prefix_diff], SET_LEAF(new_leaf));

      return NULL;
    }

recurse:

  /* Find a child to recurse to */
  child = find_child (n, key[depth]);

  if (child)
    {
      return insert_recursive (*child,
                               child,
                               key,
                               key_len,
                               value,
                               depth + 1,
                               old);
    }


  /* No child, node goes within us */
  l = leaf_new (key, key_len, value);

  add_child (n, ref, key[depth], SET_LEAF(l));

  return NULL;
}

static void
remove_child_256 (Node256  *n,
                  Node    **ref,
                  guchar    c)
{
  n->children[c] = NULL;
  n->n.num_children--;

  /*
   * Resize to a Node48 on underflow, not immediately to prevent
   * trashing if we sit on the 48/49 boundary
   */
  if (n->n.num_children == 37)
    {
      Node48 *new_node;
      gint pos, i;

      new_node = node_new (NODE_48);
      *ref = (Node*) new_node;

      copy_header ((Node*) new_node, (Node*) n);

      pos = 0;
      for (i = 0; i < 256; i++)
        {
          if (n->children[i])
            {
              new_node->children[pos] = n->children[i];
              new_node->keys[i] = pos + 1;
              pos++;
            }
        }

      g_free(n);
    }
}

static void
remove_child_48 (Node48  *n,
                 Node   **ref,
                 guchar   c)
{
  gint pos;

  pos = n->keys[c];

  n->keys[c] = 0;
  n->children[pos - 1] = NULL;
  n->n.num_children--;

  if (n->n.num_children == 12)
    {
      Node16 *new_node;
      gint child, i;

      new_node = node_new (NODE_16);
      *ref = (Node*) new_node;

      copy_header ((Node*) new_node, (Node*) n);

      child = 0;

      for (i = 0; i < 256; i++)
        {
          pos = n->keys[i];

          if (pos)
            {
              new_node->keys[child] = i;
              new_node->children[child] = n->children[pos - 1];
              child++;
            }
        }

      g_free(n);
    }
}

static void
remove_child_16 (Node16  *n,
                 Node   **ref,
                 Node   **l)
{
  gint pos;

  pos = l - n->children;

  memmove (n->keys + pos,
           n->keys + pos + 1,
           n->n.num_children - 1 - pos);

  memmove (n->children + pos,
           n->children + pos + 1,
           (n->n.num_children - 1 - pos) * sizeof (gpointer));

  n->n.num_children--;

  /* Fall back to a 4-child node */
  if (n->n.num_children == 3)
    {
      Node4 *new_node;

      new_node = node_new (NODE_4);
      *ref = (Node*) new_node;

      copy_header ((Node*) new_node, (Node*) n);

      memcpy (new_node->keys, n->keys, 4);
      memcpy (new_node->children, n->children, 4 * sizeof (gpointer));

      g_free(n);
    }
}

static void
remove_child_4 (Node4  *n,
                Node  **ref,
                Node  **l)
{
  gint pos;

  pos = l - n->children;

  memmove (n->keys + pos,
           n->keys + pos + 1,
           n->n.num_children - 1 - pos);

  memmove (n->children + pos,
           n->children + pos + 1,
           (n->n.num_children - 1 - pos) * sizeof(gpointer));

  n->n.num_children--;

  /* Remove nodes with only a single child */
  if (n->n.num_children == 1)
    {
      Node *child = n->children[0];

      if (!IS_LEAF (child))
        {
          gint prefix;

          /* Concatenate the prefixes */
          prefix = n->n.partial_len;

          if (prefix < MAX_PREFIX_LEN)
            {
              n->n.partial[prefix] = n->keys[0];
              prefix++;
            }

          if (prefix < MAX_PREFIX_LEN)
            {
              gint sub_prefix;

              sub_prefix = MIN (child->partial_len, MAX_PREFIX_LEN - prefix);

              memcpy (n->n.partial + prefix, child->partial, sub_prefix);

              prefix += sub_prefix;
            }

          /* Store the prefix in the child */
          memcpy (child->partial, n->n.partial, MIN (prefix, MAX_PREFIX_LEN));

          child->partial_len += n->n.partial_len + 1;
        }

      *ref = child;
      g_free(n);
    }
}

static void
remove_child (Node    *n,
              Node   **ref,
              guchar   c,
              Node   **l)
{
  switch (n->type)
    {
    case NODE_4:
      return remove_child_4 ((Node4*) n, ref, l);

    case NODE_16:
      return remove_child_16 ((Node16*) n, ref, l);

    case NODE_48:
      return remove_child_48 ((Node48*) n, ref, c);

    case NODE_256:
      return remove_child_256 ((Node256*) n, ref, c);

    default:
        g_assert_not_reached ();
    }
}

static Leaf*
remove_recursive (Node          *n,
                  Node         **ref,
                  const guchar  *key,
                  gint           key_len,
                  gint           depth)
{
  Node **child;

  if (!n)
    return NULL;

  if (IS_LEAF (n))
    {
      Leaf *l = LEAF_RAW (n);

      if (leaf_matches (l, key, key_len))
        {
          *ref = NULL;
          return l;
        }

      return NULL;
    }

  /* Bail if the prefix does not match */
  if (n->partial_len)
    {
      gint prefix_len;

      prefix_len = check_prefix (n, key, key_len, depth);

      if (prefix_len != MIN (MAX_PREFIX_LEN, n->partial_len))
        return NULL;

      depth = depth + n->partial_len;
    }

  /* Find child node */
  child = find_child (n, key[depth]);

  if (!child)
    return NULL;

  /* If the child is leaf, delete from this node */
  if (IS_LEAF (*child))
    {
      Leaf *l;

      l = LEAF_RAW(*child);

      if (leaf_matches (l, key, key_len))
        {
          remove_child (n, ref, key[depth], child);
          return l;
        }

      return NULL;
    }
  else
    {
      return remove_recursive (*child, child, key, key_len, depth + 1);
    }
}

static gboolean
iter_recursive (Node        *n,
                RadixTreeCb  cb,
                gpointer     user_data)
{
  gint res, i;

  if (!n)
    return GW_RADIX_TREE_ITER_CONTINUE;

  if (IS_LEAF (n))
    {
      Leaf *l = LEAF_RAW (n);
      return cb (LEAF_KEY (l), l->key_len, l->value, user_data);
    }

  switch (n->type)
    {
    case NODE_4:
        for (i = 0; i < n->num_children; i++)
          {
            res = iter_recursive (((Node4*) n)->children[i], cb, user_data);

            if (res)
              return res;
          }
        break;

    case NODE_16:
        for (i = 0; i < n->num_children; i++)
          {
            res = iter_recursive (((Node16*) n)->children[i], cb, user_data);

            if (res)
              return res;
          }
        break;

    case NODE_48:
        for (i = 0; i < 256; i++)
          {
            gint idx;

            idx = ((Node48*) n)->keys[i];

            if (idx == 0)
              continue;

            res = iter_recursive (((Node48*) n)->children[idx - 1], cb, user_data);

            if (res)
              return res;
          }
        break;

    case NODE_256:
        for (i = 0; i < 256; i++)
          {
            if (!((Node256*) n)->children[i])
              continue;

            res = iter_recursive (((Node256*) n)->children[i], cb, user_data);

            if (res)
              return res;
          }
        break;

    default:
        g_assert_not_reached ();
    }

  return GW_RADIX_TREE_ITER_CONTINUE;
}

static void
gw_radix_tree_free (GwRadixTree *self)
{
  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  destroy_node_recursive (self->root);

  g_slice_free (GwRadixTree, self);
}

/**
 * gw_radix_tree_new:
 *
 * Creates a new #GwRadixTree.
 *
 * Returns: (transfer full): a new #GwRadixTree.
 *
 * Since: 0.1.0
 */
GwRadixTree *
gw_radix_tree_new (void)
{
  GwRadixTree *self;

  self = g_slice_new0 (GwRadixTree);
  self->ref_count = 1;

  return self;
}

/**
 * gw_radix_tree_copy:
 * @self: a #GwRadixTree
 *
 * Creates a deep copy of @self.
 *
 * Returns: (transfer full): a new #GwRadixTree
 *
 * Since: 0.1
 */
GwRadixTree*
gw_radix_tree_copy (GwRadixTree *self)
{
  GwRadixTree *copy;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  copy = gw_radix_tree_new ();

  return copy;
}

GwRadixTree*
gw_radix_tree_ref (GwRadixTree *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
gw_radix_tree_unref (GwRadixTree *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    gw_radix_tree_free (self);
}

/**
 * gw_radix_tree_new_with_free_func:
 * @destroy_func: (nullable): A function to free the data elements, or %NULL.
 *
 * Creates a new #GwRadixTree with @destroy_func.
 *
 * Returns: (transfer full): a new #GwRadixTree.
 *
 * Since: 0.1.0
 */
GwRadixTree*
gw_radix_tree_new_with_free_func (GDestroyNotify destroy_func)
{
  GwRadixTree *self;

  self = g_slice_new0 (GwRadixTree);
  self->ref_count = 1;
  self->destroy_func = destroy_func;

  return self;
}

/**
 * gw_radix_tree_contains:
 * @self: a #GwRadixTree
 * @key: the key to search for.
 * @key_length: the length of @key, or -1
 *
 * Checks if @self contains @key.
 *
 * Returns: %TRUE if @key is in @self, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_radix_tree_contains (GwRadixTree *self,
                        const gchar *key,
                        gsize        key_length)
{
  gboolean found;

  g_return_val_if_fail (self, FALSE);

  gw_radix_tree_lookup (self, key, key_length, &found);

  return found;
}

/**
 * gw_radix_tree_lookup:
 * @tree: a #GwRadixTree
 * @key: the key to look for
 * @key_length: the length of @key, or -1
 * @found: whether the value was found or not
 *
 * Look for the given key on the tree, and return it's associated value.
 * It'll return NULL when the value is not found. Use the @found argument
 * to differentiate between saved NULL values.
 *
 * Returns: the associated value of the key, or NULL.
 *
 * Since: 0.1.0
 */
gpointer
gw_radix_tree_lookup (GwRadixTree *self,
                      const gchar *key,
                      gsize        key_length,
                      gboolean    *found)
{
  Node **child;
  Node *n;
  Leaf *l;
  gint depth;

  g_return_val_if_fail (self, NULL);

  n = self->root;
  depth = 0;

  if (key_length == -1)
    key_length = strlen (key);

  l = NULL;

  while (n)
    {
      if (IS_LEAF (n))
        {
          n = (Node*) LEAF_RAW (n);

          if (leaf_matches ((Leaf*) n, (guchar*) key, key_length))
            l = (Leaf*) n;

          break;
        }

      /* Bail if the prefix does not match */
      if (n->partial_len)
        {
          gint prefix_len;

          prefix_len = check_prefix (n, (guchar*) key, key_length, depth);

          if (prefix_len != MIN (MAX_PREFIX_LEN, n->partial_len))
            {
              l = NULL;
              break;
            }

          depth = depth + n->partial_len;
        }

      /* Recursively search */
      child = find_child (n, key[depth]);
      n = child ? *child : NULL;
      depth++;
    }


  if (found)
    *found = l != NULL;

  return l ? l->value : NULL;
}

/**
 * gw_radix_tree_insert:
 * @tree: the #GwRadixTree to add to
 * @key: string user as identifier of the value. Must be a UTF-8 valid string.
 * @key_length: the length of @key, or -1
 * @value: (nullable): user data to be stored. Can be %NULL.
 *
 * Inserts a new value in the @tree. If the key is already in the tree, @value
 * will be overwritten.
 *
 * Returns: %TRUE if a new node was added, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_radix_tree_insert (GwRadixTree *self,
                      const gchar *key,
                      gsize        key_length,
                      gpointer     value)
{
  gpointer old_val;
  gboolean old;

  g_return_val_if_fail (self, FALSE);

  old = FALSE;

  old_val = insert_recursive (self->root,
                              &self->root,
                              (const guchar*) key,
                              key_length == -1 ? strlen (key) : key_length,
                              value,
                              0,
                              &old);

  if (!old_val)
    self->size++;

  return !old_val;
}

/**
 * gw_radix_tree_get_keys:
 * @tree: a #GwRadixTree
 *
 * Retrieve all keys from the tree
 *
 * Returns: (transfer-full): a newly-allocated #GList with the keys.
 *
 * Since: 0.1.0
 */
GStrv
gw_radix_tree_get_keys (GwRadixTree *self)
{
  GPtrArray *result;

  g_return_val_if_fail (self, NULL);

  result = g_ptr_array_new ();

  gw_radix_tree_iter (self, get_keys_cb, result);

  /* Tail NULL */
  g_ptr_array_add (result, NULL);

  return (GStrv) g_ptr_array_free (result, FALSE);
}

/**
 * gw_radix_tree_get_values:
 * @tree: a #GwRadixTree
 *
 * Retrieve all values from the tree
 *
 * Returns: (transfer-full): a newly-allocated #GList with the values.
 *
 * Since: 0.1.0
 */
GPtrArray*
gw_radix_tree_get_values (GwRadixTree *self)
{
  GPtrArray *result;

  g_return_val_if_fail (self, NULL);

  result = g_ptr_array_new ();

  gw_radix_tree_iter (self, get_values_cb, result);

  return result;
}

/**
 * gw_radix_tree_iter:
 * @tree: the #GwRadixTree to be traversed
 * @callback: user-defined function to call on each value
 * @user_data:
 *
 * Traverse the tree calling @callback on each saved value.
 *
 * It iterates until @callback returns TR
 *
 * Returns: the same result as @callback.
 *
 * Since: 0.1.0
 */
gboolean
gw_radix_tree_iter (GwRadixTree *self,
                    RadixTreeCb  callback,
                    gpointer     user_data)
{
  g_return_val_if_fail (self, FALSE);

  return iter_recursive (self->root, callback, user_data);
}

/**
 * gw_radix_tree_remove:
 * @tree: the #GwRadixTree
 * @key: the key to be removed. Must be UTF-8 valid.
 * @key_length: the length of @key, or -1
 *
 * Removes the given key from the tree. The key must be
 * a UTF-8 validated string.
 *
 * Since: 0.1.0
 */
void
gw_radix_tree_remove (GwRadixTree *self,
                      const gchar *key,
                      gsize        key_length)
{
  Leaf *removed;

  g_return_if_fail (self);

  removed = remove_recursive (self->root,
                              &self->root,
                              (guchar*) key,
                              key_length == -1 ? strlen (key) : key_length,
                              0);

  if (removed)
    {
      if (self->destroy_func && removed->value)
        self->destroy_func (removed->value);

      g_free (removed);

      self->size--;
    }
}

/**
 * gw_radix_tree_steal:
 * @tree: the #GwRadixTree
 * @key: the key to be removed. Must be UTF-8 valid.
 * @key_length: the length of @key, or -1
 *
 * Removes the given key from the tree without calling the destroy
 * function if any.
 *
 * Since: 0.1.0
 */
void
gw_radix_tree_steal (GwRadixTree *self,
                     const gchar *key,
                     gsize        key_length)
{
  Leaf *removed;

  g_return_if_fail (self);

  removed = remove_recursive (self->root,
                              &self->root,
                              (guchar*) key,
                              key_length == -1 ? strlen (key) : key_length,
                              0);

  if (removed)
    {
      g_free (removed);
      self->size--;
    }
}

/**
 * gw_radix_tree_clear:
 * @self: the #GwRadixTree to be cleared.
 *
 * Clear out the nodes from the tree.
 *
 * Since: 0.1.0
 */
void
gw_radix_tree_clear (GwRadixTree *self)
{
  g_return_if_fail (self);

  destroy_node_recursive (self->root);
  self->root = NULL;
  self->size = 0;
}

/**
 * gw_radix_tree_get_size:
 * @self: the #GwRadixTree to count.
 *
 * Retrieves the number of elements of @tree.
 *
 * Returns: the number of elements.
 *
 * Since: 0.1.0
 */
gint
gw_radix_tree_get_size (GwRadixTree *self)
{
  g_return_val_if_fail (self, -1);

  return self->size;
}
