#include "sc_test.hpp"

#include "sc-memory/sc_agent_context.hpp"
#include "sc-memory/sc_struct.hpp"

using ScAgentContextTest = ScMemoryTest;

TEST_F(ScAgentContextTest, MoveAgentContext)
{
  ScAgentContext context1;
  ScAgentContext context2 = ScAgentContext();
  EXPECT_TRUE(context2.IsValid());

  context1 = std::move(context2);

  EXPECT_TRUE(context1.IsValid());
  EXPECT_FALSE(context2.IsValid());

  context1 = std::move(context1);

  EXPECT_TRUE(context1.IsValid());
  EXPECT_FALSE(context2.IsValid());

  ScAgentContext context3 = std::move(context2);
  EXPECT_FALSE(context3.IsValid());
}

TEST_F(ScAgentContextTest, CreateSet)
{
  ScSet set = m_ctx->CreateSet();
  EXPECT_EQ(m_ctx->GetElementType(set), ScType::NodeConst);

  ScSet set2 = m_ctx->UseSet(set);
  EXPECT_EQ(set, set2);
}

TEST_F(ScAgentContextTest, CreateStruct)
{
  ScStructure structure = m_ctx->CreateStructure();
  EXPECT_EQ(m_ctx->GetElementType(structure), ScType::NodeConstStruct);

  ScSet structure2 = m_ctx->UseStructure(structure);
  EXPECT_EQ(structure, structure2);
}
