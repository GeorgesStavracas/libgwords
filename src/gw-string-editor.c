/* gw-string-editor.c
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

#include "gw-string.h"
#include "gw-string-editor.h"

#include <string.h>

/* The likelyhood (%) a node will have height (n+1) instead of n */
#define ROPE_BIAS       25

/* Rope will stop being efficient after the string is 2 ^ ROPE_MAX_HEIGHT nodes */
#define ROPE_MAX_HEIGHT 60

/* Magical value that performs well */
#define ROPE_NODE_SIZE  136

#define ROPE_SIZE (sizeof (GwStringEditor) + sizeof (RopeNode) * ROPE_MAX_HEIGHT)

typedef struct _SkipNode SkipNode;
typedef struct _RopeNode RopeNode;

struct _SkipNode
{
  /* Number of characters between the start of the current node and the start of next */
  gsize               skip_size;
  RopeNode           *node;
};

struct _RopeNode
{
  gchar               str[ROPE_NODE_SIZE];

  /* The number of bytes in str in use */
  guint16             n_bytes;

  /* This is the number of elements allocated in RopeNode.nexts */
  guint8              height;
  SkipNode            nexts[];
};

struct _GwStringEditor
{
  guint               ref_count;

  gsize               n_chars;
  gsize               n_bytes;

  RopeNode            head;
};

typedef struct
{
  SkipNode           s[ROPE_MAX_HEIGHT];
} RopeIter;

G_DEFINE_BOXED_TYPE (GwStringEditor, gw_string_editor, gw_string_editor_ref, gw_string_editor_unref)

/*
 * Node methods
 */

static RopeNode*
rope_node_new (GwStringEditor *self,
               guint8          height)
{
  RopeNode *node;

  node = g_malloc (sizeof (RopeNode) + height * sizeof (SkipNode));
  node->height = height;

  return node;
}


/*
 * Auxiliary methods
 */

static inline gsize
codepoint_size (guint8 byte)
{
  if (byte == 0)
    return G_MAXSIZE; /*  NULL byte */
  else if (byte <= 0x7f)
    return 1; /* 0x74 = 0111 1111 */
  else if (byte <= 0xbf)
    return G_MAXSIZE; /* 1011 1111. Invalid for a starting byte */
  else if (byte <= 0xdf)
    return 2; /*  1101 1111 */
  else if (byte <= 0xef)
    return 3; /*  1110 1111 */
  else if (byte <= 0xf7)
    return 4; /*  1111 0111 */
  else if (byte <= 0xfb)
    return 5; /*  1111 1011 */
  else if (byte <= 0xfd)
    return 6; /*  1111 1101 */
  else
    return G_MAXSIZE;
}

static gsize
count_bytes_in_utf8 (const GwString *str,
                     gsize           n_chars)
{
  const GwString *p;
  guint32 i;

  p = str;

  for (i = 0; i < n_chars; i++)
    p += codepoint_size (*p);

  return p - str;
}

RopeNode*
iter_at_char_pos (GwStringEditor *self,
                  gsize           pos,
                  RopeIter       *iter)
{
  RopeNode *node;
  guint8 height;
  gsize offset, skip;

  node = &self->head;
  height = self->head.height - 1;
  offset = pos;

  while (TRUE)
    {
      skip = node->nexts[height].skip_size;

      if (offset > skip)
        {
          /* Go right */
          offset -= skip;
          node = node->nexts[height].node;
        }
      else
        {
          /* Go down */
          iter->s[height].skip_size = offset;
          iter->s[height].node = node;

          if (height == 0)
            break;

          height--;
        }
    }

  return node;
}

