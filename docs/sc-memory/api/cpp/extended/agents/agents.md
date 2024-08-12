# **C++ Agents API**

!!! warning
    This documentation is correct for only versions of sc-machine that >= 0.10.0.
--- 

This API provides functionality to implement sc-agents on C++.

## **What is the agent-driven model?**

The sc-machine implements the **agent-driven model** to process information. In the agent-based model, agents exchange messages only through shared memory, adding a new agent or eliminating one or more existing agents does not result in changes to other agents, agent initiation is decentralised and most often independent of each other. 

All agents within the OSTIS Technology are divided into two classes: platform-independent, i.e. implemented only by means of SC-code, and platform-dependent, implemented by means of sc-machine API. This sc-machine presents a powerful, but simple API for developing and maintaining platform-dependent agents in C++.

All agents react to the occurrence of events in sc-memory (sc-events). That is, an agent is called implicitly when an sc-event occurs, for which type this agent is already subscribed. Knowledge about which sc-event will cause this agent to be called (awakening of this agent) is called primary initiation condition. Upon awakening, the agent checks for the presence of its full initiation condition. If the full initiation condition is successfully checked, the agent initiates an action of some class and starts interpreting it with a agent program. After executing its program, the agent can check if there is a result.

## **What does agent specification represent?**

All knowledge about an agent: *primary initiation condition*, *class of actions* it can interpret, *initiation condition*, and *result condition*, are part of **agent's specification**. This specification can be represented either in a knowledge base, using SC-code, or programmarly, using sc-machine API.

Let's describe specification for abstract sc-agent of counting power of specified set in SCs-code. An abstract sc-agent is a class of functionally equivalent sc-agents, different instances of which can be implemented in different ways. Each abstract sc-agent has a specification corresponding to it.

```scs
// Abstract sc-agent
agent_for_calculating_set_power
<- abstract_sc_agent;
=> nrel_primary_initiation_condition: 
    // Class of sc-event and listen (subscription) sc-element
    (sc_event_add_output_arc => action_initiated); 
=> nrel_sc_agent_action_class:
    // Class of actions to be interpreted by agent
    action_for_сalculating_set_power; 
=> nrel_initiation_condition_and_result: 
    (..agent_for_calculating_set_power_initiation_condition 
        => ..agent_for_calculating_set_power_result_condition);
<= nrel_sc_agent_key_sc_elements:
// Set of key sc-elements used by this agent
{
    action_initiated;
    action;
    action_for_сalculating_set_power;
    concept_set;
    nrel_set_power;
};
=> nrel_inclusion: 
    // Instance of abstract sc-agent; concrete implementation of agent in C++
    agent_for_calculating_set_power_implementation 
    (*
        <- platform_dependent_abstract_sc_agent;;
        // Set of links with paths to sources of agent programs
        <= nrel_sc_agent_program: 
        {
            [github.com/path/to/agent/sources] 
            (* => nrel_format: format_github_source_link;; *)
        };; 
    *);;

// Full initiation condition of agent
..agent_for_calculating_set_power_initiation_condition
= [*
    action_for_сalculating_set_power _-> .._action;;
    action_initiated _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
*];; 
// Agent should check by this template that initiated action is instance of 
// class `action_for_сalculating_set_power` and that it has argument.

// Full result condition of agent
..agent_for_calculating_set_power_result_condition
= [*
    action_for_сalculating_set_power _-> .._action;;
    action_initiated _-> .._action;;
    action_finished_successfully _-> .._action;;
    .._action _-> rrel_1:: .._parameter;;
    .._action _=> nrel_result:: .._result;;
*];;
// Agent should check by this template that initiated action is finished 
// and that it has result.
```

## **What are ways of providing the agent's specification?**

The sc-machine API provides two types of functionalities to implement an agent in C++:
a. when the agent's specification is represented in the knowledge base and used by the agent;
b. when the agent's specification is represented in C++ code and also used by the agent.

In both cases, the agent's specification can be static, dynamic, or semi-dynamic.

1. **Static agent specification** is a specification provided externally in the agent's class (via overriding public getters). This specification isn't saved in the knowledge base because changes in the knowledge base can't automatically alter user-specified code. To provide this type of agent specification, you can use overriding public getters in your agent's class (see documentation below).
2. **Dynamic agent specification** is a specification provided in the knowledge base or initially in the code but automatically saved into the knowledge base. After that, the first agent changes the specification of the second agent, and the second agent uses the changed specification. To implement this type of specification, use the API of `ScModule` class and the API of `ScAgentBuilder` class (see C++ Modules).
3. **Semi-dynamic agent specification** can be obtained when the specification is provided in the knowledge base or initially in the code and appended externally (via overriding public getters).

