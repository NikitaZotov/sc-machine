#include <gtest/gtest.h>

#include "sc-memory/sc_agent.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_event_wait.hpp"

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

class ScWaiterTest : public ScMemoryTest
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

TEST_F(ScWaiterTest, Smoke)
{
  EXPECT_TRUE(m_addr.IsValid());
}

TEST_F(ScWaiterTest, Valid)
{
  WaitTestData data(m_addr);
  auto eventWaiter = m_ctx->CreateEventWaiter<ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      });
  EXPECT_TRUE(eventWaiter->Wait(
      5000,
      [&data]()
      {
        data.m_isDone = true;
      }));
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaiterTest, TimeOut)
{
  EXPECT_FALSE(
      m_ctx->CreateEventWaiter<ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>>(m_addr)->Wait(1000));
}

TEST_F(ScWaiterTest, CondValid)
{
  WaitTestData data(m_addr);
  auto waiter = m_ctx->CreateConditionWaiter<ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const &) -> sc_bool
      {
        return SC_TRUE;
      });

  EXPECT_TRUE(waiter->Wait());
  EXPECT_TRUE(data.m_isDone);
}

TEST_F(ScWaiterTest, CondValidFalse)
{
  WaitTestData data(m_addr);

  auto waiter = m_ctx->CreateConditionWaiter<ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>(
      m_addr,
      [&data]()
      {
        EmitEvent(data);
      },
      [](ScEventGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const &) -> sc_bool
      {
        return SC_FALSE;
      });

  sc_bool result = SC_TRUE;
  EXPECT_FALSE(waiter->Wait(
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

TEST_F(ScWaiterTest, InvalidWaiters)
{
  ScAddr nodeAddr{23124323};

  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventGenerateOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventGenerateIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventGenerateEdge<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventEraseOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventEraseIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateEventWaiter<ScEventEraseEdge<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateEventWaiter<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidWaitersWithCondition)
{
  ScAddr nodeAddr{23124323};

  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventGenerateOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventGenerateIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventGenerateEdge<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventEraseOutgoingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventEraseIncomingArc<ScType::EdgeAccess>>(nodeAddr, {}),
      utils::ExceptionInvalidParams);
  EXPECT_THROW(
      m_ctx->CreateConditionWaiter<ScEventEraseEdge<ScType::EdgeAccess>>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateConditionWaiter<ScEventEraseElement>(nodeAddr, {}), utils::ExceptionInvalidParams);
  EXPECT_THROW(m_ctx->CreateConditionWaiter<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateConditionWaiter<ScEventChangeLinkContent>(nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidEventsFotWaiters)
{
  ScAddr nodeAddr{23124323};
  ScAddr eventClassAddr{23124323};
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateEventWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}

TEST_F(ScWaiterTest, InvalidEventsFotWaitersWithConditions)
{
  ScAddr nodeAddr{23124323};
  ScAddr eventClassAddr{23124323};
  EXPECT_THROW(m_ctx->CreateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);

  nodeAddr = m_ctx->CreateNode(ScType::NodeConst);
  EXPECT_THROW(m_ctx->CreateConditionWaiter(eventClassAddr, nodeAddr, {}), utils::ExceptionInvalidParams);
}