static guint8
random_height (void)
{
  /*
   * TODO: This function is horribly inefficient. I'm throwing away heaps of entropy, and
   * the mod could be replaced by some clever shifting.
   *
   * However, random_height barely appears in the profiler output - so its probably
   * not worth investing the time to optimise.
   */

  guint8 height = 1;

  /*
   * The root node's height is the height of the largest node + 1, so the largest
   * node can only have ROPE_MAX_HEIGHT - 1.
   */
  while (height < ROPE_MAX_HEIGHT - 1 &&
         g_random_int_range (0, 100) < ROPE_BIAS)
    {
      height++;
    }

  return height;
}

static void
delete_at_iter (GwStringEditor *self,
                RopeNode       *node,
                RopeIter       *iter,
                gsize           length)
{
  gsize offset;

  self->n_chars -= length;
  offset = iter->s[0].skip_size;

  while (length)
    {
      gsize n_chars;
      gsize removed;
      guint i;

      /* End of the current node. Skip to the start of the next one */
      if (offset == node->nexts[0].skip_size)
        {
          node = iter->s[0].node->nexts[0].node;
          offset = 0;
        }

      n_chars = node->nexts[0].skip_size;
      removed = MIN (length, n_chars - offset);

      if (removed < n_chars || node == &self->head)
        {
          gsize trailing_bytes;
          gsize removed_bytes;
          gsize leading_bytes;

          /* Just trim this node down to size */
          leading_bytes = count_bytes_in_utf8 (node->str, offset);
          removed_bytes = count_bytes_in_utf8 (&node->str[leading_bytes], removed);
          trailing_bytes = node->n_bytes - leading_bytes - removed_bytes;

          if (trailing_bytes)
            {
              memmove (&node->str[leading_bytes],
                       &node->str[leading_bytes + removed_bytes],
                       trailing_bytes);
            }

          node->n_bytes -= removed_bytes;
          self->n_bytes -= removed_bytes;

          for (i = 0; i < node->height; i++)
            node->nexts[i].skip_size -= removed;
        }
      else
        {
          RopeNode *next;

          /* Remove the node from the list */
          for (i = 0; i < node->height; i++)
            {
              iter->s[i].node->nexts[i].node = node->nexts[i].node;
              iter->s[i].node->nexts[i].skip_size += node->nexts[i].skip_size - removed;
            }

          self->n_bytes -= node->n_bytes;

          /* TODO: Recycle node */
          next = node->nexts[0].node;

          g_free (node);

          node = next;
        }

      for (; i < self->head.height; i++)
        iter->s[i].node->nexts[i].skip_size -= removed;

      length -= removed;
    }
}

static void
insert_at (GwStringEditor *self,
           RopeIter       *iter,
           const GwString *str,
           gsize           n_bytes,
           gsize           n_chars)
{
  RopeNode *new_node;
  guint8 max_height;
  guint8 new_height;
  gint i;

  max_height = self->head.height;
  new_height = random_height ();

  new_node = rope_node_new (self, new_height);
  new_node->n_bytes = n_bytes;

  memcpy (new_node->str, str, n_bytes);

  /* Max height (the rope's head's height) must be 1+ the height of the largest node */
  while (max_height <= new_height)
    {
      self->head.height++;
      self->head.nexts[max_height] = self->head.nexts[max_height - 1];

      // This is the position (offset from the start) of the rope.
      iter->s[max_height] = iter->s[max_height - 1];
      max_height++;
    }

  /* Fill in the new node's nexts array */
  for (i = 0; i < new_height; i++)
    {
      SkipNode *prev_skip;

      prev_skip = &iter->s[i].node->nexts[i];

      new_node->nexts[i].node = prev_skip->node;
      new_node->nexts[i].skip_size = n_chars + prev_skip->skip_size - iter->s[i].skip_size;

      prev_skip->node = new_node;
      prev_skip->skip_size = iter->s[i].skip_size;

      /* & move the iterator to the end of the newly inserted node */
      iter->s[i].node = new_node;
      iter->s[i].skip_size = n_chars;
    }

  for (; i < max_height; i++)
    {
      iter->s[i].node->nexts[i].skip_size += n_chars;
      iter->s[i].skip_size += n_chars;
    }

  self->n_chars += n_chars;
  self->n_bytes += n_bytes;
}

