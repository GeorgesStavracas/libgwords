/* gw-task-helper.c
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

#include "gw-task-helper-private.h"

void
gw_task_helper_run (gpointer             object,
                    gpointer             task_data,
                    GDestroyNotify       task_data_free,
                    GTaskThreadFunc      thread_func,
                    GCancellable        *cancellable,
                    GAsyncReadyCallback  callback,
                    gpointer             user_data)
{

  GTask *task;

  task = g_task_new (object, cancellable, callback, user_data);

  if (task_data)
    g_task_set_task_data (task, task_data, task_data_free);

  g_task_run_in_thread (task, thread_func);
}
