/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <sc-memory/test/sc_test.hpp>

#include <sc-memory/sc_keynodes.hpp>
#include <sc-memory/utils/sc_keynode_cache.hpp>

using ScKeynodesTest = ScMemoryTest;

TEST_F(ScKeynodesTest, CoreKeynodes)
{
  EXPECT_TRUE(ScKeynodes::nrel_inclusion.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_inclusion) == ScType::ConstNodeNonRole);

  EXPECT_TRUE(ScKeynodes::rrel_1.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::rrel_1) == ScType::ConstNodeRole);
  EXPECT_TRUE(ScKeynodes::rrel_2.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::rrel_2) == ScType::ConstNodeRole);
  EXPECT_TRUE(ScKeynodes::rrel_3.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::rrel_3) == ScType::ConstNodeRole);
  EXPECT_TRUE(ScKeynodes::nrel_basic_sequence.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_basic_sequence) == ScType::ConstNodeNonRole);

  EXPECT_TRUE(ScKeynodes::rrel_key_sc_element.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::rrel_key_sc_element) == ScType::ConstNodeRole);
  EXPECT_TRUE(ScKeynodes::rrel_main_key_sc_element.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::rrel_main_key_sc_element) == ScType::ConstNodeRole);

  EXPECT_TRUE(ScKeynodes::nrel_idtf.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_idtf) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::nrel_main_idtf.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_main_idtf) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::nrel_system_identifier.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_system_identifier) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::nrel_scs_global_idtf.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_scs_global_idtf) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::lang_ru.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::lang_ru) == ScType::ConstNodeClass);

  EXPECT_TRUE(ScKeynodes::action.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::receptor_action.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::receptor_action) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::effector_action.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::effector_action) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::behavioral_action.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::behavioral_action) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::information_action.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::information_action) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_state.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_state) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_deactivated.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_deactivated) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_initiated.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_initiated) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_finished.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_finished) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_finished_successfully.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_finished_successfully) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_finished_unsuccessfully.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_finished_unsuccessfully) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::action_finished_with_error.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::action_finished_with_error) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::nrel_result.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_result) == ScType::ConstNodeNonRole);

  EXPECT_TRUE(ScKeynodes::abstract_sc_agent.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::abstract_sc_agent) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::nrel_primary_initiation_condition.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_primary_initiation_condition) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::nrel_sc_agent_action_class.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_sc_agent_action_class) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::nrel_initiation_condition_and_result.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_initiation_condition_and_result) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::platform_dependent_abstract_sc_agent.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::platform_dependent_abstract_sc_agent) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::platform_independent_abstract_sc_agent.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::platform_independent_abstract_sc_agent) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::nrel_sc_agent_program.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::nrel_sc_agent_program) == ScType::ConstNodeNonRole);

  EXPECT_TRUE(ScKeynodes::sc_event.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::sc_event_after_generate_connector.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_after_generate_connector) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_after_generate_incoming_arc.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_after_generate_incoming_arc) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_after_generate_outgoing_arc.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_after_generate_outgoing_arc) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_before_erase_connector.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_before_erase_connector) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_before_erase_incoming_arc.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_before_erase_incoming_arc) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::nrel_max_customer_waiting_time_for_action_to_finish.IsValid());
  EXPECT_TRUE(
      m_ctx->GetElementType(ScKeynodes::nrel_max_customer_waiting_time_for_action_to_finish)
      == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_before_erase_outgoing_arc.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_before_erase_outgoing_arc) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_before_erase_edge.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_before_erase_edge) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_before_erase_element.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_before_erase_element) == ScType::ConstNodeNonRole);
  EXPECT_TRUE(ScKeynodes::sc_event_before_change_link_content.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::sc_event_before_change_link_content) == ScType::ConstNodeNonRole);

  EXPECT_TRUE(ScKeynodes::binary_type.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_type) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_float.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_float) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_double.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_double) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_int8.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_int8) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_int16.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_int16) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_int32.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_int32) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_int64.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_int64) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_uint8.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_uint8) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_uint16.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_uint16) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_uint16.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_uint64) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_string.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_string) == ScType::ConstNodeClass);
  EXPECT_TRUE(ScKeynodes::binary_custom.IsValid());
  EXPECT_TRUE(m_ctx->GetElementType(ScKeynodes::binary_custom) == ScType::ConstNodeClass);
}

