/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_memory_make_template_json_action.hpp"

class ScMemoryTemplateGenerateJsonAction : public ScMemoryMakeTemplateJsonAction
{
public:
  ScMemoryJsonPayload Complete(ScAgentContext * context, ScMemoryJsonPayload requestPayload, ScMemoryJsonPayload &)
      override
  {
    ScTemplateGenResult result;
    auto pair = GetTemplate(context, requestPayload);
    context->GenerateByTemplate(*pair.first, result, pair.second);

    std::vector<size_t> hashesVectors;
    for (size_t i = 0; i < result.Size(); ++i)
      hashesVectors.push_back(result[i].Hash());

    SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN
    ScMemoryJsonPayload const & resultPayload = {{"aliases", result.GetReplacements()}, {"addrs", hashesVectors}};
    SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
    delete pair.first;
    return resultPayload;
  }
};
