/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryCheckElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScAgentContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload &)
      override
  {
    ScMemoryJsonPayload responsePayload;

    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<ScAddr::HashType>()};

      size_t type = 0;
      if (addr.IsValid())
        type = context->GetElementType(addr);

      responsePayload.push_back(type);
    }

    return responsePayload;
  }
};
