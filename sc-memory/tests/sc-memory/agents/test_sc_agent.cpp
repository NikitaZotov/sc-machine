#include "test_sc_agent.hpp"

/// --------------------------------------

ScAddr ATestGenerateConnector::GetActionClass() const
{
  return ATestGenerateConnector::generate_connector_action;
}

ScResult ATestGenerateConnector::DoProgram(
    ScEventAfterGenerateConnector<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateIncomingArc::GetActionClass() const
{
  return ATestGenerateIncomingArc::generate_incoming_arc_action;
}

ScResult ATestGenerateIncomingArc::DoProgram(
    ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateOutgoingArc::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGenerateOutgoingArc::DoProgram(
    ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateEdge::GetActionClass() const
{
  return ATestGenerateEdge::add_edge_action;
}

ScResult ATestGenerateEdge::DoProgram(ScEventAfterGenerateEdge<ScType::EdgeUCommonConst> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseConnector::GetActionClass() const
{
  return ATestEraseConnector::erase_connector_action;
}

ScResult ATestEraseConnector::DoProgram(
    ScEventBeforeEraseConnector<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseIncomingArc::GetActionClass() const
{
  return ATestEraseIncomingArc::erase_incoming_arc_action;
}

ScResult ATestEraseIncomingArc::DoProgram(
    ScEventBeforeEraseIncomingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseOutgoingArc::GetActionClass() const
{
  return ATestEraseOutgoingArc::erase_outgoing_arc_action;
}

ScResult ATestEraseOutgoingArc::DoProgram(
    ScEventBeforeEraseOutgoingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseEdge::GetActionClass() const
{
  return ATestEraseEdge::erase_edge_action;
}

ScResult ATestEraseEdge::DoProgram(ScEventBeforeEraseEdge<ScType::EdgeUCommonConst> const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestEraseElement::GetActionClass() const
{
  return ATestEraseElement::erase_element_action;
}

ScResult ATestEraseElement::DoProgram(ScEventBeforeEraseElement const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestChangeLinkContent::GetActionClass() const
{
  return ATestChangeLinkContent::content_change_action;
}

ScResult ATestChangeLinkContent::DoProgram(ScEventBeforeChangeLinkContent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGenerateMultipleOutputArc::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGenerateMultipleOutputArc::DoProgram(
    ScEventAfterGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm> const &,
    ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestDoProgramOne::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestDoProgramOne::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestDoProgramTwo::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestDoProgramTwo::DoProgram(ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestDoProgram::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScAddr ATestDoProgram::GetAbstractAgent() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;  // invalid abstract agent
}

ScResult ATestDoProgram::DoProgram(ScAction & action)
{
  GetAbstractAgent();

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestException::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestException::DoProgram(ScAction & action)
{
  SC_THROW_EXCEPTION(utils::ScException, "Some exception.");
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestCheckResult::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestCheckResult::DoProgram(ScActionEvent const &, ScAction & action)
{
  auto [firstArgument, secondArgument] = action.GetArguments<2>();

  if (!firstArgument.IsValid())
  {
    msWaiter.Unlock();
    return action.FinishWithError();
  }

  if (!secondArgument.IsValid())
  {
    msWaiter.Unlock();
    return action.FinishUnsuccessfully();
  }

  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetInitiationConditionTemplate::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScTemplate ATestGetInitiationConditionTemplate::GetInitiationConditionTemplate() const
{
  ScTemplate initiationCondition;
  initiationCondition.Triple(GetActionClass(), ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action");
  initiationCondition.Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);
  return initiationCondition;
}

ScResult ATestGetInitiationConditionTemplate::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestCheckInitiationCondition::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

bool ATestCheckInitiationCondition::CheckInitiationCondition(ScActionEvent const & event)
{
  return m_context.HelperCheckEdge(
             ATestGenerateOutgoingArc::generate_outgoing_arc_action,
             event.GetArcTargetElement(),
             ScType::EdgeAccessConstPosPerm)
         && m_context.Iterator3(event.GetArcTargetElement(), ScType::EdgeAccessConstPosPerm, ScType::NodeConst)->Next();
}

ScResult ATestCheckInitiationCondition::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

/// --------------------------------------

ScAddr ATestGetResultConditionTemplate::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestGetResultConditionTemplate::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

ScTemplate ATestGetResultConditionTemplate::GetResultConditionTemplate() const
{
  ScTemplate initiationCondition;
  initiationCondition.Triple(GetActionClass(), ScType::EdgeAccessVarPosPerm, ScType::NodeVar >> "_action");
  initiationCondition.Triple("_action", ScType::EdgeAccessVarPosPerm, ScType::NodeVar);
  return initiationCondition;
}

/// --------------------------------------

ScAddr ATestCheckResultCondition::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestCheckResultCondition::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}

bool ATestCheckResultCondition::CheckResultCondition(ScActionEvent const & event, ScAction & action)
{
  return m_context.HelperCheckEdge(ScKeynodes::action_finished_successfully, action, ScType::EdgeAccessConstPosPerm)
         && m_context.Iterator3(event.GetArcTargetElement(), ScType::EdgeAccessConstPosPerm, ScType::NodeConst)->Next();
}

/// --------------------------------------

ScAddr ATestActionDeactivated::GetActionClass() const
{
  return ATestGenerateOutgoingArc::generate_outgoing_arc_action;
}

ScResult ATestActionDeactivated::DoProgram(ScActionEvent const &, ScAction & action)
{
  msWaiter.Unlock();
  return action.FinishSuccessfully();
}
