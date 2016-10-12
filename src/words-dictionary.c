/* words-dictionary.c
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

#include "words-dictionary.h"
#include "words-radix-tree.h"

/**
 * SECTION:words-dictionary
 * @short_title:a dictionary of words
 * @title:WordsDictionary
 * @stability:Unstable
 *
 * The #WordsDictionary class is a wrapper around a #WordsRadixTree specialized in
 * dealing with words.
 */

typedef struct
{
  WordsRadixTree     *tree;
} WordsDictionaryPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (WordsDictionary, words_dictionary, G_TYPE_OBJECT)

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
  WordsDictionary *self;
  TaskData *data;
  GError *error;

  self = source_object;
  data = task_data;
  error = NULL;

  words_dictionary_load_from_gfile_sync (self,
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
tree_size_changed_cb (WordsDictionary *self)
{
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_SIZE]);
}

static void
words_dictionary_finalize (GObject *object)
{
  WordsDictionary *self = (WordsDictionary *)object;
  WordsDictionaryPrivate *priv = words_dictionary_get_instance_private (self);

  g_clear_object (&priv->tree);

  G_OBJECT_CLASS (words_dictionary_parent_class)->finalize (object);
}

static void
words_dictionary_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  WordsDictionary *self = WORDS_DICTIONARY (object);
  WordsDictionaryPrivate *priv = words_dictionary_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_RADIX_TREE:
      g_value_set_object (value, priv->tree);
      break;

    case PROP_SIZE:
      g_value_set_int (value, words_radix_tree_get_size (priv->tree));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
