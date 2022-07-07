#pragma once

#include "sc_memory_json_action.hpp"

class ScMemoryCheckElementsJsonAction : public ScMemoryJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScMemoryContext * context, ScMemoryJsonPayload requestPayload) override
  {
    ScMemoryJsonPayload responsePayload;

    for (auto & hash : requestPayload)
    {
      ScAddr const addr{hash.get<size_t>()};

      size_t type = 0;
      if (addr.IsValid())
        type = context->GetElementType(addr);

      responsePayload.push_back(type);
    }

    return responsePayload;
  }
};