#include <gtest/gtest.h>

#include <thread>

#include "sc-memory/sc_memory.hpp"

#include "sc_test.hpp"

#define SC_LOCK_WAIT_WHILE_TRUE(expression) \
  ({ \
    sc_uint32 retries = 20; \
    sc_uint32 i = 0; \
    while (expression && i < retries) \
    { \
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
      ++i; \
    } \
  })

void TestAddAccessLevelsForUserToInitActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScAddr const & actionClassAddr)
{
  ScAddr const & nrelUserActionClassAddr = context->HelperFindBySystemIdtf("nrel_user_action_class");
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeDCommonConst, userAddr, actionClassAddr);
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, nrelUserActionClassAddr, edgeAddr);
}

void TestAddAccessLevelsForUserToInitReadActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & readActionInScMemoryAddr = context->HelperFindBySystemIdtf("read_action_in_sc_memory");
  TestAddAccessLevelsForUserToInitActions(context, userAddr, readActionInScMemoryAddr);
}

void TestAddAccessLevelsForUserToInitWriteActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  ScAddr const & writeActionInScMemoryAddr = context->HelperFindBySystemIdtf("write_action_in_sc_memory");
  TestAddAccessLevelsForUserToInitActions(context, userAddr, writeActionInScMemoryAddr);
}

void TestAddRightsForUserToInitEraseActions(std::unique_ptr<ScMemoryContext> const & context, ScAddr const & userAddr)
{
  ScAddr const & eraseActionInScMemoryAddr = context->HelperFindBySystemIdtf("erase_action_in_sc_memory");
  TestAddAccessLevelsForUserToInitActions(context, userAddr, eraseActionInScMemoryAddr);
}

void TestAddAllAccessLevelsForUserToInitActions(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr)
{
  TestAddAccessLevelsForUserToInitReadActions(context, userAddr);
  TestAddAccessLevelsForUserToInitWriteActions(context, userAddr);
  TestAddRightsForUserToInitEraseActions(context, userAddr);
}

void TestAuthenticationRequestUser(std::unique_ptr<ScMemoryContext> const & context, ScAddr const & userAddr)
{
  ScAddr const & conceptAuthenticationRequestUserAddr =
      context->HelperFindBySystemIdtf("concept_authentication_request_user");
  context->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthenticationRequestUserAddr, userAddr);
}

void TestReadActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  EXPECT_EQ(userContext.GetElementInputArcsCount(nodeAddr), 0u);
  EXPECT_EQ(userContext.GetElementOutputArcsCount(nodeAddr), 1u);
  EXPECT_EQ(userContext.GetEdgeSource(edgeAddr), nodeAddr);
  EXPECT_EQ(userContext.GetEdgeTarget(edgeAddr), linkAddr);
  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_TRUE(userContext.GetEdgeInfo(edgeAddr, nodeAddr1, nodeAddr2));
  EXPECT_EQ(nodeAddr1, nodeAddr);
  EXPECT_EQ(nodeAddr2, linkAddr);
  EXPECT_EQ(userContext.GetElementType(nodeAddr), ScType::NodeConst);
  EXPECT_NO_THROW(userContext.CalculateStat());
  std::string content;
  EXPECT_FALSE(userContext.GetLinkContent(linkAddr, content));
  EXPECT_TRUE(content.empty());
  EXPECT_NO_THROW(userContext.FindLinksByContent("test"));
  EXPECT_NO_THROW(userContext.FindLinksByContentSubstring("test"));
  EXPECT_NO_THROW(userContext.FindLinksContentsByContentSubstring("test"));
  EXPECT_NO_THROW(userContext.HelperFindBySystemIdtf("test"));
  EXPECT_NO_THROW(userContext.HelperResolveSystemIdtf("test"));

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_TRUE(it3->Next());

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_FALSE(it5->Next());

  EXPECT_TRUE(userContext.HelperCheckEdge(nodeAddr, linkAddr, ScType::EdgeAccessConstPosTemp));
}

void TestReadActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);
  ScAddr const & edgeAddr = context->CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr);

  EXPECT_THROW(userContext.GetElementInputArcsCount(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementOutputArcsCount(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeSource(edgeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetEdgeTarget(edgeAddr), utils::ExceptionInvalidState);
  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_THROW(userContext.GetEdgeInfo(edgeAddr, nodeAddr1, nodeAddr2), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.GetElementType(nodeAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CalculateStat(), utils::ExceptionInvalidState);
  std::string content;
  EXPECT_THROW(userContext.GetLinkContent(linkAddr, content), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContent("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.FindLinksContentsByContentSubstring("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperFindBySystemIdtf("test"), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test"), utils::ExceptionInvalidState);

  ScIterator3Ptr it3 = userContext.Iterator3(nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown);
  EXPECT_THROW(it3->Next(), utils::ExceptionInvalidState);

  ScIterator5Ptr it5 = userContext.Iterator5(
      nodeAddr, ScType::EdgeAccessConstPosTemp, ScType::Unknown, ScType::EdgeAccessConstPosTemp, ScType::NodeConstRole);
  EXPECT_THROW(it5->Next(), utils::ExceptionInvalidState);

  EXPECT_THROW(
      userContext.HelperCheckEdge(nodeAddr, nodeAddr, ScType::EdgeAccessConstPosTemp), utils::ExceptionInvalidState);
}

void TestWriteActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);

  EXPECT_TRUE(userContext.CreateNode(ScType::NodeConst).IsValid());
  EXPECT_TRUE(userContext.CreateLink(ScType::LinkConst).IsValid());
  EXPECT_TRUE(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr).IsValid());

  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_TRUE(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst));
  EXPECT_TRUE(userContext.Save());
}

void TestWriteActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);
  ScAddr const & linkAddr = context->CreateLink(ScType::LinkConst);

  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.CreateLink(ScType::LinkConst), utils::ExceptionInvalidState);
  EXPECT_THROW(
      userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, nodeAddr, linkAddr), utils::ExceptionInvalidState);

  ScAddr nodeAddr1, nodeAddr2;
  EXPECT_THROW(userContext.SetElementSubtype(nodeAddr, ScType::NodeConst), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.Save(), utils::ExceptionInvalidState);
}

void TestEraseActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(userContext.EraseElement(nodeAddr));
}

void TestEraseActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & nodeAddr = context->CreateNode(ScType::NodeConst);

  EXPECT_THROW(userContext.EraseElement(nodeAddr), utils::ExceptionInvalidState);
}

void TestApplyActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_NO_THROW(userContext.SetLinkContent(linkAddr, "test"));
}

void TestApplyActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_THROW(userContext.SetLinkContent(linkAddr, "test"), utils::ExceptionInvalidState);
}

void TestChangeActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_NO_THROW(userContext.HelperSetSystemIdtf("test", linkAddr));
  EXPECT_NO_THROW(userContext.HelperResolveSystemIdtf("test", ScType::NodeConst));
}

void TestChangeActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  ScAddr const & linkAddr = context->CreateNode(ScType::LinkConst);

  EXPECT_THROW(userContext.HelperSetSystemIdtf("test", linkAddr), utils::ExceptionInvalidState);
  EXPECT_THROW(userContext.HelperResolveSystemIdtf("test", ScType::NodeConst), utils::ExceptionInvalidState);
}

void TestActionsSuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  TestReadActionsSuccessfully(context, userContext);
  TestWriteActionsSuccessfully(context, userContext);
  TestEraseActionsSuccessfully(context, userContext);
  TestApplyActionsSuccessfully(context, userContext);
  TestChangeActionsSuccessfully(context, userContext);
}

void TestActionsUnsuccessfully(std::unique_ptr<ScMemoryContext> const & context, ScMemoryContext & userContext)
{
  TestReadActionsUnsuccessfully(context, userContext);
  TestWriteActionsUnsuccessfully(context, userContext);
  TestEraseActionsUnsuccessfully(context, userContext);
  TestApplyActionsUnsuccessfully(context, userContext);
  TestChangeActionsUnsuccessfully(context, userContext);
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByUnauthenticatedUser)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  TestActionsUnsuccessfully(m_ctx, userContext);
}

