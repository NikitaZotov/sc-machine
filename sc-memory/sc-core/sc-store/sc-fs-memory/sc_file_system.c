/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_file_system.h"

#include "sc_io.h"
#include "../sc-base/sc_message.h"

#include "glib.h"
#include "glib/gstdio.h"
#include "../sc-container/sc-string/sc_string.h"
#include "../sc_stream.h"
#include "../sc_stream_file.h"

#define SC_FS_FILE_COMMAND "file -b --mime-encoding "

sc_bool sc_fs_is_directory(const sc_char * path)
{
  return g_file_test(path, G_FILE_TEST_IS_DIR);
}

sc_bool sc_fs_is_file(const sc_char * path)
{
  return g_file_test(path, G_FILE_TEST_IS_REGULAR);
}

sc_bool sc_fs_remove_directory(const sc_char * path)
{
  if (sc_fs_is_directory(path) == SC_FALSE)
    return SC_FALSE;

  GDir * directory = g_dir_open(path, 0, null_ptr);

  // calculate files
  sc_char tmp_path[MAX_PATH_LENGTH];
  sc_char const * file = g_dir_read_name(directory);
  while (file != null_ptr)
  {
    g_snprintf(tmp_path, MAX_PATH_LENGTH, "%s/%s", path, file);

    if (sc_fs_is_file(tmp_path) == SC_TRUE)
    {
      if (g_remove(tmp_path) == -1)
        return SC_FALSE;
    }
    else if (sc_fs_is_directory(tmp_path) == SC_TRUE)
      sc_fs_remove_directory(tmp_path);

    file = g_dir_read_name(directory);
  }

  g_dir_close(directory);
  g_rmdir(path);

  return SC_TRUE;
}

sc_bool sc_fs_create_directory(const sc_char * path)
{
#if SC_PLATFORM_LINUX || SC_PLATFORM_MAC
  int const mode = 0777;
#else
  int const mode = 0;
#endif
  if (g_mkdir_with_parents(path, mode) == -1)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_fs_remove_file(sc_char const * path)
{
  if (!sc_fs_is_file(path) || g_remove(path) == -1)
    return SC_FALSE;

  return SC_TRUE;
}

sc_bool sc_fs_create_file(sc_char const * path)
{
  if (path == null_ptr)
    return SC_FALSE;

  sc_io_channel * channel = sc_io_new_channel(path, "w+", null_ptr);
  if (channel == null_ptr)
    return SC_FALSE;

  sc_io_channel_shutdown(channel, SC_TRUE, null_ptr);
  return SC_TRUE;
}

void * sc_fs_new_tmp_write_channel(const sc_char * path, sc_char ** tmp_file_name, sc_char * prefix)
{
  *tmp_file_name = g_strdup_printf("%s/%s_%lu", path, prefix, (sc_ulong)g_get_real_time());

  sc_io_channel * result = sc_io_new_write_channel(*tmp_file_name, null_ptr);
  return result;
}

void sc_fs_initialize_file_path(sc_char const * path, sc_char const * postfix, sc_char ** out_path)
{
  sc_uint32 size = sc_str_len(path) + sc_str_len(postfix) + 2;
  *out_path = sc_mem_new(sc_char, size + 1);
  sc_str_printf(*out_path, size, "%s/%s", path, postfix);
}

sc_char * sc_fs_execute(sc_char const * command)
{
  FILE * pipe = popen(command, "r");
  sc_int32 buffer_size = 50;
  sc_char buffer[buffer_size];
  while (fgets(buffer, buffer_size, pipe) != null_ptr)
    ;

  pclose(pipe);

  sc_char * char_result;
  sc_str_cpy(char_result, buffer, sc_str_len(buffer));
  return char_result;
}

sc_bool sc_fs_is_binary_file(sc_char const * file_path)
{
  sc_char command_prefix[] = SC_FS_FILE_COMMAND;
  sc_char * command;
  sc_str_cpy(command, command_prefix, sc_str_len(command_prefix) + sc_str_len(file_path));
  strcat(command, file_path);

  sc_char * result = sc_fs_execute(command);
  sc_mem_free(command);
  sc_bool const is_binary_file = sc_str_has_prefix(result, "binary");
  sc_mem_free(result);
  return is_binary_file;
}

void sc_fs_get_file_content(sc_char const * file_path, sc_char ** content, sc_uint32 * content_size)
{
  sc_stream * stream = sc_stream_file_new(file_path, SC_STREAM_FLAG_READ);
  if (stream == null_ptr)
  {
    *content = null_ptr;
    *content_size = 0;
    return;
  }

  sc_stream_get_data(stream, content, content_size);
  sc_stream_free(stream);
}