TEST_F(ScKeynodesTest, GetRrelIndex)
{
  EXPECT_EQ(ScKeynodes::GetRrelIndexNum(), 20u);
  EXPECT_THROW(ScKeynodes::GetRrelIndex(0), utils::ExceptionInvalidParams);
  EXPECT_TRUE(ScKeynodes::GetRrelIndex(1).IsValid());
  EXPECT_TRUE(ScKeynodes::GetRrelIndex(ScKeynodes::GetRrelIndexNum()).IsValid());
  EXPECT_THROW(ScKeynodes::GetRrelIndex(ScKeynodes::GetRrelIndexNum() + 1), utils::ExceptionInvalidParams);
}

TEST_F(ScKeynodesTest, KeynodeToString)
{
  EXPECT_EQ(std::string(ScKeynodes::action), "action");
}

TEST_F(ScKeynodesTest, Events)
{
  ScAddr keynodes[] = {
      ScKeynodes::sc_event_unknown,
      ScKeynodes::sc_event_after_generate_connector,
      ScKeynodes::sc_event_after_generate_incoming_arc,
      ScKeynodes::sc_event_after_generate_outgoing_arc,
      ScKeynodes::sc_event_after_generate_edge,
      ScKeynodes::sc_event_before_erase_connector,
      ScKeynodes::sc_event_before_erase_incoming_arc,
      ScKeynodes::sc_event_before_erase_outgoing_arc,
      ScKeynodes::sc_event_before_erase_edge,
      ScKeynodes::sc_event_before_erase_element,
      ScKeynodes::sc_event_before_change_link_content,
  };

  for (ScAddr event : keynodes)
    EXPECT_TRUE(m_ctx->CheckConnector(ScKeynodes::sc_event, event, ScType::ConstPermPosArc));
}

TEST_F(ScKeynodesTest, BinaryTypes)
{
  ScAddr keynodes[] = {
      ScKeynodes::binary_double,
      ScKeynodes::binary_float,
      ScKeynodes::binary_int8,
      ScKeynodes::binary_int16,
      ScKeynodes::binary_int32,
      ScKeynodes::binary_custom,
      ScKeynodes::binary_uint8,
      ScKeynodes::binary_uint16,
      ScKeynodes::binary_uint32,
      ScKeynodes::binary_uint64,
      ScKeynodes::binary_string,
  };

  for (ScAddr a : keynodes)
    EXPECT_TRUE(m_ctx->CheckConnector(ScKeynodes::binary_type, a, ScType::ConstPermPosArc));
}

TEST_F(ScKeynodesTest, CopyKeynode)
{
  ScKeynode keynode = ScKeynodes::action_finished;
  EXPECT_TRUE(keynode.IsValid());

  ScKeynode keynodeCopy = keynode;
  EXPECT_EQ(keynodeCopy, keynode);

  keynodeCopy = keynode;
  EXPECT_EQ(keynodeCopy, keynode);

  keynodeCopy = keynodeCopy;
  EXPECT_EQ(keynodeCopy, keynode);
}

TEST_F(ScKeynodesTest, Cache)
{
  utils::ScKeynodeCache cache(*m_ctx);

  ScAddr const addr = m_ctx->GenerateNode(ScType::ConstNode);

  EXPECT_TRUE(addr.IsValid());
  EXPECT_TRUE(m_ctx->SetElementSystemIdentifier("test_idtf", addr));

  EXPECT_TRUE(cache.GetKeynode("test_idtf").IsValid());
  EXPECT_TRUE(cache.GetKeynode("test_idtf").IsValid());
  EXPECT_FALSE(cache.GetKeynode("other").IsValid());
  EXPECT_FALSE(cache.GetKeynode("any_idtf").IsValid());
}