static inline void
update_offset_list (GwStringEditor *self,
                    RopeIter       *iter,
                    gsize           n_chars)
{
  guint i;

  for (i = 0; i < self->head.height; i++)
    iter->s[i].node->nexts[i].skip_size += n_chars;
}

gboolean
insert_at_iter (GwStringEditor *self,
                RopeNode       *node,
                RopeIter       *iter,
                GwString       *str)
{
  RopeNode *next;
  gboolean insert_here;
  gsize n_inserted_bytes;
  gsize offset_bytes;
  gsize offset;

  if (!g_utf8_validate (str, -1, NULL))
    return FALSE;

  offset_bytes = 0;
  offset = iter->s[0].skip_size;

  if (offset)
    offset_bytes = count_bytes_in_utf8 (node->str, offset);

  /*
   * We might be able to insert the new data into the current node, depending on
   * how big it is. We'll count the bytes, and also check that its valid utf8.
   */
  n_inserted_bytes = strlen (str);

  /*  Can we insert into the current node? */
  insert_here = node->n_bytes + n_inserted_bytes <= ROPE_NODE_SIZE;

  /*  Can we insert into the subsequent node? */
  next = NULL;

  if (!insert_here && offset_bytes == node->n_bytes)
    {
      next = node->nexts[0].node;

      /*
       * We can insert into the subsequent node if:
       * - We can't insert into the current node
       * - There is the next node to insert into
       * - The insert would be at the start of the next node
       * - There's room in the next node
       */

      if (next && next->n_bytes + n_inserted_bytes <= ROPE_NODE_SIZE)
        {
          guint32 i;

          offset = 0;
          offset_bytes = 0;
          insert_here = TRUE;

          for (i = 0; i < next->height; i++)
            iter->s[i].node = next;

          node = next;
        }
    }

  if (insert_here)
    {
      gsize n_inserted_chars;

      /* First move the current bytes later on in the string */
      if (offset_bytes < node->n_bytes)
        {
          memmove (&node->str[offset_bytes + n_inserted_bytes],
                   &node->str[offset_bytes],
                   node->n_bytes - offset_bytes);
        }

      /* Then copy in the string bytes */
      memcpy (&node->str[offset_bytes], str, n_inserted_bytes);
      node->n_bytes += n_inserted_bytes;

      self->n_bytes += n_inserted_bytes;

      n_inserted_chars = g_utf8_strlen (str, -1);

      self->n_chars += n_inserted_chars;

      /* Update the offset list */
      update_offset_list (self, iter, n_inserted_chars);
    }
  else
    {
      gsize n_end_chars;
      gsize n_end_bytes;
      gsize str_offset;

      n_end_bytes = node->n_bytes - offset_bytes;
      str_offset = 0;

      /* There isn't room. We'll need to add at least one new node to the rope */

      if (n_end_bytes)
        {
          /*
           * We'll pretend like the character have been deleted from the node, while leaving
           * the bytes themselves there (for later).
           */
          node->n_bytes = offset_bytes;
          n_end_chars = node->nexts[0].skip_size - offset;

          update_offset_list (self, iter, -n_end_chars);

          self->n_chars -= n_end_chars;
          self->n_bytes -= n_end_bytes;
        }

      /*
       * Now we insert new nodes containing the new character data. The data must be broken into
       * pieces of with a maximum size of ROPE_NODE_SIZE. Node boundaries must not occur in the
       * middle of a utf8 codepoint.
       */
      while (str_offset < n_inserted_bytes)
        {
          gsize new_node_bytes;
          gsize new_node_chars;

          new_node_bytes = 0;
          new_node_chars = 0;

          while (str_offset + new_node_bytes < n_inserted_bytes)
            {
              gsize size_codepoint;

              size_codepoint = codepoint_size (str[str_offset + new_node_bytes]);

              if (size_codepoint + new_node_bytes > ROPE_NODE_SIZE)
                break;

              new_node_bytes += size_codepoint;
              new_node_chars++;
            }

          insert_at (self, iter, &str[str_offset], new_node_bytes, new_node_chars);

          str_offset += new_node_bytes;
        }

      if (n_end_bytes > 0)
        insert_at (self, iter, &node->str[offset_bytes], n_end_bytes, n_end_chars);
    }

  return TRUE;
}

