/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <gtest/gtest.h>
#include <filesystem>

#include "sc_server_test.hpp"
#include "../../sc_client.hpp"

#include "sc_memory_config.hpp"
#include "../../sc_memory_json_converter.hpp"

void TestRequestCreateElements(ScClient & client)
{
  std::string const payloadString = ScMemoryJsonConverter::From(
      0,
      "create_elements",
      ScMemoryJsonPayload::array({
          {
              {"el", "node"},
              {"type", sc_type_node | sc_type_const},
          },
          {
              {"el", "link"},
              {"type", sc_type_link | sc_type_const},
              {"content", "edge_end"},
          },
          {
              {"el", "edge"},
              {"src",
               {
                   {"type", "ref"},
                   {"value", 0},
               }},
              {"trg",
               {
                   {"type", "ref"},
                   {"value", 1},
               }},
              {"type", sc_type_arc_pos_const_perm},
          },
          {
              {"el", "link"},
              {"type", sc_type_link | sc_type_const},
              {"content", 100},
          },
          {
              {"el", "link"},
              {"type", sc_type_link | sc_type_const},
              {"content", 100.0f},
          },
      }));
  EXPECT_TRUE(client.Send(payloadString));
}

void TestCheckSuccessfulCreateElements(std::unique_ptr<ScMemoryContext> const & ctx, ScClient & client)
{
  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_FALSE(responsePayload.is_null());
  EXPECT_TRUE(response["status"].get<sc_bool>());
  EXPECT_TRUE(response["errors"].empty());

  ScAddr const & src = ScAddr(responsePayload[0].get<size_t>());
  EXPECT_TRUE(src.IsValid());
  EXPECT_TRUE(ctx->GetElementType(src).IsNode());
  ScAddr const & edge = ScAddr(responsePayload[2].get<size_t>());
  EXPECT_TRUE(edge.IsValid());
  EXPECT_TRUE(ctx->GetElementType(edge).IsEdge());
  ScAddr const & trg = ScAddr(responsePayload[1].get<size_t>());
  EXPECT_TRUE(trg.IsValid());
  EXPECT_TRUE(ctx->GetElementType(trg).IsLink());

  auto const & links = ctx->FindLinksByContent("edge_end");
  EXPECT_TRUE(std::find(links.begin(), links.end(), trg) != links.end());

  ScIterator3Ptr const iter3 = ctx->Iterator3(src, sc_type_arc_pos_const_perm, trg);
  EXPECT_TRUE(iter3->Next());
  EXPECT_TRUE(iter3->Get(1) == edge);

  ScAddr const & linkInt = ScAddr(responsePayload[3].get<size_t>());
  EXPECT_TRUE(linkInt.IsValid());
  sc_int contentInt;
  ctx->GetLinkContent(linkInt, contentInt);
  EXPECT_EQ(contentInt, 100);

  ScAddr const & linkFloat = ScAddr(responsePayload[4].get<size_t>());
  EXPECT_TRUE(linkFloat.IsValid());
  float contentFloat;
  ctx->GetLinkContent(linkFloat, contentFloat);
  EXPECT_EQ(contentFloat, 100.0f);
}

void TestUnsuccessfulCreateElements(std::unique_ptr<ScMemoryContext> const & ctx, ScClient & client)
{
  auto const response = client.GetResponseMessage();
  EXPECT_FALSE(response.is_null());
  auto const & responsePayload = response["payload"];
  EXPECT_TRUE(responsePayload.is_null());
  EXPECT_FALSE(response["status"].get<sc_bool>());
  EXPECT_FALSE(response["errors"].empty());
}

TEST_F(ScServerTestWithUserMode, CreateElementsByUnauthorizedUser)
{
  ScClient client;

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);

  EXPECT_TRUE(client.Connect(m_server->GetUri() + "/?user_addr=" + std::to_string(userAddr.Hash())));
  client.Run();
  TestRequestCreateElements(client);
  TestUnsuccessfulCreateElements(m_ctx, client);
  client.Stop();
}

TEST_F(ScServerTestWithUserMode, CreateElementsByAuthorizedUser)
{
  ScClient client;

  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & conceptAuthorizedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authorized_user");
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthorizedUserAddr, userAddr);

  EXPECT_TRUE(client.Connect(m_server->GetUri() + "/?user_addr=" + std::to_string(userAddr.Hash())));
  client.Run();
  TestRequestCreateElements(client);
  TestCheckSuccessfulCreateElements(m_ctx, client);
  client.Stop();
}

TEST_F(ScServerTestWithUserMode, CreateElementsByAuthorizedUser2)
{
  ScClient client;

  ScAddr const & userAddr = m_ctx->HelperResolveSystemIdtf("test_user", ScType::NodeConst);
  ScAddr const & conceptAuthorizedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authorized_user");
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthorizedUserAddr, userAddr);

  EXPECT_TRUE(client.Connect(m_server->GetUri() + "/?user_addr=test_user"));
  client.Run();
  TestRequestCreateElements(client);
  TestCheckSuccessfulCreateElements(m_ctx, client);
  client.Stop();
}

TEST_F(ScServerTestWithUserMode, CreateElementsByAuthorizedUserWithReconnect)
{
  ScAddr const & userAddr = m_ctx->CreateNode(ScType::NodeConst);
  ScAddr const & conceptAuthorizedUserAddr = m_ctx->HelperFindBySystemIdtf("concept_authorized_user");
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosTemp, conceptAuthorizedUserAddr, userAddr);

  {
    ScClient client;
    EXPECT_TRUE(client.Connect(m_server->GetUri() + "/?user_addr=" + std::to_string(userAddr.Hash())));
    client.Run();
    TestRequestCreateElements(client);
    TestCheckSuccessfulCreateElements(m_ctx, client);
    client.Stop();
  }

  {
    ScClient client;
    EXPECT_TRUE(client.Connect(m_server->GetUri() + "/?user_addr=" + std::to_string(userAddr.Hash())));
    client.Run();
    TestRequestCreateElements(client);
    TestCheckSuccessfulCreateElements(m_ctx, client);
    client.Stop();
  }
}
