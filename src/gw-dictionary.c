/* gw-dictionary.c
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

#include "gw-dictionary.h"
#include "gw-radix-tree.h"

/**
 * SECTION:gw-dictionary
 * @short_title:a dictionary of words
 * @title:GwDictionary
 * @stability:Unstable
 *
 * The #GwDictionary class is a wrapper around a #GwRadixTree specialized in
 * dealing with words.
 */

typedef struct
{
  GwRadixTree        *tree;
} GwDictionaryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GwDictionary, gw_dictionary, G_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_RADIX_TREE,
  PROP_SIZE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS] = { NULL, };

typedef struct
{
  gchar *delimiter;
  GFile *file;
} TaskData;

static void
task_data_free (gpointer data)
{
  TaskData *task_data = data;

  g_clear_pointer (&task_data->delimiter, g_free);
  g_clear_object (&task_data->file);

  g_free (task_data);
}

/* Load files/resources in thread */
static void
load_gfile_in_thread_cb (GTask        *task,
                         gpointer      source_object,
                         gpointer      task_data,
                         GCancellable *cancellable)
{
  GwDictionary *self;
  TaskData *data;
  GError *error;

  self = source_object;
  data = task_data;
  error = NULL;

  gw_dictionary_load_from_gfile_sync (self,
                                         data->file,
                                         data->delimiter,
                                         cancellable,
                                         &error);

  if (error)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_boolean (task, TRUE);
}

static void
gw_dictionary_finalize (GObject *object)
{
  GwDictionary *self = (GwDictionary *)object;
  GwDictionaryPrivate *priv = gw_dictionary_get_instance_private (self);

  g_clear_pointer (&priv->tree, gw_radix_tree_unref);

  G_OBJECT_CLASS (gw_dictionary_parent_class)->finalize (object);
}

