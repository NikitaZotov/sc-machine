/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <condition_variable>
#include <chrono>
#include <mutex>
#include <utility>

#include "sc_event_subscription.hpp"

/*!
 * @class ScWaiter
 * @brief Implements common wait logic.
 */
class _SC_EXTERN ScWaiter : public ScObject
{
public:
  using DelegateFunc = std::function<void(void)>;

private:
  class Impl
  {
  public:
    Impl();
    virtual ~Impl();

    void Resolve();

    /*!
     * @brief Waits for condition to be resolved or a timeout.
     * @param timeout_ms Timeout in milliseconds.
     * @param startDelegate Delegate function to call at the start of the wait.
     * @return True if resolved, false if timeout.
     */
    bool Wait(sc_uint32 timeout_ms, DelegateFunc const & startDelegate);

  private:
    std::mutex m_mutex;              ///< Mutex for thread safety.
    std::condition_variable m_cond;  ///< Condition variable for waiting.
    bool m_isResolved = false;       ///< Flag indicating if the condition is resolved.
  };

public:
  _SC_EXTERN ~ScWaiter() override;

  /*!
   * @brief Resolves the wait condition.
   */
  _SC_EXTERN void Resolve();

  /*!
   * @brief Sets a delegate function to be called at the start of the wait.
   * @param startDelegate Delegate function.
   * @return Pointer to the current ScWaiter object.
   */
  _SC_EXTERN ScWaiter * SetOnWaitStartDelegate(DelegateFunc const & startDelegate);

  /*!
   * @brief Waits for condition to be resolved or a timeout.
   * @param timeout_ms Timeout in milliseconds.
   * @param onWaitSuccess Function to call on successful wait.
   * @param onWaitUnsuccess Function to call on unsuccessful wait.
   * @return True if resolved, false if timeout.
   */
  _SC_EXTERN bool Wait(
      sc_uint32 timeout_ms = 5000,
      std::function<void(void)> const & onWaitSuccess = {},
      std::function<void(void)> const & onWaitUnsuccess = {});

private:
  Impl m_impl;                       ///< Implementation of the wait logic.
  DelegateFunc m_waitStartDelegate;  ///< Delegate function for wait start.
};

SHARED_PTR_TYPE(ScWaiter);

/*!
 * @class ScEventWaiter
 * @brief Implements event wait logic. Should be alive while sc-memory context is alive.
 * @tparam TScEvent Type of the event.
 */
template <class TScEvent>
class _SC_EXTERN ScEventWaiter : public ScWaiter
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  friend class ScAgentContext;

protected:
  /*!
   * @brief Constructor for ScEventWaiter.
   * @param ctx A sc-memory context used to wait sc-event.
   * @param subscriptionElementAddr An address of sc-element to wait sc-event for.
   */
  _SC_EXTERN ScEventWaiter(ScMemoryContext const & ctx, ScAddr const & subscriptionElementAddr);

  /*!
   * @brief Constructor for ScEventWaiter.
   * @param ctx A sc-memory context used to wait sc-event.
   * @param eventClassAddr A type of sc-event.
   * @param subscriptionElementAddr An address of sc-element to wait sc-event for.
   */
  _SC_EXTERN ScEventWaiter(
      ScMemoryContext const & ctx,
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr);

  virtual bool OnEvent(TScEvent const & event);

private:
  ScElementaryEventSubscription<TScEvent> m_event;
};

/*!
 * @class ScConditionWaiter
 * @brief Implements waiting for a condition based on an event.
 * @tparam TScEvent Type of event.
 */
template <class TScEvent>
class _SC_EXTERN ScConditionWaiter final : public ScEventWaiter<TScEvent>
{
  static_assert(std::is_base_of<ScEvent, TScEvent>::value, "TScEvent type must be derived from ScEvent type.");

  friend class ScAgentContext;

protected:
  using DelegateCheckFunc = std::function<bool(TScEvent const &)>;

  /*!
   * @brief Constructor for ScConditionWaiter.
   * @param ctx A sc-memory context used to wait sc-event with condition.
   * @param subscriptionElementAddr An address of sc-element to wait event for.
   * @param func Delegate function to check the condition.
   */
  _SC_EXTERN ScConditionWaiter(
      ScMemoryContext const & ctx,
      ScAddr const & subscriptionElementAddr,
      DelegateCheckFunc const & func);

  /*!
   * @brief Constructor for ScConditionWaiter.
   * @param ctx A sc-memory context used to wait sc-event with condition.
   * @param eventClassAddr A type of sc-event.
   * @param subscriptionElementAddr An address of sc-element to wait event for.
   * @param func Delegate function to check the condition.
   */
  _SC_EXTERN ScConditionWaiter(
      ScMemoryContext const & ctx,
      ScAddr const & eventClassAddr,
      ScAddr const & subscriptionElementAddr,
      DelegateCheckFunc const & func);

private:
  bool OnEvent(TScEvent const & event) override;

private:
  DelegateCheckFunc m_checkFunc;
};

/*!
 * @class ScWaiterActionFinished
 * @brief Implements waiting for an action to finish.
 */
class _SC_EXTERN ScWaiterActionFinished final
  : public ScEventWaiter<ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm>>
{
  friend class ScAction;

protected:
  /*!
   * @brief Constructor for ScWaiterActionFinished.
   * @param ctx A sc-memory context used to wait while action will be finished.
   * @param actionAddr An address of the action.
   */
  _SC_EXTERN ScWaiterActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr);

private:
  bool OnEvent(ScEventAfterGenerateIncomingArc<ScType::EdgeAccessConstPosPerm> const & event) override;
};

#include "sc_event_wait.tpp"