gboolean
insert (GwStringEditor *self,
        gsize           pos,
        GwString       *str)
{
  RopeNode *node;
  RopeIter iter;

  pos = MIN (pos, self->n_chars);

  /* Search for the node where we'll insert the string */
  node = iter_at_char_pos (self, pos, &iter);

  return insert_at_iter (self, node, &iter, str);
}

GwStringEditor*
gw_string_editor_copy (GwStringEditor *self)
{
  GwStringEditor *copy;
  RopeNode *nodes[ROPE_MAX_HEIGHT];
  RopeNode *n;
  guint i;

  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  copy = g_malloc (ROPE_SIZE);

  /* Just copy most of the head's data. Note this won't copy the nexts list in head */
  *copy = *self;

  for (i = 0; i < self->head.height; i++)
    {
      nodes[i] = &copy->head;
      copy->head.nexts[i] = self->head.nexts[i];
    }

  for (n = self->head.nexts[0].node; n; n = n->nexts[0].node)
    {
      RopeNode *n2;
      gsize height;

      height = n->height;
      n2 = g_malloc (sizeof (RopeNode) + height * sizeof (SkipNode));

      /* Would it be faster to just *n2 = *n; ? */
      n2->n_bytes = n->n_bytes;
      n2->height = height;

      memcpy (n2->str, n->str, n->n_bytes);
      memcpy (n2->nexts, n->nexts, height * sizeof (SkipNode));

      for (i = 0; i < height; i++)
        {
          nodes[i]->nexts[i].node = n2;
          nodes[i] = n2;
        }
    }

  return copy;
}

static void
gw_string_editor_free (GwStringEditor *self)
{
  RopeNode *next, *n;

  g_assert (self);
  g_assert_cmpint (self->ref_count, ==, 0);

  for (n = self->head.nexts[0].node; n; n = next)
    {
      next = n->nexts[0].node;
      g_free (n);
    }

  g_free (self);
}