static void
gw_dictionary_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GwDictionary *self = GW_DICTIONARY (object);
  GwDictionaryPrivate *priv = gw_dictionary_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_RADIX_TREE:
      g_value_set_object (value, priv->tree);
      break;

    case PROP_SIZE:
      g_value_set_int (value, gw_radix_tree_get_size (priv->tree));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gw_dictionary_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gw_dictionary_class_init (GwDictionaryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gw_dictionary_finalize;
  object_class->get_property = gw_dictionary_get_property;
  object_class->set_property = gw_dictionary_set_property;

  /**
   * GwDictionary:radix-tree:
   *
   * The internal #GwRadixTree of this dictionary.
   *
   * Since: 0.1.0
   */
  properties[PROP_RADIX_TREE] = g_param_spec_boxed ("radix-tree",
                                                    "Radix tree",
                                                    "The internal radix tree of this dictionary",
                                                    GW_TYPE_RADIX_TREE,
                                                    G_PARAM_READABLE);

  /**
   * GwDictionary:size:
   *
   * The number of elements this dictionary contains.
   *
   * Since: 0.1.0
   */
  properties[PROP_SIZE] = g_param_spec_int ("size",
                                            "Size of the tree",
                                            "The number of elements in this tree",
                                            0,
                                            G_MAXINT,
                                            0,
                                            G_PARAM_READABLE);

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
gw_dictionary_init (GwDictionary *self)
{
  GwDictionaryPrivate *priv = gw_dictionary_get_instance_private (self);

  priv->tree = gw_radix_tree_new ();
}

/**
 * gw_dictionary_new:
 *
 * Creates a new #GwDictionary.
 *
 * Returns:(transfer full): a #GwDictionary.
 *
 * Since: 0.1.0
 */
GwDictionary*
gw_dictionary_new (void)
{
  return g_object_new (GW_TYPE_DICTIONARY, NULL);
}

/**
 * gw_dictionary_contains:
 * @self: a #GwDictionary
 * @word: the key to search for
 *
 * Checks if @self contains the word @word.
 *
 * Returns: %TRUE if @self contains @word, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_contains (GwDictionary *self,
                        const gchar     *word,
                        gsize            word_length)
{
  GwDictionaryPrivate *priv;

  g_return_val_if_fail (GW_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (word != NULL, FALSE);

  priv = gw_dictionary_get_instance_private (self);

  return gw_radix_tree_contains (priv->tree, word, word_length);
}

/**
 * gw_dictionary_insert:
 * @self: a #GwDictionary
 * @word: a word to insert in @self
 * @word_length: the length of @key, or -1
 *
 * Inserts @word in @self. If @word is already present, nothing
 * happens.
 *
 * Since: 0.1.0
 */
void
gw_dictionary_insert (GwDictionary *self,
                      const gchar     *word,
                      gsize            word_length)
{
  GwDictionaryPrivate *priv;

  g_return_if_fail (GW_IS_DICTIONARY (self));
  g_return_if_fail (word != NULL);

  priv = gw_dictionary_get_instance_private (self);

  gw_radix_tree_insert (priv->tree, word, word_length, NULL);
}

/**
 * gw_dictionary_remove:
 * @self: a #GwDictionary
 * @word: a word to remove from @self
 *
 * Removes @word from @self. If @word is not present, nothing
 * happens.
 *
 * Since: 0.1.0
 */
void
gw_dictionary_remove (GwDictionary *self,
                      const gchar     *word,
                      gsize            word_length)
{
  GwDictionaryPrivate *priv;

  g_return_if_fail (GW_IS_DICTIONARY (self));
  g_return_if_fail (word != NULL);

  priv = gw_dictionary_get_instance_private (self);

  gw_radix_tree_remove (priv->tree, word, word_length);
}

/**
 * gw_dictionary_load_from_file:
 * @self: a #GwDictionary
 * @file_path: the path to the file
 * @delimiter: (nullable): the delimiter to split the file contents
 * @callback: (): callback to call when the operation is done
 * @cancellable: (nullable): cancellable to cancel the operation
 * @user_data:
 *
 * Loads the content of @file_path to @self. If @delimiter is %NULL,
 * the newline is used instead.
 *
 * This method has the same effect of creating a #GFile and loading
 * with gw_dictionary_load_from_gfile().
 *
 * Since: 0.1.0
 */
void
gw_dictionary_load_from_file (GwDictionary     *self,
                              const gchar         *file_path,
                              const gchar         *delimiter,
                              GAsyncReadyCallback  callback,
                              GCancellable        *cancellable,
                              gpointer             user_data)
{
  GFile *file;

  g_return_if_fail (GW_IS_DICTIONARY (self));
  g_return_if_fail (file_path != NULL);

  file = g_file_new_for_path (file_path);

  gw_dictionary_load_from_gfile (self,
                                    file,
                                    delimiter,
                                    callback,
                                    cancellable,
                                    user_data);

  g_clear_object (&file);
}

/**
 * gw_dictionary_load_from_file_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finished the operation started by gw_dictionary_load_from_file().
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_load_from_file_finish (GAsyncResult  *result,
                                     GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * gw_dictionary_load_from_file_sync:
 * @self: a #GwDictionary
 * @file_path: location of the file to load
 * @delimiter: (nullable): delimitir to split the file contents
 * @cancellable: (nullable): cancellable to cancel the operation
 * @error: (nullable): return location for a #GError
 *
 * Reads the contents of @file_path into @self.
 *
 * This method blocks the mainloop. See gw_dictionary_load_from_file() for
 * the asynchronous variant.
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_load_from_file_sync (GwDictionary  *self,
                                   const gchar      *file_path,
                                   const gchar      *delimiter,
                                   GCancellable     *cancellable,
                                   GError          **error)
{
  GFile *file;
  gboolean retval;

  g_return_val_if_fail (GW_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (file_path != NULL, FALSE);

  file = g_file_new_for_path (file_path);

  retval = gw_dictionary_load_from_gfile_sync (self,
                                                  file,
                                                  delimiter,
                                                  cancellable,
                                                  error);

  g_clear_object (&file);

  return retval;
}

/**
 * gw_dictionary_load_from_file:
 * @self: a #GwDictionary
 * @file_path: the path to the file
 * @delimiter: (nullable): the delimiter to split the file contents
 * @callback: (): callback to call when the operation is done
 * @cancellable: (nullable): cancellable to cancel the operation
 * @user_data:
 *
 * Loads the content of @file_path to @self. If @delimiter is %NULL,
 * the newline is used instead.
 *
 * This method has the same effect of creating a #GFile and loading
 * with gw_dictionary_load_from_gfile().
 *
 * Since: 0.1.0
 */
void
gw_dictionary_load_from_resource (GwDictionary     *self,
                                  const gchar         *resource_path,
                                  const gchar         *delimiter,
                                  GAsyncReadyCallback  callback,
                                  GCancellable        *cancellable,
                                  gpointer             user_data)
{
  GFile *file;
  gchar *full_path;

  g_return_if_fail (GW_IS_DICTIONARY (self));
  g_return_if_fail (resource_path != NULL);

  if (g_str_has_prefix (resource_path, "resource://"))
    full_path = g_strdup (resource_path);
  else
    full_path = g_strdup_printf ("resource://%s", resource_path);

  file = g_file_new_for_uri (full_path);

  gw_dictionary_load_from_gfile (self,
                                    file,
                                    delimiter,
                                    callback,
                                    cancellable,
                                    user_data);

  g_clear_pointer (&full_path, g_free);
  g_clear_object (&file);
}

/**
 * gw_dictionary_load_from_file_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finished the operation started by gw_dictionary_load_from_file().
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_load_from_resource_finish (GAsyncResult  *result,
                                         GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * gw_dictionary_load_from_file_sync:
 * @self: a #GwDictionary
 * @file_path: location of the file to load
 * @delimiter: (nullable): delimitir to split the file contents
 * @cancellable: (nullable): cancellable to cancel the operation
 * @error: (nullable): return location for a #GError
 *
 * Reads the contents of @file_path into @self.
 *
 * This method blocks the mainloop. See gw_dictionary_load_from_file() for
 * the asynchronous variant.
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_load_from_resource_sync (GwDictionary  *self,
                                       const gchar      *resource_path,
                                       const gchar      *delimiter,
                                       GCancellable     *cancellable,
                                       GError          **error)
{
  GFile *file;
  gboolean retval;
  gchar *full_path;

  g_return_val_if_fail (GW_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (resource_path != NULL, FALSE);

  if (g_str_has_prefix (resource_path, "resource://"))
    full_path = g_strdup (resource_path);
  else
    full_path = g_strdup_printf ("resource://%s", resource_path);

  file = g_file_new_for_uri (full_path);

  retval = gw_dictionary_load_from_gfile_sync (self,
                                                  file,
                                                  delimiter,
                                                  cancellable,
                                                  error);
  g_clear_pointer (&full_path, g_free);
  g_clear_object (&file);

  return retval;
}

/**
 * gw_dictionary_load_from_file:
 * @self: a #GwDictionary
 * @file: a #GFile
 * @delimiter: (nullable): the delimiter to split the file contents
 * @callback: (): callback to call when the operation is done
 * @cancellable: (nullable): cancellable to cancel the operation
 * @user_data:
 *
 * Loads the content of @file_path to @self. If @delimiter is %NULL,
 * the newline is used instead.
 *
 * This method has the same effect of creating a #GFile and loading
 * with gw_dictionary_load_from_gfile().
 *
 * Since: 0.1.0
 */
void
gw_dictionary_load_from_gfile (GwDictionary     *self,
                               GFile               *file,
                               const gchar         *delimiter,
                               GAsyncReadyCallback  callback,
                               GCancellable        *cancellable,
                               gpointer             user_data)
{
  TaskData *data;
  GTask *task;

  g_return_if_fail (GW_IS_DICTIONARY (self));
  g_return_if_fail (file != NULL);

  data = g_new0 (TaskData, 1);
  data->file = g_object_ref (file);
  data->delimiter = g_strdup (delimiter);

  task = g_task_new (self, cancellable, callback, user_data);

  g_task_set_task_data (task, data, task_data_free);
  g_task_run_in_thread (task, load_gfile_in_thread_cb);
}

/**
 * gw_dictionary_load_from_gfile_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finished the operation started by gw_dictionary_load_from_file().
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_load_from_gfile_finish (GAsyncResult  *result,
                                      GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * gw_dictionary_load_from_gfile_sync:
 * @self: a #GwDictionary
 * @file: a #GFile
 * @delimiter: (nullable): delimitir to split the file contents
 * @cancellable: (nullable): cancellable to cancel the operation
 * @error: (nullable): return location for a #GError
 *
 * Reads the contents of @file into @self.
 *
 * This method blocks the mainloop. See gw_dictionary_load_from_gfile() for
 * the asynchronous variant.
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
gw_dictionary_load_from_gfile_sync (GwDictionary  *self,
                                    GFile            *file,
                                    const gchar      *delimiter,
                                    GCancellable     *cancellable,
                                    GError          **error)
{
  gboolean success;
  GStrv split_content;
  gchar *contents;
  gint i;

  g_return_val_if_fail (GW_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (file != NULL, FALSE);

  /* If nothing is passed, make the default delimiter a newline */
  if (!delimiter || delimiter[0] == '\0')
    delimiter = "\n";

  contents = NULL;
  success = g_file_load_contents (file,
                                  cancellable,
                                  &contents,
                                  NULL,
                                  NULL,
                                  error);

  if (!success)
    return FALSE;

  split_content = g_strsplit (contents, delimiter, -1);

  for (i = 0; split_content[i]; i++)
    {
      if (g_cancellable_set_error_if_cancelled (cancellable, error))
        break;

      gw_dictionary_insert (self, split_content[i], -1);
    }

  g_clear_pointer (&split_content, g_strfreev);
  g_clear_pointer (&contents, g_free);

  return success;
}