TEST_F(ScMemoryTestWithUserMode, CreateElementsByAuthenticatedUserCreatedBefore)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestActionsSuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });

  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, CreateElementsByAuthenticatedUserCreatedAfter)
{
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");

  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        ScMemoryContext userContext{userAddr};
        TestActionsSuccessfully(m_ctx, userContext);

        isChecked = true;

        return true;
      });

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());
}

TEST_F(ScMemoryTestWithUserMode, SeveralCreateElementsByAuthenticatedUserCreatedAfter)
{
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");

  std::atomic_bool isChecked = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &isChecked](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        ScMemoryContext userContext{userAddr};
        TestActionsSuccessfully(m_ctx, userContext);

        isChecked = true;

        return true;
      });

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isChecked.load());
  EXPECT_TRUE(isChecked.load());

  {
    ScMemoryContext userContext{userAddr};
    TestActionsSuccessfully(m_ctx, userContext);
  }
  {
    ScMemoryContext userContext{userAddr};
    TestActionsSuccessfully(m_ctx, userContext);
  }
}

TEST_F(ScMemoryTestWithUserMode, CreateElementsByAuthenticatedUserCreatedBeforeAndUnauthenticatedAfter)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScMemoryContext userContext{userAddr};

  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");

  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestActionsSuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });

  TestAddAllAccessLevelsForUserToInitActions(m_ctx, userAddr);

  ScAddr const & conceptAuthenticationRequestUserAddr =
      m_ctx->HelperFindBySystemIdtf("concept_authentication_request_user");
  ScAddr const & authEdgeAddr =
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthenticationRequestUserAddr, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());

  ScEventRemoveOutputEdge event2(
      *m_ctx,
      conceptAuthenticationRequestUserAddr,
      [this, &userContext, &isAuthenticated](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = false;

        return true;
      });

  m_ctx->EraseElement(authEdgeAddr);

  SC_LOCK_WAIT_WHILE_TRUE(isAuthenticated.load());
  EXPECT_FALSE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleElementsByAuthenticatedUserWithoutRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleByAuthenticatedUserWithoutWriteAndEraseRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestReadActionsSuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleByAuthenticatedUserWithoutEraseRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestReadActionsSuccessfully(m_ctx, userContext);
        TestWriteActionsSuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  TestAddAccessLevelsForUserToInitWriteActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleByAuthenticatedUserWithoutWriteRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestReadActionsSuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsSuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAddAccessLevelsForUserToInitReadActions(m_ctx, userAddr);
  TestAddRightsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleByAuthenticatedUserWithoutReadAndEraseRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsSuccessfully(m_ctx, userContext);
        TestEraseActionsUnsuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAddAccessLevelsForUserToInitWriteActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleByAuthenticatedUserWithoutReadRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsSuccessfully(m_ctx, userContext);
        TestEraseActionsSuccessfully(m_ctx, userContext);
        TestApplyActionsSuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAddAccessLevelsForUserToInitWriteActions(m_ctx, userAddr);
  TestAddRightsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScMemoryTestWithUserMode, HandleByAuthenticatedUserWithoutReadAndWriteRights)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  ScMemoryContext userContext{userAddr};
  ScAddr const & conceptAuthenticatedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authenticated_user");
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&](ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr)
      {
        TestReadActionsUnsuccessfully(m_ctx, userContext);
        TestWriteActionsUnsuccessfully(m_ctx, userContext);
        TestEraseActionsSuccessfully(m_ctx, userContext);
        TestApplyActionsUnsuccessfully(m_ctx, userContext);
        TestChangeActionsUnsuccessfully(m_ctx, userContext);

        isAuthenticated = true;

        return true;
      });
  TestAddRightsForUserToInitEraseActions(m_ctx, userAddr);
  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}