Static agent specification can be useful if you are implementing an agent in C++ for the first time or if you want to minimize the number of searches in the knowledge base. Dynamic agent specification provides the opportunity to analyze and change this specification by other agents. Semi-dynamic specification can be useful if you want to change some parts of this specification but still want the agent's calls to be quick.

## **Static agent specification**

Here, the API for implementing an agent with a static specification will be discussed. This is the easiest implementation variant to understand for you. To learn about dynamic agent specification, see [**C++ Modules API**](modules.md).

There are two main classes that you can use to implement an agent: `ScAgent` and `ScActionAgent`.

### **ScAgent**

This class can be used for all classes of agents. The example using this class is represented below.

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

// Override your agent class from `ScAgent` class and specify template argument 
// as sc-event class. Here `ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>` 
// is type of event to which the given agent reacts.
class MyAgent : public ScAgent<ScEventAddInputArc<ScType::EdgeAccessConstPosPerm>>
{
public:
  // Here you should specify class of actions which the given agent interpretes. 
  // This overriding is required.
  ScAddr GetActionClass() const override;
  // Here you should implement program of the given agent. 
  // This overriding is required.
  ScResult DoProgram(
    ScEventAddInputArc<ScType::EdgeAccessConstPosPerm> const & event, 
    ScAction & action) override;

  // Other user-defined methods.
};
```

See [**C++ Events API**](events.md) and [**C++ Event subscriptions API**](event_subscriptions.md) to learn how to use and handle sc-events.

!!! note
    Define action class as keynode in agent class or keynodes class.

!!! warning
    You must override methods `GetActionClass` and `DoProgram`.

You can specify any existing event types as a template argument to the `ScAgent` class. For example, you can create agent that will be triggered to sc-event of removing sc-element.

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class MyAgent : public ScAgent<ScEventRemoveElement>
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(
    ScEventRemoveElement const & event, ScAction & action) override;

  // Other user-defined methods.
};
```

If you want to change specification of this agent in kb, then write like this:

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

class MyAgent : public ScBaseAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(
    ScElementaryEvent const & event, ScAction & action) override;

  // Other user-defined methods.
};
```

This implementation allows to provide any sc-event type to `DoProgram`.

!!! note
    `MyAgent` is alias for ScAgent<ScElementaryEvent>`.

!!! note
    If you provide specification of your agent in knowledge base, then you don't need to override `GetActionClass`.

### **ScActionAgent**

In addition to agents that initiate actions themselves and then interpret these actions, there is a need to implement agents that interpret actions initiated by other agents. For this class of agents, it is much easier to create a initial initiation construction in the knowledge base.

This class can be only used for agents that should be triggered to sc-event of adding output sc-arc from `action_initiated` class. These agents are named action agents. Action agent interpreters action initiated by other agent.

```cpp
// File my_agent.hpp
#pragma once

#include <sc-memory/sc_agent.hpp>

// Override your agent class from `ScAgent` class and specify template argument 
// as sc-event class. Here `ScActionEvent` is type of event to which 
// the given agent reacts.
class MyAgent : public ScActionAgent
{
public:
  // Here you should specify class of actions which the given agent interpretes. 
  // This overriding is required.
  ScAddr GetActionClass() const override;
  // Here you should implement program of the given agent. 
  // This overriding is required.
  ScResult DoProgram(ScActionEvent const & event, ScAction & action) override;

  // Other user-defined methods.
};
```

!!! note
    `ScActionAgent` has default `GetInitiationConditionTemplate` that returns template that can be used to check that initiated action is action with class of specified agent.

!!! note
    `ScActionEvent` is alias for `ScEventAddOutputArc<ScType::EdgeAccessConstPosPerm>` with subscruption sc-element `action_initiated`.

### **ScAgentAbstract**

There is a base class for agents in C++. This class provides implemented methods to retrieve elements of the agent's specification from the knowledge base. All these methods can be overridden in your agent class.

This class does not provide methods to subscribe and unsubscribe a specified agent. This functionality is provided by the classes `ScAgent` and `ScActionAgent`. You should derive your agent class from these classes, not from the ScAgentAbstract class (see documentation above).

### **Reguired methods**

#### **GetActionClass**

It gets action class that the agent of specified class interpreters. If the abstract sc-agent for this agent class does not have an action class, then method will throw `utils::ExceptionInvalidState`.

```cpp
// File my_agent.cpp
#include "my_agent.hpp"

#include "keynodes/my_keynodes.hpp"

ScAddr MyAgent::GetActionClass() const
{
  // You must specify valid sc-address of action class. 
  // In other case, the given sc-agent can’t be registered in sc-memory.
  return MyKeynodes::my_action;
}
```

