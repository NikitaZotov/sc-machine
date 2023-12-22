/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_addr.hpp>

namespace utils
{
class CommonUtils
{
public:
  static bool checkType(ScMemoryContext * ms_context, const ScAddr & element, ScType scType);

  SC_DEPRECATED(
      0.9.0,
      "Use bool GetLinkContent(ScAddr const & addr, TContentType & typedContent) instead of. It will be removed in "
      "sc-machine 0.10.0.")
  static std::string getLinkContent(ScMemoryContext * ms_context, const ScAddr & scLink);

  static std::string getIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & idtfRelation,
      const ScAddrVector & linkClasses = {});

  static std::string getMainIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddrVector & linkClasses = {});

  static void setIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const ScAddr & relation,
      const std::string & identifier,
      const ScAddrVector & linkClasses = {});

  static void setMainIdtf(
      ScMemoryContext * ms_context,
      const ScAddr & node,
      const std::string & identifier,
      const ScAddrVector & linkClasses = {});

  static size_t getSetPower(ScMemoryContext * ms_context, const ScAddr & set);

  static bool isEmpty(ScMemoryContext * ms_context, const ScAddr & set);

  static std::string getAddrHashString(ScAddr const & scAddr);
};

}  // namespace utils
