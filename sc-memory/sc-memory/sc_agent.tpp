/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_agent.hpp"

#include "sc_action.hpp"
#include "sc_agent_context.hpp"
#include "sc_result.hpp"
#include "sc_event_subscription.hpp"
#include "sc_keynodes.hpp"

template <class TScEvent>
ScAgentAbstract<TScEvent>::ScAgentAbstract()
  : m_memoryCtx(nullptr)
{
}

template <class TScEvent>
ScAgentAbstract<TScEvent>::~ScAgentAbstract()
{
  m_memoryCtx.Destroy();
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetAbstractAgent() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      ScType::NodeConst,
      ScType::EdgeDCommonConst,
      m_agentImplementationAddr,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_inclusion);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << this->GetName<ScAgentAbstract<TScEvent>>()
                                 << "` is not included to any abstract sc-agent. Check that agent implementation has "
                                    "specified relation `nrel_inclusion`.");

  return it5->Get(0);
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetEventClass() const
{
  if constexpr (!std::is_same<TScEvent, ScElementaryEvent>::value)
    return TScEvent::eventClassAddr;

  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << this->GetName<ScAgentAbstract<TScEvent>>()
            << "` has not primary initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_memoryCtx.GetEdgeSource(it5->Get(2));
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetEventSubscriptionElement() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << this->GetName<ScAgentAbstract<TScEvent>>()
            << "` has not primary initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  return m_memoryCtx.GetEdgeTarget(it5->Get(2));
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetActionClass() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::NodeConstClass,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << this->GetName<ScAgentAbstract<TScEvent>>()
                              << "` has not action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  return it5->Get(2);
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::CheckInitiationCondition(TScEvent const & event)
{
  return SC_TRUE;
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetInitiationCondition() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << this->GetName<ScAgentAbstract<TScEvent>>()
            << "` has not initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_memoryCtx.GetEdgeSource(it5->Get(2));
}

template <class TScEvent>
ScTemplate ScAgentAbstract<TScEvent>::GetInitiationConditionTemplate() const
{
  return ScTemplate();
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::CheckResultCondition(TScEvent const &, ScAction &)
{
  return SC_TRUE;
}

template <class TScEvent>
ScAddr ScAgentAbstract<TScEvent>::GetResultCondition() const
{
  ScIterator5Ptr const it5 = m_memoryCtx.Iterator5(
      GetAbstractAgent(),
      ScType::EdgeDCommonConst,
      ScType::EdgeDCommonConst,
      ScType::EdgeAccessConstPosPerm,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << this->GetName<ScAgentAbstract<TScEvent>>()
            << "` has not initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  return m_memoryCtx.GetEdgeTarget(it5->Get(2));
}

template <class TScEvent>
ScTemplate ScAgentAbstract<TScEvent>::GetResultConditionTemplate() const
{
  return ScTemplate();
}

template <class TScEvent>
void ScAgentAbstract<TScEvent>::SetContext(ScAddr const & userAddr)
{
  m_memoryCtx = ScAgentContext(userAddr);
}

template <class TScEvent>
void ScAgentAbstract<TScEvent>::SetImplementation(ScAddr const & agentImplementationAddr)
{
  m_agentImplementationAddr = agentImplementationAddr;
}

template <class TScEvent>
sc_bool ScAgentAbstract<TScEvent>::MayBeSpecified() const
{
  return m_agentImplementationAddr.IsValid();
}

template <class TScEvent>
std::function<void(TScEvent const &)> ScAgentAbstract<TScEvent>::GetCallback(ScAddr const &)
{
  return {};
}

template <class TScEvent>
ScAgent<TScEvent>::ScAgent()
  : ScAgentAbstract<TScEvent>(){};

template <class TScEvent>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent>::Subscribe(
    ScMemoryContext * ctx,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetContext(ctx->GetUserAddr());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentName = agent.GetName();
  if (!ScAgentAbstract<TScEvent>::m_events.count(agentName))
    ScAgentAbstract<TScEvent>::m_events.insert({agentName, {}});

  ScAddrVector subscriptionVector{subscriptionAddrs...};
  if (agent.MayBeSpecified())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = ScAgentAbstract<TScEvent>::m_events.find(agentName)->second;
  for (ScAddr const & subscriptionAddr : subscriptionVector)
  {
    if (!ctx->IsElement(subscriptionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Not able to subscribe agent `" << agentName << "` to event `" << eventName
                                          << "due subscription sc-element with address `" << subscriptionAddr.Hash()
                                          << "` is not valid.");
    if (subscriptionsMap.find(subscriptionAddr) != subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` has already been subscribed to event `" << eventName << "("
                    << subscriptionAddr.Hash() << ")`.");

    SC_LOG_INFO(
        "Subscribe agent `" << agentName << "` to event `" << eventName << "(" << subscriptionAddr.Hash() << ")`.");

    if constexpr (std::is_same<ScElementaryEvent, TScEvent>::value)
    {
      subscriptionsMap.insert(
          {subscriptionAddr,
           new ScElementaryEventSubscription(
               *ctx,
               agent.GetEventClass(),
               subscriptionAddr,
               TScAgent::template GetCallback<TScAgent>(agentImplementationAddr))});
    }
    else
    {
      subscriptionsMap.insert(
          {subscriptionAddr,
           new TScElementaryEventSubscription<TScEvent>(
               *ctx, subscriptionAddr, TScAgent::template GetCallback<TScAgent>(agentImplementationAddr))});
    }
  }
}

template <class TScEvent>
template <class TScAgent, class... TScAddr>
void ScAgent<TScEvent>::Unsubscribe(
    ScMemoryContext * ctx,
    ScAddr const & agentImplementationAddr,
    TScAddr const &... subscriptionAddrs)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  TScAgent agent;
  agent.SetContext(ctx->GetUserAddr());
  agent.SetImplementation(agentImplementationAddr);

  std::string const & agentName = agent.GetName();
  auto const & agentsMapIt = ScAgentAbstract<TScEvent>::m_events.find(agentName);
  if (agentsMapIt == ScAgentAbstract<TScEvent>::m_events.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Agent `" << agentName << "` has not been subscribed to any events yet.");

  ScAddrVector subscriptionVector{subscriptionAddrs...};
  if (agent.MayBeSpecified())
    subscriptionVector.emplace_back(agent.GetEventSubscriptionElement());

  std::string const & eventClassName = ScEvent::GetName<TScEvent>();
  auto & subscriptionsMap = agentsMapIt->second;
  for (ScAddr const & subscriptionAddr : subscriptionVector)
  {
    if constexpr (!std::is_same<TScEvent, ScEventRemoveElement>::value)
    {
      if (!ctx->IsElement(subscriptionAddr))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Not able to unsubscribe agent `" << agentName << "` from event `" << eventClassName
                                              << "due subscription sc-element with address `" << subscriptionAddr.Hash()
                                              << "` is not valid.");
    }

    auto const & it = subscriptionsMap.find(subscriptionAddr);
    if (it == subscriptionsMap.cend())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Agent `" << agentName << "` has not been subscribed to event `" << eventClassName << "("
                    << subscriptionAddr.Hash() << ")` yet.");

    SC_LOG_INFO(
        "Unsubscribe agent `" << agentName << "` from event `" << eventClassName << "(" << subscriptionAddr.Hash()
                              << ")`.");

    delete it->second;
    subscriptionsMap.erase(subscriptionAddr);
  }
}

template <class TScEvent>
template <class TScAgent>
std::function<void(TScEvent const &)> ScAgent<TScEvent>::GetCallback(ScAddr const & agentImplementationAddr)
{
  static_assert(std::is_base_of<ScAgent, TScAgent>::value, "TScAgent type must be derived from ScAgent type.");

  return [agentImplementationAddr](TScEvent const & event) -> void
  {
    auto const & CreateAction = [](TScEvent const & event, ScAgent & agent) -> ScAction
    {
      auto [subscriptionElementAddr, _, otherElementAddr] = event.GetTriple();
      if (subscriptionElementAddr == ScKeynodes::action_initiated)
      {
        ScAddr const & actionAddr = otherElementAddr;
        return ScAction(&agent.m_memoryCtx, actionAddr, agent.GetActionClass());
      }

      return ScAction(&agent.m_memoryCtx, agent.GetActionClass()).Initiate();
    };

    TScAgent agent;
    SC_LOG_INFO("Agent `" << agent.GetName() << "` started");

    agent.SetContext(event.GetUser());
    agent.SetImplementation(agentImplementationAddr);
    if (ScMemory::ms_globalContext->HelperCheckEdge(
            ScKeynodes::action_deactivated, agent.GetActionClass(), ScType::EdgeAccessConstPosPerm))
    {
      SC_LOG_WARNING(
          "Agent `" << agent.GetName() << "` was not started due actions with class `" << agent.GetActionClass().Hash()
                    << "` are deactivated.");
      return;
    }

    ScAction action = CreateAction(event, agent);

    ScTemplate && initiationConditionTemplate = agent.GetInitiationConditionTemplate();
    if (initiationConditionTemplate.IsEmpty() && agent.MayBeSpecified())
    {
      ScAddr const & initiationConditionAddr = agent.GetInitiationCondition();
      agent.m_memoryCtx.HelperBuildTemplate(initiationConditionTemplate, initiationConditionAddr);
    }

    ScTemplateSearchResult searchResult;
    if (!initiationConditionTemplate.IsEmpty()
        && !agent.m_memoryCtx.HelperSearchTemplate(initiationConditionTemplate, searchResult))
    {
      SC_LOG_WARNING(
          "Agent `" << agent.GetName() << "` was finished due its initiation condition was tested unsuccessfully.");
      return;
    }

    ScResult result = agent.DoProgram(event, action);

    if (result == SC_RESULT_OK)
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished successfully");
    else if (result == SC_RESULT_NO)
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished unsuccessfully");
    else
      SC_LOG_INFO("Agent `" << agent.GetName() << "` finished with error");

    ScTemplate && resultConditionTemplate = agent.GetResultConditionTemplate();
    if (resultConditionTemplate.IsEmpty() && agent.MayBeSpecified())
    {
      ScAddr const & resultConditionAddr = agent.GetResultCondition();
      agent.m_memoryCtx.HelperBuildTemplate(resultConditionTemplate, resultConditionAddr);
    }

    if (!resultConditionTemplate.IsEmpty()
        && !agent.m_memoryCtx.HelperSearchTemplate(resultConditionTemplate, searchResult))
    {
      SC_LOG_WARNING("Result condition of agent `" << agent.GetName() << "` tested unsuccessfully.");
      return;
    }

    return;
  };
}

template <class TScAgent>
void ScActionAgent::Subscribe(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr)
{
  ScAgent<ScActionEvent>::Subscribe<TScAgent>(ctx, agentImplementationAddr, ScKeynodes::action_initiated);
}

template <class TScAgent>
void ScActionAgent::Unsubscribe(ScMemoryContext * ctx, ScAddr const & agentImplementationAddr)
{
  ScAgent<ScActionEvent>::Unsubscribe<TScAgent>(ctx, agentImplementationAddr, ScKeynodes::action_initiated);
}

// ScIterator5Ptr it5 = ctx->Iterator5(
//     ScType::Unknown,
//     ScType::EdgeDCommonConst,
//     agentImplementationAddr,
//     ScType::EdgeAccessConstPosPerm,
//     ScKeynodes::nrel_sc_agent_program);
// if (!it5->Next())
//   SC_THROW_EXCEPTION(
//       utils::ExceptionInvalidState,
//       "Agent implementation `" << agentImplementationName
//                                << "` has not sc-agent program. Check that agent implementation `"
//                                << agentImplementationName << " has specified relation `nrel_sc_agent_program`.");

// if (it5->Next())
//   SC_THROW_EXCEPTION(
//       utils::ExceptionInvalidState,
//       "Agent implementation `" << agentImplementationName << "` has two or more sc-agent programs.");
