/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_keynodes.h"

#include "sc-core/sc-store/sc_iterator3.h"

sc_result sc_common_resolve_keynode(
    sc_memory_context * ctx,
    sc_char const * sys_idtf,
    sc_addr * keynode,
    sc_addr const init_memory_generated_structure)
{
  return sc_helper_resolve_system_identifier(ctx, sys_idtf, keynode);
}
