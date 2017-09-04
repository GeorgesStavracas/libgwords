/* gw-document.c
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

#include "gw-document.h"
#include "gw-language.h"
#include "gw-modifier.h"
#include "gw-segmenter.h"
#include "gw-string.h"
#include "gw-string-editor.h"
#include "gw-task-helper-private.h"

#include <string.h>

typedef struct
{
  GwLanguage         *language;
  GwString           *text;
} GwDocumentPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GwDocument, gw_document, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_LANGUAGE,
  PROP_TEXT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

/*
 * GTask thread functions
 */
static void
modify_in_thread_cb (GTask        *task,
                     gpointer      source_object,
                     gpointer      task_data,
                     GCancellable *cancellable)
{
  GwDocument *self;
  GwModifier *modifier;
  GError *error;
  gboolean result;

  self = source_object;
  modifier = task_data;
  error = NULL;

  result = gw_document_modify_sync (self, modifier, cancellable, &error);

  if (error)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_boolean (task, result);
}


static void
realize_in_thread_cb (GTask        *task,
                      gpointer      source_object,
                      gpointer      task_data,
                      GCancellable *cancellable)
{
  GwDocument *self;
  GwString *realization;
  GError *error;

  self = source_object;
  error = NULL;

  realization = gw_document_realize_sync (self, cancellable, &error);

  if (error)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_pointer (task, gw_string_ref (realization), (GDestroyNotify) gw_string_unref);
}

/*
 * GObject overrides
 */
static void
gw_document_dispose (GObject *object)
{
  GwDocument *self = (GwDocument *)object;
  GwDocumentPrivate *priv = gw_document_get_instance_private (self);

  g_clear_pointer (&priv->text, gw_string_unref);
  g_clear_object (&priv->language);

  G_OBJECT_CLASS (gw_document_parent_class)->dispose (object);
}

