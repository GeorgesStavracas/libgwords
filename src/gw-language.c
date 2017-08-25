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


#include "gw-language.h"
#include "gw-radix-tree.h"
#include "gw-utils.h"

struct _GwLanguage
{
  GObject             parent;

  gchar              *code;

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
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };


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
      self->code = g_value_dup_string (value);

      /* Default to the system language if nothing was provided */
      if (!self->code)
        {
          g_autofree gchar *region = NULL;
          g_autofree gchar *lang = NULL;

          if (!gw_get_system_language (&lang, &region))
            {
              self->init_error = g_error_new (GW_LANGUAGE_ERROR,
                                              GW_LANGUAGE_ERROR_INVALID,
                                              "Couldn't fetch system language");
              break;
            }

          self->code = g_strdup_printf ("%s_%s", lang, region);
        }

      self->valid = TRUE;
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
  g_return_if_fail (language_code && *language_code);

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
