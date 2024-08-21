/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_object.hpp"

#include "sc_type.hpp"

#include "sc_keynodes.hpp"

/*!
 * @class ScEvent
 * @brief Base class for all sc-events.
 *
 * ScEvent is an abstract class that represents a generic event in the sc-memory.
 * It provides interfaces to get a user and a subscription element associated with the event.
 */
class _SC_EXTERN ScEvent : public ScObject
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  friend class ScEventSubscriptionFactory;

public:
  _SC_EXTERN virtual ~ScEvent();

  _SC_EXTERN virtual ScAddr GetEventClass() const = 0;

  _SC_EXTERN virtual ScAddr GetUser() const = 0;

  _SC_EXTERN virtual ScAddr GetSubscriptionElement() const = 0;
};

/*!
 * @class ScEvent
 * @brief Represents a basic sc-event.
 *
 * ScEvent is a subclass of ScEvent that represents basic events in the sc-memory.
 */
class _SC_EXTERN ScElementaryEvent : public ScEvent
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;

public:
  _SC_EXTERN ScAddr GetEventClass() const override;

  _SC_EXTERN ScAddr GetUser() const override;

  _SC_EXTERN ScAddr GetSubscriptionElement() const override;

  /*!
   * @brief Gets a triple associated with the event.
   * @return An array representing the triple.
   */
  _SC_EXTERN std::tuple<ScAddr, ScAddr, ScAddr> GetTriple() const;

  /*!
   * @brief Gets a connector element outgoing from or ingoing to a subscription element.
   * @return ScAddr representing a connector element from/to a subscription element.
   */
  _SC_EXTERN ScAddr GetConnector() const;

  /*!
   * @brief Gets a type of connector element outgoing from or ingoing to a subscription element.
   * @return ScType representing a type of connector element outgoing from or ingoing to a subscription element.
   */
  _SC_EXTERN ScType GetConnectorType() const;

  /*!
   * @brief Gets other element of connector outgoing from or ingoing to subscription element.
   * @return ScAddr representing other element of connector outgoing from or ingoing to subscription element.
   */
  _SC_EXTERN ScAddr GetOtherElement() const;

protected:
  _SC_EXTERN ScElementaryEvent(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

  ScAddr m_eventClassAddr;
  ScAddr m_userAddr;
  ScAddr m_subscriptionAddr;
  ScAddr m_connectorAddr;
  ScType m_connectorType;
  ScAddr m_otherAddr;
};

/*!
 * @class ScElementaryEvent
 * @brief Represents a basic sc-event.
 *
 * ScElementaryEvent is a subclass of ScEvent that represents basic events in the sc-memory.
 */
template <ScType const & _elementType>
class _SC_EXTERN TScElementaryEvent : public ScElementaryEvent
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

protected:
  _SC_EXTERN TScElementaryEvent(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScType const & elementType = _elementType;
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_unknown;
};

/*!
 * @class ScEventGenerateArc
 * @brief Represents an event where an arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventGenerateArc : public TScElementaryEvent<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetGeneratedArc() const;

  _SC_EXTERN virtual ScType GetGeneratedArcType() const;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const;

protected:
  _SC_EXTERN ScEventGenerateArc(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);
};

/*!
 * @class ScEventGenerateEdge
 * @brief Represents an event where an edge is added.
 */
template <ScType const & edgeType>
class _SC_EXTERN ScEventGenerateEdge : public TScElementaryEvent<edgeType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

public:
  _SC_EXTERN virtual ScAddr GetGeneratedEdge() const;

  _SC_EXTERN virtual ScType GetGeneratedEdgeType() const;

  _SC_EXTERN virtual ScAddr GetEdgeSourceElement() const;

  _SC_EXTERN virtual ScAddr GetEdgeTargetElement() const;

protected:
  _SC_EXTERN ScEventGenerateEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_generate_edge;
};

/*!
 * @class ScEventGenerateOutgoingArc
 * @brief Represents an event where an outgoing sc-arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventGenerateOutgoingArc : public ScEventGenerateArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

protected:
  _SC_EXTERN ScEventGenerateOutgoingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_generate_outgoing_arc;
};

/*!
 * @class ScEventGenerateIncomingArc
 * @brief Represents an event where an incoming sc-arc is added.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventGenerateIncomingArc final : public ScEventGenerateArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

public:
  _SC_EXTERN ScAddr GetArcSourceElement() const override;

  _SC_EXTERN ScAddr GetArcTargetElement() const override;

protected:
  _SC_EXTERN ScEventGenerateIncomingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_generate_incoming_arc;
};

/*!
 * @class ScEventEraseArc
 * @brief Represents an event where an arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventEraseArc : public TScElementaryEvent<arcType>
{
public:
  _SC_EXTERN virtual ScAddr GetErasableArc() const;

  _SC_EXTERN virtual ScType GetErasableArcType() const;

  _SC_EXTERN virtual ScAddr GetArcSourceElement() const;

  _SC_EXTERN virtual ScAddr GetArcTargetElement() const;

protected:
  _SC_EXTERN ScEventEraseArc(
      ScAddr const & eventClassAddr,
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);
};

/*!
 * @class ScEventEraseEdge
 * @brief Represents an event where an edge is removed.
 */
template <ScType const & edgeType>
class _SC_EXTERN ScEventEraseEdge : public TScElementaryEvent<edgeType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

public:
  _SC_EXTERN virtual ScAddr GetErasableEdge() const;

  _SC_EXTERN virtual ScType GetErasableEdgeType() const;

  _SC_EXTERN virtual ScAddr GetEdgeSourceElement() const;

  _SC_EXTERN virtual ScAddr GetEdgeTargetElement() const;

protected:
  _SC_EXTERN ScEventEraseEdge(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_erase_edge;
};

/*!
 * @class ScEventEraseOutgoingArc
 * @brief Represents an event where an outgoing sc-arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventEraseOutgoingArc final : public ScEventEraseArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

protected:
  _SC_EXTERN ScEventEraseOutgoingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_erase_outgoing_arc;
};

/*!
 * @class ScEventEraseIncomingArc
 * @brief Represents an event where an incoming sc-arc is removed.
 */
template <ScType const & arcType>
class _SC_EXTERN ScEventEraseIncomingArc final : public ScEventEraseArc<arcType>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

public:
  _SC_EXTERN ScAddr GetArcSourceElement() const override;

  _SC_EXTERN ScAddr GetArcTargetElement() const override;

protected:
  _SC_EXTERN ScEventEraseIncomingArc(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_erase_incoming_arc;
};

#include "sc_event.tpp"

/*!
 * @class ScEventEraseElement
 * @brief Represents an event where an element is erased.
 */
class _SC_EXTERN ScEventEraseElement final : public TScElementaryEvent<ScType::Unknown>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

protected:
  _SC_EXTERN ScEventEraseElement(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_erase_element;
};

/*!
 * @class ScEventChangeLinkContent
 * @brief Represents an event where the content of an element is changed.
 */
class _SC_EXTERN ScEventChangeLinkContent final : public TScElementaryEvent<ScType::Unknown>
{
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  template <class TScEvent, class TScContext>
  friend class ScAgentAbstract;

protected:
  _SC_EXTERN ScEventChangeLinkContent(
      ScAddr const & userAddr,
      ScAddr const & subscriptionElementAddr,
      ScAddr const & connectorAddr,
      ScType const & connectorType,
      ScAddr const & otherAddr);

private:
  static inline ScAddr const & eventClassAddr = ScKeynodes::sc_event_change_link_content;
};

using ScActionEvent = ScEventGenerateOutgoingArc<ScType::EdgeAccessConstPosPerm>;