words_dictionary_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
words_dictionary_class_init (WordsDictionaryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = words_dictionary_finalize;
  object_class->get_property = words_dictionary_get_property;
  object_class->set_property = words_dictionary_set_property;

  /**
   * WordsDictionary:radix-tree:
   *
   * The internal #WordsRadixTree of this dictionary.
   *
   * Since: 0.1.0
   */
  properties[PROP_RADIX_TREE] = g_param_spec_object ("radix-tree",
                                                     "Radix tree",
                                                     "The internal radix tree of this dictionary",
                                                     WORDS_TYPE_RADIX_TREE,
                                                     G_PARAM_READABLE);

  /**
   * WordsDictionary:size:
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
words_dictionary_init (WordsDictionary *self)
{
  WordsDictionaryPrivate *priv = words_dictionary_get_instance_private (self);

  priv->tree = words_radix_tree_new ();

  g_signal_connect (priv->tree,
                    "notify::size",
                    G_CALLBACK (tree_size_changed_cb),
                    self);
}

/**
 * words_dictionary_new:
 *
 * Creates a new #WordsDictionary.
 *
 * Returns:(transfer full): a #WordsDictionary.
 *
 * Since: 0.1.0
 */
WordsDictionary*
words_dictionary_new (void)
{
  return g_object_new (WORDS_TYPE_DICTIONARY, NULL);
}

/**
 * words_dictionary_contains:
 * @self: a #WordsDictionary
 * @word: the key to search for
 *
 * Checks if @self contains the word @word.
 *
 * Returns: %TRUE if @self contains @word, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_contains (WordsDictionary *self,
                           const gchar     *word)
{
  WordsDictionaryPrivate *priv;

  g_return_val_if_fail (WORDS_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (word != NULL, FALSE);

  priv = words_dictionary_get_instance_private (self);

  return words_radix_tree_contains (priv->tree, word);
}

/**
 * words_dictionary_insert:
 * @self: a #WordsDictionary
 * @word: a word to insert in @self
 *
 * Inserts @word in @self. If @word is already present, nothing
 * happens.
 *
 * Since: 0.1.0
 */
void
words_dictionary_insert (WordsDictionary *self,
                         const gchar     *word)
{
  WordsDictionaryPrivate *priv;

  g_return_if_fail (WORDS_IS_DICTIONARY (self));
  g_return_if_fail (word != NULL);

  priv = words_dictionary_get_instance_private (self);

  words_radix_tree_insert (priv->tree, word, NULL);
}

/**
 * words_dictionary_remove:
 * @self: a #WordsDictionary
 * @word: a word to remove from @self
 *
 * Removes @word from @self. If @word is not present, nothing
 * happens.
 *
 * Since: 0.1.0
 */
void
words_dictionary_remove (WordsDictionary *self,
                         const gchar     *word)
{
  WordsDictionaryPrivate *priv;

  g_return_if_fail (WORDS_IS_DICTIONARY (self));
  g_return_if_fail (word != NULL);

  priv = words_dictionary_get_instance_private (self);

  words_radix_tree_remove (priv->tree, word);
}

/**
 * words_dictionary_load_from_file:
 * @self: a #WordsDictionary
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
 * with words_dictionary_load_from_gfile().
 *
 * Since: 0.1.0
 */
void
words_dictionary_load_from_file (WordsDictionary     *self,
                                 const gchar         *file_path,
                                 const gchar         *delimiter,
                                 GAsyncReadyCallback  callback,
                                 GCancellable        *cancellable,
                                 gpointer             user_data)
{
  GFile *file;

  g_return_if_fail (WORDS_IS_DICTIONARY (self));
  g_return_if_fail (file_path != NULL);

  file = g_file_new_for_path (file_path);

  words_dictionary_load_from_gfile (self,
                                    file,
                                    delimiter,
                                    callback,
                                    cancellable,
                                    user_data);

  g_clear_object (&file);
}

/**
 * words_dictionary_load_from_file_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finished the operation started by words_dictionary_load_from_file().
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_load_from_file_finish (GAsyncResult  *result,
                                        GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * words_dictionary_load_from_file_sync:
 * @self: a #WordsDictionary
 * @file_path: location of the file to load
 * @delimiter: (nullable): delimitir to split the file contents
 * @cancellable: (nullable): cancellable to cancel the operation
 * @error: (nullable): return location for a #GError
 *
 * Reads the contents of @file_path into @self.
 *
 * This method blocks the mainloop. See words_dictionary_load_from_file() for
 * the asynchronous variant.
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_load_from_file_sync (WordsDictionary  *self,
                                      const gchar      *file_path,
                                      const gchar      *delimiter,
                                      GCancellable     *cancellable,
                                      GError          **error)
{
  GFile *file;
  gboolean retval;

  g_return_val_if_fail (WORDS_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (file_path != NULL, FALSE);

  file = g_file_new_for_path (file_path);

  retval = words_dictionary_load_from_gfile_sync (self,
                                                  file,
                                                  delimiter,
                                                  cancellable,
                                                  error);

  g_clear_object (&file);

  return retval;
}

/**
 * words_dictionary_load_from_file:
 * @self: a #WordsDictionary
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
 * with words_dictionary_load_from_gfile().
 *
 * Since: 0.1.0
 */
void
words_dictionary_load_from_resource (WordsDictionary     *self,
                                     const gchar         *resource_path,
                                     const gchar         *delimiter,
                                     GAsyncReadyCallback  callback,
                                     GCancellable        *cancellable,
                                     gpointer             user_data)
{
  GFile *file;
  gchar *full_path;

  g_return_if_fail (WORDS_IS_DICTIONARY (self));
  g_return_if_fail (resource_path != NULL);

  if (g_str_has_prefix (resource_path, "resource://"))
    full_path = g_strdup (resource_path);
  else
    full_path = g_strdup_printf ("resource://%s", resource_path);

  file = g_file_new_for_uri (full_path);

  words_dictionary_load_from_gfile (self,
                                    file,
                                    delimiter,
                                    callback,
                                    cancellable,
                                    user_data);

  g_clear_pointer (&full_path, g_free);
  g_clear_object (&file);
}

/**
 * words_dictionary_load_from_file_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finished the operation started by words_dictionary_load_from_file().
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_load_from_resource_finish (GAsyncResult  *result,
                                            GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * words_dictionary_load_from_file_sync:
 * @self: a #WordsDictionary
 * @file_path: location of the file to load
 * @delimiter: (nullable): delimitir to split the file contents
 * @cancellable: (nullable): cancellable to cancel the operation
 * @error: (nullable): return location for a #GError
 *
 * Reads the contents of @file_path into @self.
 *
 * This method blocks the mainloop. See words_dictionary_load_from_file() for
 * the asynchronous variant.
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_load_from_resource_sync (WordsDictionary  *self,
                                          const gchar      *resource_path,
                                          const gchar      *delimiter,
                                          GCancellable     *cancellable,
                                          GError          **error)
{
  GFile *file;
  gboolean retval;
  gchar *full_path;

  g_return_val_if_fail (WORDS_IS_DICTIONARY (self), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);
  g_return_val_if_fail (resource_path != NULL, FALSE);

  if (g_str_has_prefix (resource_path, "resource://"))
    full_path = g_strdup (resource_path);
  else
    full_path = g_strdup_printf ("resource://%s", resource_path);

  file = g_file_new_for_uri (full_path);

  retval = words_dictionary_load_from_gfile_sync (self,
                                                  file,
                                                  delimiter,
                                                  cancellable,
                                                  error);
  g_clear_pointer (&full_path, g_free);
  g_clear_object (&file);

  return retval;
}

/**
 * words_dictionary_load_from_file:
 * @self: a #WordsDictionary
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
 * with words_dictionary_load_from_gfile().
 *
 * Since: 0.1.0
 */
void
words_dictionary_load_from_gfile (WordsDictionary     *self,
                                  GFile               *file,
                                  const gchar         *delimiter,
                                  GAsyncReadyCallback  callback,
                                  GCancellable        *cancellable,
                                  gpointer             user_data)
{
  TaskData *data;
  GTask *task;

  g_return_if_fail (WORDS_IS_DICTIONARY (self));
  g_return_if_fail (file != NULL);

  data = g_new0 (TaskData, 1);
  data->file = g_object_ref (file);
  data->delimiter = g_strdup (delimiter);

  task = g_task_new (self, cancellable, callback, user_data);

  g_task_set_task_data (task, data, task_data_free);
  g_task_run_in_thread (task, load_gfile_in_thread_cb);
}

/**
 * words_dictionary_load_from_gfile_finish:
 * @result: a #GAsyncResult
 * @error: (nullable): return location for a #GError
 *
 * Finished the operation started by words_dictionary_load_from_file().
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_load_from_gfile_finish (GAsyncResult  *result,
                                         GError       **error)
{
  g_return_val_if_fail (G_IS_TASK (result), FALSE);
  g_return_val_if_fail (!error || !*error, FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * words_dictionary_load_from_gfile_sync:
 * @self: a #WordsDictionary
 * @file: a #GFile
 * @delimiter: (nullable): delimitir to split the file contents
 * @cancellable: (nullable): cancellable to cancel the operation
 * @error: (nullable): return location for a #GError
 *
 * Reads the contents of @file into @self.
 *
 * This method blocks the mainloop. See words_dictionary_load_from_gfile() for
 * the asynchronous variant.
 *
 * Returns: %TRUE if the load was successfull, %FALSE otherwise.
 *
 * Since: 0.1.0
 */
gboolean
words_dictionary_load_from_gfile_sync (WordsDictionary  *self,
                                       GFile            *file,
                                       const gchar      *delimiter,
                                       GCancellable     *cancellable,
                                       GError          **error)
{
  gboolean success;
  GStrv split_content;
  gchar *contents;
  gint i;

  g_return_val_if_fail (WORDS_IS_DICTIONARY (self), FALSE);
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

      words_dictionary_insert (self, split_content[i]);
    }

  g_clear_pointer (&split_content, g_strfreev);
  g_clear_pointer (&contents, g_free);

  return success;
}
