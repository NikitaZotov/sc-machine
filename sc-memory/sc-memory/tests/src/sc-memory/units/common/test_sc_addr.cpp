/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "sc-memory/sc_memory.hpp"

TEST(ScAddrTest, Hash)
{
  sc_addr a;
  a.offset = 123;
  a.seg = 654;

  ScAddr addr1(a);
  auto const hash = addr1.Hash();
  ScAddr const addr2(hash);

  EXPECT_EQ(addr1, addr2);

  addr1.Reset();
  EXPECT_EQ(addr1, ScAddr::Empty);
}
