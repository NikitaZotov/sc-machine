/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

extern "C"
{
#include "sc-core/sc_memory.h"
#include "sc-core/sc-container/sc_string.h"
#include "sc-core/sc-container/sc_container_iterator.h"
}

#include "sc_test.hpp"

TEST_F(ScMemoryTest, sc_memory_find_links_with_content_string)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const link_addr = sc_memory_link_new2(context, sc_type_link | sc_type_const);

  sc_char content[] = "content";
  sc_stream * stream = sc_stream_memory_new(content, sc_str_len(content), SC_STREAM_FLAG_READ, SC_FALSE);
  EXPECT_EQ(sc_memory_set_link_content(context, link_addr, stream), SC_RESULT_OK);

  sc_list * link_hashes;
  sc_memory_find_links_with_content_string(context, stream, &link_hashes);
  sc_iterator * it = sc_list_iterator(link_hashes);
  while (sc_iterator_next(it))
  {
    auto found_link_hash = (sc_addr_hash)sc_iterator_get(it);
    sc_addr found_addr;
    SC_ADDR_LOCAL_FROM_INT(found_link_hash, found_addr);
    EXPECT_TRUE(SC_ADDR_IS_EQUAL(found_addr, link_addr));
  }
  sc_iterator_destroy(it);

  sc_stream_free(stream);
  sc_list_destroy(link_hashes);
}

TEST_F(ScMemoryTest, sc_memory_find_links_by_content_substring)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const link_addr = sc_memory_link_new2(context, sc_type_link | sc_type_const);

  sc_char content[] = "content";
  sc_stream * stream = sc_stream_memory_new(content, sc_str_len(content), SC_STREAM_FLAG_READ, SC_FALSE);
  EXPECT_EQ(sc_memory_set_link_content(context, link_addr, stream), SC_RESULT_OK);

  sc_list * link_hashes;
  sc_memory_find_links_by_content_substring(context, stream, 1, &link_hashes);
  sc_iterator * it = sc_list_iterator(link_hashes);
  while (sc_iterator_next(it))
  {
    auto found_link_hash = (sc_addr_hash)sc_iterator_get(it);
    sc_addr found_addr;
    SC_ADDR_LOCAL_FROM_INT(found_link_hash, found_addr);
    EXPECT_TRUE(SC_ADDR_IS_EQUAL(found_addr, link_addr));
  }
  sc_iterator_destroy(it);

  sc_stream_free(stream);
  sc_list_destroy(link_hashes);
}

TEST_F(ScMemoryTest, sc_memory_find_links_contents_by_content_substring)
{
  sc_memory_context * context = **m_ctx;
  sc_addr const link_addr = sc_memory_link_new2(context, sc_type_link | sc_type_const);

  sc_char content[] = "content";
  sc_stream * stream = sc_stream_memory_new(content, sc_str_len(content), SC_STREAM_FLAG_READ, SC_FALSE);
  EXPECT_EQ(sc_memory_set_link_content(context, link_addr, stream), SC_RESULT_OK);

  sc_list * string_hashes;
  sc_memory_find_links_contents_by_content_substring(context, stream, 1, &string_hashes);
  sc_iterator * it = sc_list_iterator(string_hashes);
  while (sc_iterator_next(it))
  {
    auto * found_string = (sc_char *)sc_iterator_get(it);
    EXPECT_TRUE(sc_str_cmp(found_string, content));
  }
  sc_iterator_destroy(it);

  sc_stream_free(stream);
  sc_list_clear(string_hashes);
  sc_list_destroy(string_hashes);
}
