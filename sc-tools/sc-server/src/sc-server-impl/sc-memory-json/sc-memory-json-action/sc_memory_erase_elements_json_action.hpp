/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryEraseElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScAgentContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload &)
      override
  {
    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<ScAddr::HashType>()};
      context->EraseElement(addr);
    }

    return {SC_TRUE};
  }
};
