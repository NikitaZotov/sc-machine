/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_action.hpp"

#include "sc_agent_context.hpp"
#include "sc_structure.hpp"

template <std::size_t N>
_SC_EXTERN auto ScAction::GetArguments() noexcept
{
  return GetArgumentsImpl(std::make_index_sequence<N>{});
}

template <std::size_t... Is>
_SC_EXTERN auto ScAction::GetArgumentsImpl(std::index_sequence<Is...>) noexcept
{
  return std::make_tuple(GetArgument(Is + 1)...);
}

template <class... TScAddr>
_SC_EXTERN ScAction & ScAction::SetArguments(TScAddr const &... arguments)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  size_t i = 0;
  for (ScAddr const & argumentAddr : ScAddrVector{arguments...})
    SetArgument(++i, argumentAddr);

  return *this;
}

template <class... TScAddr>
ScAction & ScAction::FormResult(TScAddr const &... addrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to form result for `" << this->Hash() << "` with class `" << GetClass().Hash()
                                        << "` because it had already been finished.");

  if (m_resultAddr.IsValid())
  {
    m_context->EraseElement(m_resultAddr);
    m_resultAddr = m_context->CreateNode(ScType::NodeConstStruct);
  }

  ScStructure resultStruct = m_context->ConvertToStructure(m_resultAddr);
  for (ScAddr const & addr : ScAddrVector{addrs...})
    resultStruct << addr;

  return *this;
}

template <class... TScAddr>
_SC_EXTERN ScAction & ScAction::UpdateResult(TScAddr const &... addrs)
{
  static_assert(
      (std::is_base_of<ScAddr, TScAddr>::value && ...), "Each element of parameter pack must have ScAddr type.");

  if (IsFinished())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState,
        "Not able to update result for `" << this->Hash() << "` with class `" << GetClass().Hash()
                                          << "` because it had already been finished.");

  if (!m_resultAddr.IsValid())
    m_resultAddr = m_context->CreateNode(ScType::NodeConstStruct);

  ScStructure resultStruct = m_context->ConvertToStructure(m_resultAddr);
  for (ScAddr const & addr : ScAddrVector{addrs...})
    resultStruct << addr;

  return *this;
}
