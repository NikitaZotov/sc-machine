/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/tests/sc_test.hpp>

#include "sc-memory/sc_memory.hpp"

class ScIterator5Test : public ScMemoryTest
{
protected:
  void SetUp() override
  {
    ScMemoryTest::SetUp();

    m_source = m_ctx->CreateNode(ScType::NodeConst);
    m_target = m_ctx->CreateNode(ScType::NodeVar);
    m_edge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_source, m_target);

    m_attr = m_ctx->CreateNode(ScType::NodeConst);
    m_attrEdge = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_attr, m_edge);

    ASSERT_TRUE(m_source.IsValid());
    ASSERT_TRUE(m_target.IsValid());
    ASSERT_TRUE(m_edge.IsValid());
    ASSERT_TRUE(m_attr.IsValid());
    ASSERT_TRUE(m_attrEdge.IsValid());
  }

  void TearDown() override
  {
    ScMemoryTest::TearDown();
  }

protected:
  ScAddr m_source;
  ScAddr m_target;
  ScAddr m_edge;
  ScAddr m_attr;
  ScAddr m_attrEdge;
};

TEST_F(ScIterator5Test, smoke)
{
  EXPECT_TRUE(m_source.IsValid());
  EXPECT_TRUE(m_target.IsValid());
  EXPECT_TRUE(m_edge.IsValid());
  EXPECT_TRUE(m_attr.IsValid());
  EXPECT_TRUE(m_attrEdge.IsValid());

  EXPECT_TRUE(m_ctx->IsElement(m_source));
  EXPECT_TRUE(m_ctx->IsElement(m_target));
  EXPECT_TRUE(m_ctx->IsElement(m_edge));
  EXPECT_TRUE(m_ctx->IsElement(m_attr));
  EXPECT_TRUE(m_ctx->IsElement(m_attrEdge));
}

TEST_F(ScIterator5Test, InvalidIndex)
{
  ScIterator5Ptr const iter5 = m_ctx->Iterator5(
      ScType::Node, ScType::EdgeAccessConstPosPerm, m_target, ScType::EdgeAccessConstPosPerm, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);
  EXPECT_THROW(iter5->Get(5), utils::ExceptionInvalidParams);

  ScAddrQuintuple quintuple = iter5->Get();
  EXPECT_EQ(quintuple[0], m_source);
  EXPECT_EQ(quintuple[1], m_edge);
  EXPECT_EQ(quintuple[2], m_target);
  EXPECT_EQ(quintuple[3], m_attrEdge);
  EXPECT_EQ(quintuple[4], m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAA)
{
  ScIterator5Ptr const iter5 = m_ctx->Iterator5(
      ScType::Node, ScType::EdgeAccessConstPosPerm, m_target, ScType::EdgeAccessConstPosPerm, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAA2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(sc_type_node, sc_type_arc_pos_const_perm, m_target, sc_type_arc_pos_const_perm, sc_type_node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(ScType::Node, ScType::EdgeAccessConstPosPerm, m_target, ScType::EdgeAccessConstPosPerm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAFAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(sc_type_node, sc_type_arc_pos_const_perm, m_target, sc_type_arc_pos_const_perm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAA)
{
  ScIterator5Ptr const iter5 = m_ctx->Iterator5(
      m_source, ScType::EdgeAccessConstPosPerm, ScType::Node, ScType::EdgeAccessConstPosPerm, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAA2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(m_source, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, sc_type_node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(m_source, ScType::EdgeAccessConstPosPerm, ScType::Node, ScType::EdgeAccessConstPosPerm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAAAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(m_source, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAA)
{
  ScIterator5Ptr const iter5 = m_ctx->Iterator5(
      m_source, ScType::EdgeAccessConstPosPerm, m_target, ScType::EdgeAccessConstPosPerm, ScType::Node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAA2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(m_source, sc_type_arc_pos_const_perm, m_target, sc_type_arc_pos_const_perm, sc_type_node);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAF)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(m_source, ScType::EdgeAccessConstPosPerm, m_target, ScType::EdgeAccessConstPosPerm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, FAFAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(m_source, sc_type_arc_pos_const_perm, m_target, sc_type_arc_pos_const_perm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAAAF)
{
  ScIterator5Ptr const iter5 = m_ctx->Iterator5(
      ScType::Node, ScType::EdgeAccessConstPosPerm, ScType::NodeVar, ScType::EdgeAccessConstPosPerm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}

TEST_F(ScIterator5Test, AAAAF2)
{
  ScIterator5Ptr const iter5 =
      m_ctx->Iterator5(sc_type_node, sc_type_arc_pos_const_perm, sc_type_node, sc_type_arc_pos_const_perm, m_attr);

  EXPECT_TRUE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), m_source);
  EXPECT_EQ(iter5->Get(1), m_edge);
  EXPECT_EQ(iter5->Get(2), m_target);
  EXPECT_EQ(iter5->Get(3), m_attrEdge);
  EXPECT_EQ(iter5->Get(4), m_attr);

  EXPECT_FALSE(iter5->Next());

  EXPECT_EQ(iter5->Get(0), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(1), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(2), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(3), ScAddr::Empty);
  EXPECT_EQ(iter5->Get(4), ScAddr::Empty);
}