See [**C++ Keynodes API**](keynodes.md) and learn how to define sc-keynodes and use them for your agent.

#### **DoProgram**

In the agent's program lies its basic operating logic. Using program, the agent processes an input construction and generates an output construction.
Each agent interprets action with the help of its program.

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  // Class `ScAction` encapsulates information about sc-action. The provided 
  // action is action that the given agent interpretes right now. 
  // It belongs to `MyKeynodes::my_action` class. 
  // Actions are copyable and movable. `ScAction` is derived from `ScAddr`.
 
  // Implement logic of your agent...

  // You must specify action state in all ends of your agent program. 
  // `FinishSuccessfully` sets action as `action_finished_successfully`.
  // You can’t create object of `ScResult` via constructor, because it is 
  // private.
  return action.FinishSuccessfully(); 
}
```

!!! warning
    If you don't catch sc-exceptions in `DoProgram` then sc-machine will catch them and will warn you about it.

##### Handling action arguments

There are many variants of methods that get action arguments. Use them, they can help you to simplify code.

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  auto [argAddr1, argAddr] = action.GetArguments<2>();

  // Some logic...

  return action.FinishSuccessfully(); 
}
```

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  ScAddr const & argAddr1 = action.GetArgument(ScKeynodes::rrel_1);
  // Parameter has ScAddr type.

  // Some logic...

  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::Program(ScActionEvent const & event, ScAction & action)
{
  ScAddr const & argAddr1 = action.GetArgument(1); // size_t
  // It would be the same if we pass ScKeynodes::rrel_1 instead of 1.

  // Some logic...

  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::Program(MyEventType const & event, ScAction & action)
{
  ScAddr const & argAddr1 
    = action.GetArgument(1, MyKeynodes::default_text_link);
  // If action hasn’t the first argument, then MyKeynodes::default_text_link 
  // will return.

  // Some logic...

  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  // You can use object of ScAction as object of ScAddr.
  ScIterator3Ptr const it3 = m_memoryCtx.Iterator3(action, ..., ...);

  // Some logic...

  return action.FinishSuccessfully();
}
```

##### Handling action result

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  // Some logic...
 
  action.FormResult(foundAddr1, generatedAddr1, ...); 
  // Or you can use UpdateResult.
  return action.FinishSuccessfully();
}
```

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  // Some logic...
 
  action.SetResult(structAddr);
  return action.FinishSuccessfully();
}
```

##### Handling action finish state

```cpp
ScResult MyAgent::DoProgram(ScActionEvent const & event, ScAction & action)
{
  // Some logic...

  if (/* case 1 */)
    return action.FinishSuccessfully();
  else if (/* case 2 */)
    return action.FinishUnsuccessfully();
  else
    return action.FinishWithError();
}
```

```cpp
ScResult MyAgent::Program(MyEventType const & event, ScAction & action)
{
  action.IsInitiated(); // result: SC_TRUE
  action.IsFinished(); // result: SC_FALSE
  action.IsFinishedSucessfully(); // result: SC_FALSE

  // Some logic...

  return action.FinishSuccessfully();
}
```

!!! warning
    The API of `ScAction` provides other methods. Not use `GetResult` for initiated but not finished action and `Initiate` for initiated or finished action.

### **Not reguired methods**

#### **GetAbstractAgent**

It searches abstract agent for agent of specified class. If the agent implementation for this agent class is not included in any abstract sc-agent, then `GetAbstractAgent` will throw `utils::ExceptionInvalidState`.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetAbstractAgent() const
{
  // You must specify valid sc-address of abstract agent. 
  // In other case, the given sc-agent can’t be registered in sc-memory.
  return MyKeynodes::my_abstract_agent;
}
```

!!! warning
    Remember, if you override only this method and required methods, then other getters will return elements of specification for specified abstract agent. All not overrided getters call `GetAbstractAgent`. 

#### **GetEventClass**

It searches the sc-event class to which the agent class is subscribed. It will throw `utils::ExceptionInvalidState` if the abstract sc-agent for this agent class does not have primary initiation condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetEventClass() const
{
  // You must specify valid sc-address of event class. 
  // In other case, the given sc-agent can’t be registered in sc-memory.
  return ScKeynodes::sc_event_add_output_arc;
}
```

#### **GetEventSubscriptionElement**

This method searches sc-event subscription sc-element for which sc-event initiates. It will throw `utils::ExceptionInvalidState` if the abstract sc-agent for this agent class does not have primary initiation condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetEventSubscriptionElement() const
{
  // You must specify valid sc-address of sc-event subscription sc-element. 
  // In other case, the given sc-agent can’t be registered in sc-memory.
  return ScKeynodes::action_initiated;
}
```

