/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-memory/sc_agent_builder.hpp"

#include "sc-memory/sc_agent_context.hpp"
#include "sc-memory/sc_action.hpp"

#include "sc-memory/sc_keynodes.hpp"

template <class TScAgent>
ScAgentBuilder<TScAgent>::ScAgentBuilder(ScAddr const & agentImplementationAddr) noexcept
  : ScAgentBuilder(nullptr, agentImplementationAddr)
{
}

template <class TScAgent>
ScAgentBuilder<TScAgent>::ScAgentBuilder(ScModule * module, ScAddr const & agentImplementationAddr) noexcept
  : m_module(module)
  , m_agentImplementationAddr(agentImplementationAddr)
{
}

template <class TScAgent>
ScAddr ScAgentBuilder<TScAgent>::GetAgentImplementation() const noexcept
{
  return m_agentImplementationAddr;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetAbstractAgent(ScAddr const & abstractAgentAddr) noexcept
{
  m_checkAbstractAgent = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_abstractAgentAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified abstract agent for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    ScIterator3Ptr it3 =
        context->CreateIterator3(ScKeynodes::abstract_sc_agent, ScType::ConstPermPosArc, m_abstractAgentAddr);
    if (!it3->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-element for agent class `"
              << TScAgent::template GetName<TScAgent>()
              << "` is not abstract agent, because it does not belong to class `abstract_sc_agent`.");
  };

  m_abstractAgentAddr = abstractAgentAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetPrimaryInitiationCondition(
    std::tuple<ScAddr, ScAddr> const & primaryInitiationCondition) noexcept
{
  auto [eventClassAddr, eventSubscriptionElementAddr] = primaryInitiationCondition;

  m_checkPrimaryInitiationCondition = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_eventClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-event class for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    if (!context->IsElement(m_eventSubscriptionElementAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-event subscription sc-element for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                         << "` is not valid.");
  };

  m_eventClassAddr = eventClassAddr;
  m_eventSubscriptionElementAddr = eventSubscriptionElementAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetActionClass(ScAddr const & actionClassAddr) noexcept
{
  m_checkActionClass = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_actionClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified action class for agent class `" << TScAgent::template GetName<TScAgent>() << "` is not valid.");

    if (!ScAction::IsActionClassValid(context, m_actionClassAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified sc-element for agent class `"
              << TScAgent::template GetName<TScAgent>()
              << "` is not action class, because it is not included to the one of classes: `receptor_action`, "
                 "`effector_action`, `behavioral_action` or `information_action`.");
  };

  m_actionClassAddr = actionClassAddr;
  return this;
}

template <class TScAgent>
ScAgentBuilder<TScAgent> * ScAgentBuilder<TScAgent>::SetInitiationConditionAndResult(
    std::tuple<ScAddr, ScAddr> const & initiationCondition) noexcept
{
  auto [initiationConditionAddr, resultConditionAddr] = initiationCondition;

  m_checkInitiationConditionAndResult = [this](ScMemoryContext * context)
  {
    if (!context->IsElement(m_initiationConditionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified initiation condition template for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                      << "` is not valid.");

    if (!context->IsElement(m_resultConditionAddr))
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidParams,
          "Specified result condition template for agent class `" << TScAgent::template GetName<TScAgent>()
                                                                  << "` is not valid.");
  };

  m_initiationConditionAddr = initiationConditionAddr;
  m_resultConditionAddr = resultConditionAddr;
  return this;
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveSpecification(ScMemoryContext * context) noexcept(false)
{
  std::string const & agentClassName = TScAgent::template GetName<TScAgent>();
  std::string agentImplementationName;

  ResolveAgentImplementation(context, agentImplementationName, agentClassName);
  ResolveAbstractAgent(context, agentImplementationName, agentClassName);

  std::string abstractAgentName = context->GetElementSystemIdentifier(m_abstractAgentAddr);
  if (abstractAgentName.empty())
    abstractAgentName = std::to_string(m_abstractAgentAddr.Hash());

  ResolvePrimaryInitiationCondition(context, abstractAgentName, agentClassName);
  ResolveActionClass(context, abstractAgentName, agentClassName);
  ResolveInitiationConditionAndResultCondition(context, abstractAgentName, agentClassName);
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveAgentImplementation(
    ScMemoryContext * context,
    std::string & agentImplementationName,
    std::string const & agentClassName) noexcept(false)
{
  auto const & ValidateAgentImplementation = [&]()
  {
    bool isPlatformIndependentAgent = false;
    ScIterator3Ptr it3 = context->CreateIterator3(
        ScKeynodes::platform_dependent_abstract_sc_agent, ScType::ConstPermPosArc, m_agentImplementationAddr);
    if (!it3->Next())
    {
      ScIterator3Ptr const it3 = context->CreateIterator3(
          ScKeynodes::platform_independent_abstract_sc_agent, ScType::ConstPermPosArc, m_agentImplementationAddr);
      if (!it3->Next())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidState,
            "Specified sc-element with system identifier `"
                << agentImplementationName << "` is not agent implementation for agent class `" << agentClassName
                << "`, because it does not belong to the one of classes: `platform_dependent_abstract_sc_agent` or "
                   "`platform_independent_abstract_sc_agent`.");

      isPlatformIndependentAgent = true;
    }

    if (it3->Next())
      SC_LOG_WARNING(
          "Specified agent implementation with system identifier `"
          << agentImplementationName << "` for agent class `" << agentClassName << "` belongs to class `"
          << (isPlatformIndependentAgent ? "platform_independent_abstract_sc_agent"
                                         : "platform_dependent_abstract_sc_agent")
          << "` twice.");

    if (!isPlatformIndependentAgent)
    {
      it3 = context->CreateIterator3(
          ScKeynodes::platform_independent_abstract_sc_agent, ScType::ConstPermPosArc, m_agentImplementationAddr);
      if (it3->Next())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidState,
            "Specified agent implementation `"
                << agentImplementationName << "` for agent class `" << agentClassName
                << "` is invalid, because it belongs to both `platform_dependent_abstract_sc_agent` class and "
                   "`platform_independent_abstract_sc_agent` class.");
    }
  };

  if (context->IsElement(m_agentImplementationAddr))
  {
    agentImplementationName = context->GetElementSystemIdentifier(m_agentImplementationAddr);

    ValidateAgentImplementation();

    SC_LOG_DEBUG("Agent implementation for agent class `" << agentClassName << "` was found.");
    return;
  }

  agentImplementationName = agentClassName;

  m_agentImplementationAddr = context->SearchElementBySystemIdentifier(agentImplementationName);
  if (context->IsElement(m_agentImplementationAddr))
  {
    ValidateAgentImplementation();

    SC_LOG_DEBUG(
        "Agent implementation for class `" << agentClassName << "` was not generated, because it already exists.");
  }
  else
  {
    m_agentImplementationAddr = context->GenerateNode(ScType::ConstNode);
    context->SetElementSystemIdentifier(agentImplementationName, m_agentImplementationAddr);
    context->GenerateConnector(
        ScType::ConstPermPosArc, ScKeynodes::platform_dependent_abstract_sc_agent, m_agentImplementationAddr);
    SC_LOG_DEBUG("Agent implementation for class `" << agentClassName << "` was generated.");
  }
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveAbstractAgent(
    ScMemoryContext * context,
    std::string const & agentImplementationName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_abstractAgentAddr))
  {
    ScIterator5Ptr const it5 = context->CreateIterator5(
        m_abstractAgentAddr,
        ScType::ConstCommonArc,
        m_agentImplementationAddr,
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_inclusion);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and agent implementation for class `"
          << agentClassName << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & arcAddr =
          context->GenerateConnector(ScType::ConstCommonArc, m_abstractAgentAddr, m_agentImplementationAddr);
      context->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_inclusion, arcAddr);
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and agent implementation for class `" << agentClassName
                                                                                             << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->CreateIterator5(
      ScType::Unknown,
      ScType::ConstCommonArc,
      m_agentImplementationAddr,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_inclusion);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName
                                 << "` is not included to any abstract sc-agent. Check that agent implementation has "
                                    "specified relation `nrel_inclusion`.");

  m_abstractAgentAddr = it5->Get(0);

  bool hasSeveralConnectionsToElement = false;
  bool hasSeveralElements = false;
  while (it5->Next())
  {
    if (m_abstractAgentAddr == it5->Get(0))
      hasSeveralConnectionsToElement = true;
    else
    {
      hasSeveralElements = true;
      break;
    }
  }

  if (hasSeveralConnectionsToElement)
    SC_LOG_WARNING(
        "Agent implementation `" << agentImplementationName << "` is included to the same abstract agent twice.");

  if (hasSeveralElements)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Agent implementation `" << agentImplementationName << "` is included to two or more abstract agents.");

  ScType const & type = context->GetElementType(m_abstractAgentAddr);
  if (type.BitAnd(ScType::ConstNode) != ScType::ConstNode)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
            << agentImplementationName
            << "`, because sc-element does not have sc-type `ScType::ConstNode`, it has sc-type `" << std::string(type)
            << "`.");

  ScIterator3Ptr const it3 =
      context->CreateIterator3(ScKeynodes::abstract_sc_agent, ScType::ConstPermPosArc, m_abstractAgentAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_inclusion` is not abstract sc-agent for agent implementation `"
            << agentImplementationName << "`, because it does not belong to class `abstract_sc_agent`.");

  if (it3->Next())
    SC_LOG_WARNING(
        "Abstract sc-agent for agent implementation `" << agentImplementationName
                                                       << "` belongs to class `abstract_sc_agent` twice.");

  SC_LOG_DEBUG("Abstract agent for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolvePrimaryInitiationCondition(
    ScMemoryContext * context,
    std::string const & abstractAgentName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_eventClassAddr) && context->IsElement(m_eventSubscriptionElementAddr))
  {
    ScIterator5Ptr const it5 = context->CreateIterator5(
        m_abstractAgentAddr,
        ScType::ConstCommonArc,
        ScType::ConstCommonArc,
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_primary_initiation_condition);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Primary initiation condition for class `" << agentClassName
                                                     << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & primaryConditionAddr =
          context->GenerateConnector(ScType::ConstCommonArc, m_eventClassAddr, m_eventSubscriptionElementAddr);
      ScAddr const & arcAddr =
          context->GenerateConnector(ScType::ConstCommonArc, m_abstractAgentAddr, primaryConditionAddr);
      context->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_primary_initiation_condition, arcAddr);
      SC_LOG_DEBUG("Primary initiation condition for class  `" << agentClassName << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->CreateIterator5(
      m_abstractAgentAddr,
      ScType::ConstCommonArc,
      ScType::Unknown,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_primary_initiation_condition);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << abstractAgentName
            << "` does not have primary initiation condition. Check that abstract sc-agent has specified "
               "relation `nrel_primary_initiation_condition`.");

  ScAddr const & primaryInitiationConditionAddr = it5->Get(2);

  bool hasSeveralConnectionsToElement = false;
  bool hasSeveralElements = false;
  while (it5->Next())
  {
    if (primaryInitiationConditionAddr == it5->Get(2))
      hasSeveralConnectionsToElement = true;
    else
    {
      hasSeveralElements = true;
      break;
    }
  }

  if (hasSeveralConnectionsToElement)
    SC_LOG_WARNING(
        "Abstract sc-agent `" << abstractAgentName
                              << "` has two or more connections to the same pair of primary initiation condition.");

  if (hasSeveralElements)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName
                              << "` has two or more connections to different pairs of primary initiation condition.");

  ScType const & type = context->GetElementType(primaryInitiationConditionAddr);
  if (type.BitAnd(ScType::ConstCommonArc) != ScType::ConstCommonArc)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_primary_initiation_condition` is not primary initiation condition for "
        "abstract sc-agent `"
            << abstractAgentName
            << "`, because found sc-element does not have sc-type `ScType::ConstCommonArc`, it has sc-type `" << type
            << "`.");

  auto [m_eventClassAddr, m_eventSubscriptionElementAddr] =
      context->GetConnectorIncidentElements(primaryInitiationConditionAddr);

  ScIterator3Ptr const it3 = context->CreateIterator3(ScKeynodes::sc_event, ScType::ConstPermPosArc, m_eventClassAddr);
  if (!it3->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "The first element of primary initiation condition for abstract sc-agent `"
            << abstractAgentName << "` is not sc-event class, because it doesn't belong to class `sc_event`.");

  if (it3->Next())
    SC_LOG_WARNING(
        "Found sc-event class for abstract sc-agent `" << abstractAgentName << "` belongs to class `sc_event` twice.");

  SC_LOG_DEBUG("Primary initiation condition for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveActionClass(
    ScMemoryContext * context,
    std::string const & abstractAgentName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_actionClassAddr))
  {
    ScIterator5Ptr const it5 = context->CreateIterator5(
        m_abstractAgentAddr,
        ScType::ConstCommonArc,
        m_actionClassAddr,
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_sc_agent_action_class);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and action class for agent class `"
          << agentClassName << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & arcAddr =
          context->GenerateConnector(ScType::ConstCommonArc, m_abstractAgentAddr, m_actionClassAddr);
      context->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_sc_agent_action_class, arcAddr);
      SC_LOG_DEBUG(
          "Connection between specified abstract agent and action class for agent class `" << agentClassName
                                                                                           << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr it5 = context->CreateIterator5(
      m_abstractAgentAddr,
      ScType::ConstCommonArc,
      ScType::Unknown,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_sc_agent_action_class);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName
                              << "` does not have action class. Check that abstract sc-agent has specified "
                                 "relation `nrel_sc_agent_action_class`.");

  m_actionClassAddr = it5->Get(2);

  bool hasSeveralConnectionsToElement = false;
  bool hasSeveralElements = false;
  while (it5->Next())
  {
    if (m_actionClassAddr == it5->Get(2))
      hasSeveralConnectionsToElement = true;
    else
    {
      hasSeveralElements = true;
      break;
    }
  }

  if (hasSeveralConnectionsToElement)
    SC_LOG_WARNING(
        "Abstract sc-agent `" << abstractAgentName << "` has two or more connections to the same action class.");

  if (hasSeveralElements)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `" << abstractAgentName << "` has two or more connections to different action classes.");

  ScType const & type = context->GetElementType(m_actionClassAddr);
  if (type.BitAnd(ScType::ConstNodeClass) != ScType::ConstNodeClass)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_sc_agent_action_class` is not action class for abstract sc-agent `"
            << abstractAgentName
            << "`, because sc-element does not have sc-type `ScType::ConstNodeClass`, it has sc-type `" << type
            << "`.");

  if (!ScAction::IsActionClassValid(context, m_actionClassAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_sc_agent_action_class` is not action class for abstract sc-agent `"
            << abstractAgentName
            << "`, because it is not included to the one of classes: `receptor_action`, `effector_action`, "
               "`behavioral_action` or `information_action`.");

  SC_LOG_DEBUG("Action class for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::ResolveInitiationConditionAndResultCondition(
    ScMemoryContext * context,
    std::string const & abstractAgentName,
    std::string const & agentClassName) noexcept(false)
{
  if (context->IsElement(m_initiationConditionAddr) && context->IsElement(m_resultConditionAddr))
  {
    ScIterator5Ptr const it5 = context->CreateIterator5(
        m_abstractAgentAddr,
        ScType::ConstCommonArc,
        ScType::ConstCommonArc,
        ScType::ConstPermPosArc,
        ScKeynodes::nrel_initiation_condition_and_result);
    if (it5->Next())
      SC_LOG_DEBUG(
          "Initiation condition and result for class `" << agentClassName
                                                        << "` was not generated, because it already exists.");
    else
    {
      ScAddr const & conditionAndResultAddr =
          context->GenerateConnector(ScType::ConstCommonArc, m_initiationConditionAddr, m_resultConditionAddr);
      ScAddr const & arcAddr =
          context->GenerateConnector(ScType::ConstCommonArc, m_abstractAgentAddr, conditionAndResultAddr);
      context->GenerateConnector(ScType::ConstPermPosArc, ScKeynodes::nrel_initiation_condition_and_result, arcAddr);
      SC_LOG_DEBUG("Initiation condition and result for class  `" << agentClassName << "` was generated.");
    }
    return;
  }

  ScIterator5Ptr const it5 = context->CreateIterator5(
      m_abstractAgentAddr,
      ScType::ConstCommonArc,
      ScType::Unknown,
      ScType::ConstPermPosArc,
      ScKeynodes::nrel_initiation_condition_and_result);
  if (!it5->Next())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << abstractAgentName
            << "` does not have initiation condition and result. Check that abstract sc-agent has specified "
               "relation `nrel_initiation_condition_and_result`.");

  ScAddr const & initiationConditionAndResultAddr = it5->Get(2);

  bool hasSeveralConnectionsToElement = false;
  bool hasSeveralElements = false;
  while (it5->Next())
  {
    if (initiationConditionAndResultAddr == it5->Get(2))
      hasSeveralConnectionsToElement = true;
    else
    {
      hasSeveralElements = true;
      break;
    }
  }

  if (hasSeveralConnectionsToElement)
    SC_LOG_WARNING(
        "Abstract sc-agent `" << abstractAgentName
                              << "` has two or more connections to the same pair of initiation condition and result.");

  if (hasSeveralElements)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Abstract sc-agent `"
            << abstractAgentName
            << "` has two or more connections to different pairs of initiation condition and result.");

  ScType type = context->GetElementType(initiationConditionAndResultAddr);
  if (type.BitAnd(ScType::ConstCommonArc) != ScType::ConstCommonArc)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Found sc-element by relation `nrel_initiation_condition_and_result` is initiation condition and result for "
        "abstract sc-agent `"
            << abstractAgentName
            << "`, because sc-element does not have sc-type `ScType::ConstCommonArc`, it has sc-type `"
            << std::string(type) << "`.");

  auto const [m_initiationConditionAddr, m_resultConditionAddr] =
      context->GetConnectorIncidentElements(initiationConditionAndResultAddr);
  type = context->GetElementType(m_initiationConditionAddr);
  if (type.BitAnd(ScType::ConstNodeStructure) != ScType::ConstNodeStructure)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Initiation condition for abstract sc-agent `"
            << abstractAgentName
            << "` is not sc-template, because it does not have sc-type `ScType::ConstNodeStructure`, it has sc-type `"
            << std::string(type) << "`.");

  type = context->GetElementType(m_resultConditionAddr);
  if (type.BitAnd(ScType::ConstNodeStructure) != ScType::ConstNodeStructure)
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Result condition for abstract sc-agent `"
            << abstractAgentName
            << "` is not sc-template, because it does not have sc-type `ScType::ConstNodeStructure`, it has sc-type `"
            << std::string(type) << "`.");

  SC_LOG_DEBUG("Initiation condition and result for agent class `" << agentClassName << "` was found.");
}

template <class TScAgent>
ScModule * ScAgentBuilder<TScAgent>::FinishBuild() noexcept
{
  m_resolveSpecification = [this](ScMemoryContext * context)
  {
    ResolveSpecification(context);
  };

  return m_module;
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::Initialize(ScMemoryContext * context) noexcept(false)
{
  if (m_checkAbstractAgent)
    m_checkAbstractAgent(context);

  if (m_checkPrimaryInitiationCondition)
    m_checkPrimaryInitiationCondition(context);

  if (m_checkActionClass)
    m_checkActionClass(context);

  if (m_checkInitiationConditionAndResult)
    m_checkInitiationConditionAndResult(context);

  if (m_resolveSpecification)
    m_resolveSpecification(context);
}

template <class TScAgent>
void ScAgentBuilder<TScAgent>::Shutdown(ScMemoryContext *) noexcept(false)
{
}
