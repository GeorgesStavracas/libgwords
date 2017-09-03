/* gw-language.c
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

#include "gw-dictionary.h"
#include "gw-extension-points.h"
#include "gw-language.h"
#include "gw-radix-tree.h"
#include "gw-string.h"
#include "gw-segmenter.h"
#include "gw-task-helper-private.h"
#include "gw-utils.h"

struct _GwLanguage
{
  GObject             parent;

  gchar              *code;

  GwSegmenter        *segmenter;
  GwDictionary       *dictionary;

  GType               document_gtype;

  GError             *init_error;
  gboolean            valid : 1;
};

static void          gw_language_initable_iface_init          (GInitableIface     *iface);

G_DEFINE_TYPE_WITH_CODE (GwLanguage, gw_language, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, gw_language_initable_iface_init)
                         G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE, NULL))

enum
{
  PROP_0,
  PROP_CODE,
  PROP_DICTIONARY,
  PROP_SEGMENTER,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };


/*
 * GTask thread functions
 */
static void
create_document_in_thread_cb (GTask        *task,
                              gpointer      source_object,
                              gpointer      task_data,
                              GCancellable *cancellable)
{
  GwLanguage *self;
  GwDocument *document;
  GwString *text;
  GError *error;

  self = source_object;
  text = task_data;
  error = NULL;

  document = gw_language_create_document_sync (self, text, cancellable, &error);

  if (error)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_pointer (task, g_object_ref (document), g_object_unref);
}

/*
 * Auxiliary methods
 */

static gboolean
load_dictionary_file_at_path (GwLanguage  *self,
                              const gchar *path)
{
  GError *error;
  g_autofree gchar *dict_file;
  g_autofree gchar *filepath;

  /* TODO: if fail to find lang_REGION.gwdict, try only lang.gwdict */
  error = NULL;
  dict_file = g_strdup_printf ("%s.gwdict", self->code);
  filepath = g_build_filename (path, dict_file, NULL);

  if (!g_file_test (filepath, G_FILE_TEST_EXISTS) || !g_file_test (filepath, G_FILE_TEST_IS_REGULAR))
    return FALSE;

  /* Setup the dictionary */
  g_debug ("Loading dictionary file: %s", filepath);

  self->dictionary = gw_dictionary_new ();

  if (!gw_dictionary_load_from_file_sync (self->dictionary,
                                          filepath,
                                          NULL,
                                          NULL,
                                          &error))
    {
      g_warning ("Error loading dictionary: %s", error->message);
      g_clear_error (&error);
      return FALSE;
    }

  return TRUE;
}

static void
setup_dictionary (GwLanguage *self)
{
  const gchar * const * system_data_dirs;
  g_autofree gchar *user_data_dir;

  /* Try 1: g_get_user_data_dir */
  user_data_dir = g_build_filename (g_get_user_data_dir (), "gwords", NULL);

  if (load_dictionary_file_at_path (self, user_data_dir))
    goto out;

  /* Try 2: g_get_system_data_dirs */
  system_data_dirs = g_get_system_data_dirs ();

  for (; system_data_dirs && *system_data_dirs; system_data_dirs++)
    {
      g_autofree gchar *real_datadir;
      const gchar *datadir;

      datadir = *system_data_dirs;
      real_datadir = g_build_filename (datadir, "gwords", NULL);

      if (load_dictionary_file_at_path (self, real_datadir))
        goto out;
    }

out:
  if (self->dictionary)
    g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_DICTIONARY]);
}

static void
setup_segmenter (GwLanguage *self)
{
  GIOExtensionPoint *extension_point;
  GIOExtension *segmenter_extension;

  extension_point = g_io_extension_point_lookup (GW_EXTENSION_POINT_SEGMENTER);
  segmenter_extension = g_io_extension_point_get_extension_by_name (extension_point, self->code);

  if (!segmenter_extension)
    segmenter_extension = g_io_extension_point_get_extension_by_name (extension_point, "fallback");

  self->segmenter = g_object_new (g_io_extension_get_type (segmenter_extension),
                                  "language", self,
                                   NULL);

  g_debug ("Loading segmenter: %s", g_type_name (g_io_extension_get_type (segmenter_extension)));

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SEGMENTER]);
}

static void
setup_document (GwLanguage *self)
{
  GIOExtensionPoint *extension_point;
  GIOExtension *document_extension;

  extension_point = g_io_extension_point_lookup (GW_EXTENSION_POINT_DOCUMENT);
  document_extension = g_io_extension_point_get_extension_by_name (extension_point, self->code);

  if (!document_extension)
    document_extension = g_io_extension_point_get_extension_by_name (extension_point, "fallback");

  self->document_gtype = g_io_extension_get_type (document_extension);

  g_debug ("Document type: %s", g_type_name (g_io_extension_get_type (document_extension)));
}

