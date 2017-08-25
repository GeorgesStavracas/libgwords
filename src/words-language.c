/* words-language.c
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


#include "words-language.h"
#include "words-radix-tree.h"

struct _WordsLanguage
{
  GObject             parent;

  gchar              *code;

  GError             *init_error;
  gboolean            valid : 1;
};

static void          words_language_initable_iface_init          (GInitableIface     *iface);

G_DEFINE_TYPE_WITH_CODE (WordsLanguage, words_language, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, words_language_initable_iface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE, NULL))

enum
{
  PROP_0,
  PROP_CODE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };


/*
 * GInitable iface implementation
 */

G_LOCK_DEFINE_STATIC (init_lock);

static gboolean
words_language_initable_init (GInitable     *initable,
                              GCancellable  *cancellable,
                              GError       **error)
{
  WordsLanguage *self = WORDS_LANGUAGE (initable);

  G_LOCK (init_lock);

  if (!self->valid)
    g_propagate_error (error, g_error_copy (self->init_error));

  G_UNLOCK (init_lock);

  return self->valid;
}

static void
words_language_initable_iface_init (GInitableIface *iface)
{
  iface->init = words_language_initable_init;
}


/*
 * GObject overrides
 */

static void
words_language_finalize (GObject *object)
{
  WordsLanguage *self = (WordsLanguage *)object;

  g_clear_pointer (&self->code, g_free);

  G_OBJECT_CLASS (words_language_parent_class)->finalize (object);
}

static void
words_language_constructed (GObject *object)
{
  WordsLanguage *self = WORDS_LANGUAGE (object);

  (void)self;

  G_OBJECT_CLASS (words_language_parent_class)->constructed (object);
}

static void
words_language_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  WordsLanguage *self = WORDS_LANGUAGE (object);

  switch (prop_id)
    {
    case PROP_CODE:
      g_value_set_string (value, self->code);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
words_language_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  WordsLanguage *self = WORDS_LANGUAGE (object);

  switch (prop_id)
    {
    case PROP_CODE:
      self->code = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
words_language_class_init (WordsLanguageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = words_language_finalize;
  object_class->constructed = words_language_constructed;
  object_class->get_property = words_language_get_property;
  object_class->set_property = words_language_set_property;

  properties[PROP_CODE] = g_param_spec_string ("code",
                                               "Code of the language",
                                               "Code of the language",
                                               NULL,
                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
}

static void
words_language_init (WordsLanguage *self)
{
}

/**
 * gcal_event_error_quark:
 *
 * Error quark for event creating.
 */
GQuark
words_language_error_quark (void)
{
  return g_quark_from_static_string ("Invalid language");
}

/**
 * words_language_new:
 * @language: the language code
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * Asynchronously creates a new #WordsLanguage. When it finishes, @callback will
 * be called.
 *
 * Since: 0.1.0
 */
void
words_language_new (const gchar         *language_code,
                    GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data)
{
  g_return_if_fail (language_code && *language_code);

  g_async_initable_new_async (WORDS_TYPE_LANGUAGE,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "code", language_code,
                              NULL);
}

/**
 * words_language_new_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes an asynchronous initialization started in words_language_new(). If
 * an error occurs, the functions sets @error and returns %NULL.
 *
 * Returns: (transfer full) (nullable): a #WordsLanguage
 *
 * Since: 0.1.0
 */
WordsLanguage*
words_language_new_finish (GAsyncResult  *result,
                           GError       **error)
{
  GObject *language, *source;

  source = g_async_result_get_source_object (result);
  language = g_async_initable_new_finish (G_ASYNC_INITABLE (source), result, error);

  g_clear_object (&source);

  return language ? WORDS_LANGUAGE (language) : NULL;
}


/**
 * words_language_new_sync:
 * @language: the language string.
 * @cancellable: (nullable): a #GCancellable to cancel the operation
 * @error: (nullable): return value for a #GError
 *
 * Creates the given language and automatically scans the system for
 * all the available data about the language.
 *
 * @language accepts the same format of LC_LANG.
 *
 * Returns: (transfer full)(nullable): a #WordsLanguage
 */
WordsLanguage*
words_language_new_sync (const gchar   *language_code,
                         GCancellable  *cancellable,
                         GError       **error)
{
  return g_initable_new (WORDS_TYPE_LANGUAGE,
                         cancellable,
                         error,
                         "code", language_code,
                         NULL);
}
