/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_action.hpp"

#include "sc_agent_context.hpp"
#include "sc_result.hpp"
#include "sc_wait.hpp"
#include "sc_struct.hpp"

ScAction::ScAction(ScAgentContext * ctx, ScAddr const & actionAddr, ScAddr const & actionClassAddr)
  : ScAddr(actionAddr)
  , m_ctx(ctx)
  , m_actionClassAddr(actionClassAddr)
  , m_answerAddr(ScAddr::Empty){};

ScAction::ScAction(ScAgentContext * ctx, ScAddr const & actionClassAddr)
  : ScAddr(ctx->CreateNode(ScType::NodeConst))
  , m_ctx(ctx)
  , m_actionClassAddr(actionClassAddr)
  , m_answerAddr(ScAddr::Empty)
{
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_actionClassAddr, *this);
};

ScAddr ScAction::GetClass() const
{
  return m_actionClassAddr;
}

ScAddr ScAction::GetArgument(size_t idx, ScAddr const & defaultArgumentAddr) const
{
  return GetArgument(ScKeynodes::GetRrelIndex(idx), defaultArgumentAddr);
}

ScAddr ScAction::GetArgument(ScAddr const & orderRelationAddr, ScAddr const & defaultArgumentAddr) const
{
  ScIterator5Ptr const it = m_ctx->Iterator5(
      *this, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, orderRelationAddr);

  if (it->Next())
    return it->Get(2);

  return defaultArgumentAddr;
}

ScAction & ScAction::SetArgument(size_t idx, ScAddr const & argumentAddr)
{
  return SetArgument(ScKeynodes::GetRrelIndex(idx), argumentAddr);
}

ScAction & ScAction::SetArgument(ScAddr const & orderRelationAddr, ScAddr const & argumentAddr)
{
  ScIterator5Ptr const it = m_ctx->Iterator5(
      *this, ScType::EdgeAccessConstPosPerm, ScType::Unknown, ScType::EdgeAccessConstPosPerm, orderRelationAddr);

  if (it->Next())
    m_ctx->EraseElement(it->Get(1));

  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, *this, argumentAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, orderRelationAddr, arcAddr);

  return *this;
}

ScStruct ScAction::GetAnswer() noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get answer of action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                             << "` due it had not been initiated yet.");

  if (!IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to get answer of action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                             << "` due it had not been finished yet.");

  if (!m_answerAddr.IsValid())
  {
    ScIterator5Ptr const & it5 = m_ctx->Iterator5(
        *this, ScType::EdgeDCommonConst, ScType::Unknown, ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_answer);
    if (!it5->Next())
      SC_THROW_EXCEPTION(
          utils::ExceptionInvalidState,
          "Action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash() << "` has not answer structure.");

    m_answerAddr = it5->Get(2);
  }
  return ScStruct{m_ctx, m_answerAddr};
}

ScAction & ScAction::SetAnswer(ScAddr const & structureAddr) noexcept(false)
{
  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to set answer for `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                       << "` due it had already been finished.");

  if (m_answerAddr == structureAddr)
    return *this;

  if (!m_ctx->IsElement(structureAddr))
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Not able to set structure for action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                                 << "`due settable structure is not valid.");

  if (m_answerAddr.IsValid())
    m_ctx->EraseElement(m_answerAddr);

  m_answerAddr = structureAddr;
  return *this;
}

sc_bool ScAction::IsInitiated() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_initiated, *this, ScType::EdgeAccessConstPosPerm);
}

sc_bool ScAction::InitiateAndWait(sc_uint32 waitTime_ms) noexcept(false)
{
  if (IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                        << "` due it had already been initiated.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                        << "` due it had already been finished.");

  auto wait = std::make_shared<ScWaitActionFinished>(*m_ctx, *this);
  wait->SetOnWaitStartDelegate(
      [this]()
      {
        m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_initiated, *this);
      });
  return wait->Wait(waitTime_ms);
};

ScAction & ScAction::Initiate() noexcept(false)
{
  if (IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                        << "` due it had already been initiated.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to initiate action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                        << "` due it had already been finished.");

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_initiated, *this);

  return *this;
};

void ScAction::Finish(ScAddr const & actionStateAddr) noexcept(false)
{
  if (!IsInitiated())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to finish action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                      << "` due it had not been initiated yet.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to finish action `" << this->Hash() << "` with class `" << m_actionClassAddr.Hash()
                                      << "` due it had already been initiated.");

  if (!m_answerAddr.IsValid())
    m_answerAddr = m_ctx->CreateNode(ScType::NodeConstStruct);

  ScAddr const & arcAddr = m_ctx->CreateEdge(ScType::EdgeDCommonConst, *this, m_answerAddr);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::nrel_answer, arcAddr);

  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, actionStateAddr, *this);
  m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, ScKeynodes::action_finished, *this);
}

sc_bool ScAction::IsFinished() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished, *this, ScType::EdgeAccessConstPosPerm);
}

sc_bool ScAction::IsFinishedSuccessfully() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished_successfully, *this, ScType::EdgeAccessConstPosPerm);
}

ScResult ScAction::FinishSuccessfully() noexcept(false)
{
  Finish(ScKeynodes::action_finished_successfully);
  return SC_RESULT_OK;
}

sc_bool ScAction::IsFinishedUnsuccessfully() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished_unsuccessfully, *this, ScType::EdgeAccessConstPosPerm);
}

ScResult ScAction::FinishUnsuccessfully() noexcept(false)
{
  Finish(ScKeynodes::action_finished_unsuccessfully);
  return SC_RESULT_NO;
}

sc_bool ScAction::IsFinishedWithError() const
{
  return m_ctx->HelperCheckEdge(ScKeynodes::action_finished_with_error, *this, ScType::EdgeAccessConstPosPerm);
}

ScResult ScAction::FinishWithError() noexcept(false)
{
  Finish(ScKeynodes::action_finished_with_error);
  return SC_RESULT_ERROR;
}