static void
gw_language_set_language_code_internal (GwLanguage  *self,
                                        const gchar *code)
{
  /* Default to the system language if nothing was provided */
  if (!code)
    {
      g_autofree gchar *region = NULL;
      g_autofree gchar *lang = NULL;

      if (!gw_get_system_language (&lang, &region))
        {
          self->init_error = g_error_new (GW_LANGUAGE_ERROR,
                                          GW_LANGUAGE_ERROR_INVALID,
                                          "Couldn't fetch system language");
          return;
        }

      self->code = g_strdup_printf ("%s_%s", lang, region);
    }
  else
    {
      self->code = g_strdup (code);
    }

  self->valid = TRUE;

  /* Create the various objects */
  setup_dictionary (self);
  setup_document (self);
  setup_segmenter (self);
}

/*
 * GInitable iface implementation
 */

G_LOCK_DEFINE_STATIC (init_lock);

static gboolean
gw_language_initable_init (GInitable     *initable,
                           GCancellable  *cancellable,
                           GError       **error)
{
  GwLanguage *self = GW_LANGUAGE (initable);

  G_LOCK (init_lock);

  if (!self->valid)
    g_propagate_error (error, g_error_copy (self->init_error));

  G_UNLOCK (init_lock);

  return self->valid;
}

static void
gw_language_initable_iface_init (GInitableIface *iface)
{
  iface->init = gw_language_initable_init;
}


/*
 * GObject overrides
 */

static void
gw_language_finalize (GObject *object)
{
  GwLanguage *self = (GwLanguage *)object;

  g_clear_pointer (&self->code, g_free);
  g_clear_object (&self->segmenter);

  G_OBJECT_CLASS (gw_language_parent_class)->finalize (object);
}

static void
gw_language_constructed (GObject *object)
{
  GwLanguage *self = GW_LANGUAGE (object);

  (void)self;

  G_OBJECT_CLASS (gw_language_parent_class)->constructed (object);
}