GwStringEditor *
gw_string_editor_ref (GwStringEditor *self)
{
  g_return_val_if_fail (self, NULL);
  g_return_val_if_fail (self->ref_count, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
gw_string_editor_unref (GwStringEditor *self)
{
  g_return_if_fail (self);
  g_return_if_fail (self->ref_count);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    gw_string_editor_free (self);
}

GwStringEditor*
gw_string_editor_new (GwString *str)
{
  GwStringEditor *self;

  self = g_malloc0 (ROPE_SIZE);
  self->ref_count = 1;
  self->head.height = 1;
  self->head.n_bytes = 0;
  self->head.nexts[0].node = NULL;
  self->head.nexts[0].skip_size = 0;

  if (str)
    insert (self, 0, str);

  return self;
}

/**
 * gw_string_editor_insert:
 * @self: a #GwStringEditor
 * @str: the string to be added
 * @position: position in @self
 *
 * Adds @str at @position. Going beyond the limits of the string
 * won't do anything.
 *
 * Returns: %TRUE on success, %FALSE otherwise.
 *
 * Since: 0.1
 */
gboolean
gw_string_editor_insert (GwStringEditor *self,
                         GwString       *str,
                         gsize           position)
{
  g_return_val_if_fail (self, FALSE);
  g_return_val_if_fail (str, FALSE);

  return insert (self, position, str);
}

/**
 * gw_string_editor_delete:
 * @self: a #GwStringEditor
 * @position: start of the range
 * @length: end of the range
 *
 * Deletes @length characters, starting from @position. Going
 * beyond the limits of the string won't do anything.
 *
 * Since: 0.1
 */
void
gw_string_editor_delete (GwStringEditor *self,
                         gsize           position,
                         gsize           length)
{
  RopeNode *node;
  RopeIter iter;

  g_return_if_fail (self);

  position = MIN (position, self->n_chars);
  length = MIN (length, self->n_chars - position);

  /* Search for the node where we'll remove the string */
  node = iter_at_char_pos (self, position, &iter);

  delete_at_iter (self, node, &iter, length);
}

/**
 * gw_string_editor_modify:
 * @self: a #GwStringEditor
 * @start: start of the range
 * @end: end of the range
 * @new_text: the new text of the range
 * @text_len: the size of @new_text, or -1 if %NULL-terminated
 *
 * Set the content between @start and @end to @new_text. If the new
 * text is smaller than the range, the size of @self is reduced. Likewise,
 * if the new text is bigger than the range, @self grows.
 *
 * Since: 0.1
 */
void
gw_string_editor_modify (GwStringEditor *self,
                         guint64         start,
                         guint64         end,
                         GwString       *new_text,
                         guint64         text_len)
{
  RopeNode *n;
  gint64 length, diff;
  gint64 offset;


  g_return_if_fail (self);
  g_return_if_fail (end >= start);

  start = MIN (start, self->n_chars);
  end = MIN (end, self->n_chars);
  length = end - start;

  if (text_len == -1)
    text_len = strlen (new_text);

  offset = 0;
  diff = text_len - length;

  /* Remove extra space... */
  if (diff < 0)
    {
      gw_string_editor_delete (self, start + text_len, -diff);
      end += diff;
    }
  else if (diff > 0)
    {
      gw_string_editor_insert (self, new_text + text_len - diff, end);
      end -= diff;
    }

  /* Just go over the nodes and update the values */
  for (n = &self->head; n; n = n->nexts[0].node)
    {
      gsize node_start;

      /* Skip nodes before start */
      if (offset + n->n_bytes < start)
        {
          offset += n->n_bytes;
          continue;
        }

      node_start = start - offset;

      if (offset + n->n_bytes >= end)
        {
          /* The entire range is contained in this node. Just copy the memory */
          memcpy (n->str + node_start, new_text, text_len);
        }
      else
        {
          gsize copied = 0;

          while (copied < text_len)
            {
              gsize to_be_copied = MIN (n->n_bytes, text_len - copied);

              memcpy (n->str + node_start, new_text + copied, to_be_copied);

              node_start = 0;
              copied += to_be_copied;

              n = n->nexts[0].node;
            }
        }
    }
}

/**
 * gw_string_editor_get_length:
 * @self: a #GwStringEditor
 *
 * Retrieves the number of UTF8 characters that @self contains.
 *
 * Returns: the number of characters of @self
 *
 * Since: 0.1
 */
gsize
gw_string_editor_get_length (GwStringEditor *self)
{
  g_return_val_if_fail (self, -1);

  return self->n_chars;
}

/**
 * gw_string_editor_to_string:
 * @self: a #GwStringEditor
 *
 * Converts the contents of @self into a #GwString.
 *
 * Returns: (transfer full)(nullable): a #GwString
 *
 * Since: 0.1
 */
GwString*
gw_string_editor_to_string (GwStringEditor *self)
{
  GwString *new_string;
  GwString *aux;
  RopeNode *n;
  gsize n_bytes;

  g_return_val_if_fail (self, NULL);

  n_bytes = self->n_bytes;

  if (self->n_bytes == 0)
    return NULL;

  new_string = gw_string_new_with_size (n_bytes);
  new_string[n_bytes] = '\0';

  /* Copy the node values */
  aux = new_string;

  for (n = &self->head; n; n = n->nexts[0].node)
    {
      memcpy (aux, n->str, n->n_bytes);
      aux += n->n_bytes;
    }

  return new_string;
}
