/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_rocksdb_fs_storage_h_
#define _sc_rocksdb_fs_storage_h_

#include "../sc_stream.h"
#include "../sc_types.h"

/*! Initialize sc-rocksdb fs-storage in specified path
 * @param path Path to store on sc-rocksdb fs-storage.
 */
sc_bool sc_rocksdb_fs_storage_initialize(const sc_char * repo_path);

/// Shutdowns sc-rocksdb fs-storage
sc_bool sc_rocksdb_fs_storage_shutdown();

/// Clears strings in sc-rocksdb fs-storage
sc_bool sc_rocksdb_fs_storage_clear();

/// Saves strings in sc-rocksdb fs-storage
sc_bool sc_rocksdb_fs_storage_save();

/*! Appends sc-link to sc-rocksdb by its string content.
 * @param element An appendable sc-link
 * @param addr An appendable sc-link addr
 * @param sc_string A key string
 * @param size A key string size
 * @returns SC_TRUE, if link was added
 */
sc_bool sc_rocksdb_fs_storage_append_sc_link(sc_element * element, sc_addr addr, sc_char * sc_string, sc_uint32 size);

/*! Gets sc-links from sc-rocksdb by it string content.
 * @param sc_string A key string
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_rocksdb_fs_storage_get_sc_links(const sc_char * sc_string, sc_addr ** links, sc_uint32 * size);

/*! Gets sc-links from sc-rocksdb by it substring content.
 * @param sc_substr A key substring
 * @param[out] links A pointer to sc-links
 * @param[out] size A sc-links size
 * @returns SC_TRUE, if sc-links exist.
 */
sc_bool sc_rocksdb_fs_storage_get_sc_links_by_substr(const sc_char * sc_substr, sc_addr ** links, sc_uint32 * size);

/*! Gets sc-link content string with its size.
 * @param addr A sc-link
 * @param addr A sc-link addr
 * @param[out] sc_string A content string
 * @param[out] size A content string size
 */
void sc_rocksdb_fs_storage_get_sc_string_ext(
    sc_element * element,
    sc_addr addr,
    sc_char ** sc_string,
    sc_uint32 * size);

/*! Gets sc-link content string.
 * @param addr A sc-link
 * @returns A content string.
 */
sc_char * sc_rocksdb_fs_storage_get_sc_string(sc_addr addr);

#endif  // _sc_rocksdb_fs_storage_h_
