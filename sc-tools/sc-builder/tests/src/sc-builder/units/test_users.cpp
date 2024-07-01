/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "builder_test.hpp"

#include <thread>

#include <sc-core/sc_keynodes.h>

#define SC_LOCK_WAIT_WHILE_TRUE(expression) \
  ({ \
    sc_uint32 retries = 50; \
    sc_uint32 i = 0; \
    while (expression && i < retries) \
    { \
      std::this_thread::sleep_for(std::chrono::milliseconds(10)); \
      ++i; \
    } \
  })

void TestAuthenticationRequestUser(
    std::unique_ptr<ScMemoryContext> const & context,
    ScAddr const & userAddr,
    ScType const & arcType = ScType::EdgeAccessConstPosTemp)
{
  ScAddr const & conceptAuthenticationRequestUserAddr{concept_authentication_request_user_addr};
  context->CreateEdge(arcType, conceptAuthenticationRequestUserAddr, userAddr);
}

TEST_F(ScBuilderLoadUserPermissionsTest, UserWithGlobalReadPermissionsAndWithLocalWritePermissions)
{
  ScAddr const & userAddr = m_ctx->HelperFindBySystemIdtf("test_user_1");

  TestScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&userContext, &isAuthenticated](
          ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr) -> sc_result
      {
        ScAddr const & otherUserAddr = userContext.HelperFindBySystemIdtf("test_user_2");
        ScAddr const & classAddr = userContext.CreateNode(ScType::NodeConstClass);
        EXPECT_THROW(
            userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, classAddr, otherUserAddr),
            utils::ExceptionInvalidState);
        ScAddr const & userStructureAddr = userContext.HelperFindBySystemIdtf("user_structure");
        EXPECT_THROW(userContext.EraseElement(userStructureAddr), utils::ExceptionInvalidState);
        EXPECT_NO_THROW(userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, userStructureAddr, classAddr));

        return (isAuthenticated = true) ? SC_RESULT_OK : SC_RESULT_ERROR;
      });

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}

TEST_F(ScBuilderLoadUserPermissionsTest, UserWithGlobalReadPermissionsAndWithoutLocalWritePermissions)
{
  ScAddr const & userAddr = m_ctx->HelperFindBySystemIdtf("test_user_2");

  TestScMemoryContext userContext{userAddr};
  EXPECT_THROW(userContext.CreateNode(ScType::NodeConst), utils::ExceptionInvalidState);

  ScAddr const & conceptAuthenticatedUserAddr{concept_authenticated_user_addr};
  std::atomic_bool isAuthenticated = false;
  ScEventAddOutputEdge event(
      *m_ctx,
      conceptAuthenticatedUserAddr,
      [&userContext, &isAuthenticated](
          ScAddr const & addr, ScAddr const & edgeAddr, ScAddr const & userAddr) -> sc_result
      {
        ScAddr const & otherUserAddr = userContext.HelperFindBySystemIdtf("test_user_2");
        ScAddr const & classAddr = userContext.CreateNode(ScType::NodeConstClass);
        EXPECT_THROW(
            userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, classAddr, otherUserAddr),
            utils::ExceptionInvalidState);
        ScAddr const & userStructureAddr = userContext.HelperFindBySystemIdtf("user_structure");
        EXPECT_THROW(userContext.EraseElement(userStructureAddr), utils::ExceptionInvalidState);
        EXPECT_THROW(
            userContext.CreateEdge(ScType::EdgeAccessConstPosTemp, userStructureAddr, classAddr),
            utils::ExceptionInvalidState);

        return (isAuthenticated = true) ? SC_RESULT_OK : SC_RESULT_ERROR;
      });

  TestAuthenticationRequestUser(m_ctx, userAddr);

  SC_LOCK_WAIT_WHILE_TRUE(!isAuthenticated.load());
  EXPECT_TRUE(isAuthenticated.load());
}