/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>

#include "sc-memory/sc_agent.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_wait.hpp"

#include "sc_test.hpp"

namespace
{

// waiters threads
struct WaitTestData
{
  WaitTestData(ScAddr const & addr, ScAddr const & addrFrom = ScAddr())
    : m_addr(addr)
    , m_addrFrom(addrFrom)
    , m_isDone(false)
  {
  }

  ScAddr m_addr;
  ScAddr m_addrFrom;
  bool m_isDone;
};

void EmitEvent(WaitTestData & data)
{
  ScMemoryContext ctx;

  ScAddr const node = data.m_addrFrom.IsValid() ? data.m_addrFrom : ctx.CreateNode(ScType::NodeConst);
  ScAddr const edge = ctx.CreateEdge(ScType::EdgeAccessConstPosPerm, node, data.m_addr);

  ASSERT_TRUE(node.IsValid());
  ASSERT_TRUE(edge.IsValid());

  data.m_isDone = edge.IsValid();
}

}  // namespace

class ScWaitTest : public ScMemoryTest
{
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_addr = m_ctx->CreateNode(ScType::NodeConst);
    ASSERT_TRUE(m_addr.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_addr;
};

TEST_F(ScWaitTest, smoke)
{
  EXPECT_TRUE(m_addr.IsValid());
}

TEST_F(ScWaitTest, valid)
{
  WaitTestData data(m_addr);
  ScWaitEvent<ScEventAddInputEdge> waiter(*m_ctx, m_addr);
  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });
  EXPECT_TRUE(waiter.Wait(
      5000,
      [&data]()
      {
        data.m_isDone = true;
      }));
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, TimeOut)
{
  EXPECT_FALSE(ScWaitEvent<ScEventAddOutputEdge>(*m_ctx, m_addr).Wait(1000));
}

TEST_F(ScWaitTest, CondValid)
{
  WaitTestData data(m_addr);
  ScWaitCondition<ScEventAddInputEdge> waiter(
      *m_ctx,
      m_addr,
      [](ScAddr const &, ScAddr const &, ScAddr const &) -> sc_result
      {
        return SC_RESULT_OK;
      });

  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });

  EXPECT_TRUE(waiter.Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, CondValidFalse)
{
  WaitTestData data(m_addr);

  ScWaitCondition<ScEventAddInputEdge> waiter(
      *m_ctx,
      m_addr,
      [](ScAddr const &, ScAddr const &, ScAddr const &) -> sc_result
      {
        return SC_RESULT_NO;
      });

  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });

  sc_bool result = SC_TRUE;
  EXPECT_FALSE(waiter.Wait(
      2000,
      [&result]()
      {
        result = SC_TRUE;
      },
      [&result]()
      {
        result = SC_FALSE;
      }));
  EXPECT_FALSE(result);
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaitTest, ActionFinished)
{
  WaitTestData data(m_addr, ScKeynodes::kQuestionFinished);

  ScWaitActionFinished waiter(*m_ctx, m_addr);
  waiter.SetOnWaitStartDelegate(
      [&data]()
      {
        EmitEvent(data);
      });

  EXPECT_TRUE(waiter.Wait(
      5000,
      [&data]()
      {
        data.m_isDone = true;
      }));
  EXPECT_TRUE(data.m_isDone);
}