static void
gw_document_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GwDocument *self = GW_DOCUMENT (object);
  GwDocumentPrivate *priv = gw_document_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_value_set_object (value, priv->language);
      break;

    case PROP_TEXT:
      g_value_set_string (value, priv->text);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_document_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GwDocument *self = GW_DOCUMENT (object);
  GwDocumentPrivate *priv = gw_document_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      priv->language = g_value_dup_object (value);
      g_assert_nonnull (priv->language);
      break;

    case PROP_TEXT:
      if (g_value_get_string (value))
        priv->text = gw_string_new (g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_document_class_init (GwDocumentClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = gw_document_dispose;
  object_class->get_property = gw_document_get_property;
  object_class->set_property = gw_document_set_property;

  properties[PROP_LANGUAGE] = g_param_spec_object ("language",
                                                   "Language",
                                                   "Language of this document",
                                                   GW_TYPE_LANGUAGE,
                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);


  properties[PROP_TEXT] = g_param_spec_string ("text",
                                               "Text",
                                               "Text of this document",
                                               NULL,
                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gw_document_init (GwDocument *self)
{
}

/**
 * gw_document_get_language:
 * @self: a #GwDocument
 *
 * Retrieves the language of @self. Every #GwDocument is
 * guaranteed to always have a language associated.
 *
 * Returns: (transfer none): a #GwLanguage
 *
 * Since: 0.1
 */
GwLanguage*
gw_document_get_language (GwDocument *self)
{
  GwDocumentPrivate *priv;

  g_return_val_if_fail (GW_IS_DOCUMENT (self), NULL);

  priv = gw_document_get_instance_private (self);

  return priv->language;
}

/**
 * gw_document_get_text:
 * @self: a #GwDocument
 *
 * Retrieves the text of @self.
 *
 * Returns: (transfer none)(nullable): a #GwString
 *
 * Since: 0.1
 */
GwString*
gw_document_get_text (GwDocument *self)
{
  GwDocumentPrivate *priv;

  g_return_val_if_fail (GW_IS_DOCUMENT (self), NULL);

  priv = gw_document_get_instance_private (self);

  return priv->text;
}

/**
 * gw_document_modify:
 * @self: a #GwDocument
 * @modifier: a #GwModifier implementation instance
 * @cancellable: (nullable): a #GCancellable to cancel the operation
 * @callback: (scope async): a callback to call when the operation is finished
 * @user_data: (closure): user data for @callback
 *
 * Modifies @self using the passed @modifier. A new document is created
 * in the proccess.
 *
 * Since: 0.1.0
 */
void
gw_document_modify (GwDocument          *self,
                    GwModifier          *modifier,
                    GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data)
{
  g_return_if_fail (GW_IS_DOCUMENT (self));
  g_return_if_fail (GW_IS_MODIFIER (modifier));

  gw_task_helper_run (self,
                      g_object_ref (modifier),
                      g_object_unref,
                      modify_in_thread_cb,
                      cancellable,
                      callback,
                      user_data);

}

/**
 * gw_document_modify_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location of a #GError
 *
 * Finishes the operation started by gw_document_modify().
 *
 * Returns: %TRUE if the modification was successfull, %FALSE
 *          otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_document_modify_finish (GAsyncResult  *result,
                           GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * gw_document_modify_sync:
 * @self: a #GwDocument
 * @modifier: a #GwModifier implementation instance
 * @cancellable: (nullable): a #GCancellable to cancel the operation
 * @error: (nullable): return location of a #GError
 *
 * Modifies @self using the passed @modifier. A new document is created
 * in the proccess.
 *
 * Returns: %TRUE if the modification was successfull, %FALSE
 *          otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_document_modify_sync (GwDocument    *self,
                         GwModifier    *modifier,
                         GCancellable  *cancellable,
                         GError       **error)
{
  g_autoptr (GwStringEditor) editor = NULL;
  GwDocumentPrivate *priv;
  GwSegmenter *segmenter;
  GwString *text;
  gboolean was_word, is_word;
  gchar *aux, *start, *end;
  guint64 diff;
  gsize len;

  g_return_val_if_fail (GW_IS_DOCUMENT (self), FALSE);
  g_return_val_if_fail (GW_IS_MODIFIER (modifier), FALSE);

  priv = gw_document_get_instance_private (self);
  text = priv->text;
  segmenter = gw_language_get_segmenter (priv->language);
  is_word = was_word = FALSE;
  start = end = NULL;
  diff = 0;

  /* Don't attempt to segment NULL texts */
  if (!text)
    return TRUE;

  aux = text;
  len = strlen (text);

  editor = gw_string_editor_new (text);

  do
    {
      gunichar c;

      c = g_utf8_get_char (aux);

      was_word = is_word;
      is_word = gw_segmenter_is_word_character (segmenter, c, text - aux, text, len);

      if (!was_word && is_word)
        {
          start = aux;
        }
      else if (was_word && !is_word)
        {
          /* Is it really safe to just assume words won't be bigger than 256 chars? */
          gchar new_string[256] = { 0, };
          gchar substring[256];
          gsize new_size;
          gsize length;

          end = aux;
          length = end - start;

          memcpy (substring, start, length);
          substring[255] = '\0';

          /* Apply the modifier */
          if (gw_modifier_modify_word (modifier,
                                       substring,
                                       length,
                                       (GStrv) &new_string,
                                       &new_size))
            {
              /* Keep track of the general size difference */
              diff += new_size - length;

              gw_string_editor_modify (editor,
                                       text - start + diff,
                                       text - end + diff,
                                       new_string, new_size);
            }
        }

      aux = g_utf8_next_char (aux);
    }
  while (aux && *aux);

  /* Make the edited string the new one */
  g_clear_pointer (&priv->text, gw_string_unref);
  priv->text = gw_string_editor_to_string (editor);

  return TRUE;
}

/**
 * gw_document_realize:
 * @self: a #GwDocument
 * @cancellable: (nullable): a #GCancellable to cancel the operation
 * @callback: (scope async): a callback to call when the operation is finished
 * @user_data: (closure): user data for @callback
 *
 * Asynchronously realizes @self.
 *
 * Since: 0.1.0
 */
void
gw_document_realize (GwDocument          *self,
                     GCancellable        *cancellable,
                     GAsyncReadyCallback  callback,
                     gpointer             user_data)
{
  g_return_if_fail (GW_IS_DOCUMENT (self));

  gw_task_helper_run (self,
                      NULL,
                      NULL,
                      realize_in_thread_cb,
                      cancellable,
                      callback,
                      user_data);
}

/**
 * gw_document_realize_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location of a #GError
 *
 * Finishes the operation started by gw_document_realize().
 *
 * Returns: (transfer full): a #GwString with the document realization.
 *
 * Since: 0.1.0
 */
GwString*
gw_document_realize_finish (GAsyncResult  *result,
                            GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * gw_document_realize_sync:
 * @self: a #GwDocument
 * @modifier: a #GwModifier implementation instance
 * @cancellable: (nullable): a #GCancellable to cancel the operation
 * @error: (nullable): return location of a #GError
 *
 * Synchronously realizes @self.
 *
 * Returns: (transfer full): a #GwString with the document realization.
 *
 * Since: 0.1.0
 */
GwString*
gw_document_realize_sync (GwDocument    *self,
                          GCancellable  *cancellable,
                          GError       **error)
{
  g_return_val_if_fail (GW_IS_DOCUMENT (self), NULL);

  return GW_DOCUMENT_CLASS (G_OBJECT_CLASS (self))->realize (self, cancellable, error);
}
