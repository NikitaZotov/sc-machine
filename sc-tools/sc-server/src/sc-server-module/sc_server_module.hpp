/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_module.hpp>

#include <sc-config-utils/sc_memory_config.hpp>

#include "sc-server-impl/sc_server.hpp"

class _SC_EXTERN ScServerModule final : public ScModule
{
public:
  SC_MODULE_BODY(ScServerModule);

  static std::shared_ptr<ScServer> m_server;
  static ScParams ms_serverParams;

  sc_result Initialize(ScMemoryContext * ctx, ScAddr const & initMemoryGeneratedStructureAddr) override;
  sc_result Shutdown(ScMemoryContext * ctx) override;
};