!!! warning
    Don't override `GetEventClass` and `GetEventSubscriptionElement` for agent with statically specified sc-event type. Your code can't be compiled. Override them, if your agent class derive `ScAgent<ScElementaryEvent>` (`ScBaseAgent`).

#### **GetInitiationCondition**

It gets initiation condition for agent of this class. This method will throw `utils::ExceptionInvalidState` if the abstract sc-agent for this agent class does not have initiation condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetInitiationCondition() const
{
  // You must specify valid sc-address of initiation condition. 
  // In other case, the given sc-agent can’t be called.
  return ScKeynodes::my_initiation_condition;
}
```

#### **GetInitiationConditionTemplate**

You can specify the initiation condition template in code rather than in the knowledge base.

```cpp
ScTemplate MyAgent::GetInitiationConditionTemplate() const
{
  ScTemplate initiationCondition;
  // You must specify in template of initiation condition 
  // a triple in place of which you can substitute initiated 
  // event, otherwise your agent will be called even when 
  // initiation condition is fulfilled for the action of 
  // another agent.
  initiationCondition.Triple(
    ScKeynodes::action_initiated,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "_action"
  );
  // After specify triples that only apply to your agent.
  initiationCondition.Triple(
    MyKeynodes::my_action,
    ScType::EdgeAccessVarPosPerm,
    "_action"
  );
  return initiationCondition;
}
```

#### **CheckInitiationCondition**

For speed, you can implement the agent initiation condition in the form of checks on iterators.

```cpp
sc_bool MyAgent::CheckInitiationCondition(ScActionEvent const & event)
{
 // ScActionEvent is event type on which the given agent triggered. 
 // It is encapsulate information about sc-event. The provided event 
 // is event on which the agent is triggered  right now. It has methods 
 // to get information about initiated sc-event: GetUser, GetAddedArc, 
 // GetSubscriptionElement, GetArcSourceElement, GetArcTargetElement.
 // All events are not copyable and movable.
 return m_memoryCtx.HelperCheckEdge(
   ScType::EdgeAccessConstPosPerm, 
   MyKeynodes::my_action, 
   event.GetArcTargetElement());
}
// You can use all opportunities of sc-memory API to check initiation 
// condition of your agent.
```

!!! note
    Each of these methods will be called before `DoProgram` call.

!!! warning
    You can specify only one of methods: `GetInitiationCondition`, `GetInitiationConditionTemplate` and `CheckInitiationCondition`. Otherwise, your code can't be compiled.

#### **GetResultCondition**

It gets result condition for agent of this class. This method will throw `utils::ExceptionInvalidState` if the abstract sc-agent for this agent class does not have result condition.

You can redefine this method in your agent class.

```cpp
ScAddr MyAgent::GetResultCondition() const
{
  // You must specify valid sc-address of result condition. 
  // In other case, the given sc-agent can’t be finished successfully.
  return ScKeynodes::my_result_condition;
}
```

#### **GetResultConditionTemplate**

You can specify the result condition template in code rather than in the knowledge base.

```cpp
ScTemplate MyAgent::GetResultConditionTemplate() const
{
  ScTemplate resultCondition;
  // You must specify in template of result condition 
  // a triple in place of which you can substitute initiated 
  // event, otherwise your agent will be finished even when 
  // result condition is fulfilled for the action of 
  // another agent.
  resultCondition.Triple(
    ScKeynodes::action_initiated,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "_action"
  );
  // After specify triples that only apply to your agent.
  resultCondition.Triple(
    MyKeynodes::my_action,
    ScType::EdgeAccessVarPosPerm,
    "_action"
  );
  resultCondition.Quintuple(
    "_action",
    ScType::EdgeDCommonVar,
    ScType::NodeVarStruct,
    ScType::EdgeAccessVarPosPerm,
    ScKeynodes::nrel_result
  );
  return resultCondition;
}
```

#### **CheckResultCondition**

For speed, you can implement the agent result condition in the form of checks on iterators.

```cpp
sc_bool MyAgent::CheckResult(ScActionEvent const & event, ScAction & action)
{
  return m_memoryCtx.HelperCheckEdge(
    ScType::EdgeAccessConstPosPerm, action.GetResult(), MyKeynodes::my_class);
}
```

!!! note
    Each of these methods will be called after `DoProgram` call.

!!! warning
    You can specify only one of methods: `GetResultCondition`, `GetResultConditionTemplate` and `CheckResultCondition`. Otherwise, your code can't be compiled.

--- 

## **Frequently Asked Questions**