static void
gw_language_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GwLanguage *self = GW_LANGUAGE (object);

  switch (prop_id)
    {
    case PROP_CODE:
      g_value_set_string (value, self->code);
      break;

    case PROP_DICTIONARY:
      g_value_set_object (value, self->dictionary);
      break;

    case PROP_SEGMENTER:
      g_value_set_object (value, self->segmenter);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_language_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GwLanguage *self = GW_LANGUAGE (object);

  switch (prop_id)
    {
    case PROP_CODE:
      gw_language_set_language_code_internal (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_language_class_init (GwLanguageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gw_language_finalize;
  object_class->constructed = gw_language_constructed;
  object_class->get_property = gw_language_get_property;
  object_class->set_property = gw_language_set_property;

  properties[PROP_CODE] = g_param_spec_string ("code",
                                               "Code of the language",
                                               "Code of the language",
                                               NULL,
                                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  properties[PROP_SEGMENTER] = g_param_spec_object ("segmenter",
                                                    "Word segmenter",
                                                    "Word segmenter of the language",
                                                    GW_TYPE_SEGMENTER,
                                                    G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  properties[PROP_DICTIONARY] = g_param_spec_object ("dictionary",
                                                     "Word dictionary",
                                                     "Word dictionary of the language",
                                                     GW_TYPE_DICTIONARY,
                                                     G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gw_language_init (GwLanguage *self)
{
}

/**
 * gcal_event_error_quark:
 *
 * Error quark for event creating.
 */
GQuark
gw_language_error_quark (void)
{
  return g_quark_from_static_string ("Invalid language");
}

/**
 * gw_language_create_document:
 * @self: a #GwDocument
 * @text: (nullable): a #GwText
 * @cancellable: (nullable): a #GCancellable
 * @callback: (scope async): callback called when the operation is finished
 * @user_data: (closure): user data for @callback
 *
 * Asynchronously creates a #GwDocument using @self as the
 * base language.
 *
 * Since: 0.1
 */
void
gw_language_create_document (GwLanguage          *self,
                             GwString            *text,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  g_return_if_fail (GW_IS_LANGUAGE (self));

  gw_task_helper_run (self,
                      text ? gw_string_ref (text) : NULL,
                      (GDestroyNotify) (text ? gw_string_unref : NULL),
                      create_document_in_thread_cb,
                      cancellable,
                      callback,
                      user_data);
}

/**
 * gw_language_create_document_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes the operation started by gw_language_create_document().
 *
 * Returns: a concrete #GwDocument implementation.
 *
 * Since: 0.1.0
 */
GwDocument*
gw_language_create_document_finish (GAsyncResult  *result,
                                    GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * gw_language_create_document_sync:
 * @self: a #GwDocument
 * @text: (nullable): a #GwText
 * @cancellable: (nullable): a #GCancellable
 * @error: (nullable): return location for a #GError
 *
 * Synchronously creates a #GwDocument using @self as the
 * base language.
 *
 * Returns: a concrete #GwDocument implementation.
 *
 * Since: 0.1
 */
GwDocument*
gw_language_create_document_sync (GwLanguage    *self,
                                  GwString      *text,
                                  GCancellable  *cancellable,
                                  GError       **error)
{
  g_return_val_if_fail (GW_IS_LANGUAGE (self), NULL);

  return g_object_new (self->document_gtype,
                       "language", self,
                       "text", text,
                       NULL);
}

/**
 * gw_language_new:
 * @language: (nullable): the language code
 * @cancellable: (nullable): optional #GCancellable object
 * @callback: (scope async): a #GAsyncReadyCallback to call when the initialization is done
 * @user_data: (closure): data to pass to the callback function
 *
 * Asynchronously creates a new #GwLanguage. When it finishes, @callback will
 * be called.
 *
 * Since: 0.1.0
 */
void
gw_language_new (const gchar         *language_code,
                 GCancellable        *cancellable,
                 GAsyncReadyCallback  callback,
                 gpointer             user_data)
{
  g_async_initable_new_async (GW_TYPE_LANGUAGE,
                              G_PRIORITY_DEFAULT,
                              cancellable,
                              callback,
                              user_data,
                              "code", language_code,
                              NULL);
}

/**
 * gw_language_new_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finishes an asynchronous initialization started in gw_language_new(). If
 * an error occurs, the functions sets @error and returns %NULL.
 *
 * Returns: (transfer full) (nullable): a #GwLanguage
 *
 * Since: 0.1.0
 */
GwLanguage*
gw_language_new_finish (GAsyncResult  *result,
                        GError       **error)
{
  GObject *language, *source;

  source = g_async_result_get_source_object (result);
  language = g_async_initable_new_finish (G_ASYNC_INITABLE (source), result, error);

  g_clear_object (&source);

  return language ? GW_LANGUAGE (language) : NULL;
}


/**
 * gw_language_new_sync:
 * @language: (nullable): the language string.
 * @cancellable: (nullable): a #GCancellable to cancel the operation
 * @error: (nullable): return value for a #GError
 *
 * Creates the given language and automatically scans the system for
 * all the available data about the language.
 *
 * @language accepts the same format of LC_LANG.
 *
 * Returns: (transfer full)(nullable): a #GwLanguage
 */
GwLanguage*
gw_language_new_sync (const gchar   *language_code,
                      GCancellable  *cancellable,
                      GError       **error)
{
  return g_initable_new (GW_TYPE_LANGUAGE,
                         cancellable,
                         error,
                         "code", language_code,
                         NULL);
}

/**
 * gw_language_get_language_code:
 * @self: a #GwLanguage
 *
 * Retrieves the language code of @self.
 *
 * Returns: (transfer none): the language code
 *
 * Since: 0.1.0
 */
const gchar*
gw_language_get_language_code (GwLanguage *self)
{
  g_return_val_if_fail (GW_IS_LANGUAGE (self), NULL);

  return self->code;
}

/**
 * gw_language_get_segmenter:
 * @self: a #GwLanguage
 *
 * Retrieves the segmenter registered for this language, or
 * the fallback segmenter if @language has no segmenter registered.
 *
 * Returns: (transfer none): a #GwSegmenter
 *
 * Since: 0.1.0
 */
GwSegmenter*
gw_language_get_segmenter (GwLanguage *self)
{
  g_return_val_if_fail (GW_IS_LANGUAGE (self), NULL);

  return self->segmenter;
}

/**
 * gw_language_get_dictionary:
 * @self: a #GwLanguage
 *
 * Retrieves the dictionary registered for this language, or
 * %NULL if @language has no dictionary file.
 *
 * Returns: (transfer none): a #GwDictionary
 *
 * Since: 0.1.0
 */
GwDictionary*
gw_language_get_dictionary (GwLanguage *self)
{
  g_return_val_if_fail (GW_IS_LANGUAGE (self), NULL);

  return self->dictionary;
}
