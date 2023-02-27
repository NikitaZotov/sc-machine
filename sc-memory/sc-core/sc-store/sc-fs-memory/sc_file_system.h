/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_file_system_h_
#define _sc_file_system_h_

#include "../sc_types.h"

sc_bool sc_fs_isdir(const sc_char * path);

sc_bool sc_fs_isfile(const sc_char * path);

void sc_fs_rmdir(const sc_char * path);

sc_bool sc_fs_mkdirs(const sc_char * path);

sc_bool sc_fs_mkfile(sc_char const * path);

void * sc_fs_open_tmp_file(const sc_char * path, sc_char ** tmp_file_name, sc_char * prefix);

void sc_fs_initialize_file_path(sc_char const * path, sc_char const * postfix, sc_char ** out_path);

sc_char * sc_fs_exec(sc_char * command);

sc_bool sc_fs_is_binary_file(sc_char filePath[]);

void sc_fs_get_file_content(sc_char const * file_path, sc_char ** content, sc_uint32 * content_size);

#endif