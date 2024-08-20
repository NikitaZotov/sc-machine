/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "search_incoming_arcs.h"
#include "search_keynodes.h"
#include "search_utils.h"
#include "search_defines.h"
#include "search.h"

#include "sc-core/sc_helper.h"
#include "sc-core/sc_memory_headers.h"

sc_result agent_search_all_const_pos_incoming_arc(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action, result;
  sc_iterator3 *it1, *it2;
  sc_bool sys_off = SC_TRUE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &action))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // check action type
  if (sc_helper_check_arc(s_default_ctx, keynode_action_all_input_const_pos_arc, action, sc_type_arc_pos_const_perm)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  result = create_result_node();

  // find argument
  it1 = sc_iterator3_f_a_a_new(s_default_ctx, action, sc_type_arc_pos_const_perm, 0);
  if (sc_iterator3_next(it1) == SC_TRUE)
  {
    if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
      sys_off = SC_FALSE;

    // iterate incoming sc-arcs
    it2 = sc_iterator3_a_a_f_new(s_default_ctx, 0, sc_type_arc_pos_const_perm, sc_iterator3_value(it1, 2));
    while (sc_iterator3_next(it2) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))))
        continue;

      appendIntoResult(result, sc_iterator3_value(it2, 0));
      appendIntoResult(result, sc_iterator3_value(it2, 1));
    }
    sc_iterator3_free(it2);

    appendIntoResult(result, sc_iterator3_value(it1, 2));
  }
  sc_iterator3_free(it1);

  connect_result_to_action(action, result);
  finish_action(action);

  return SC_RESULT_OK;
}

// ---------------------------------------------------

sc_result agent_search_all_const_pos_incoming_arc_with_rel(sc_event_subscription const * event, sc_addr arg)
{
  sc_addr action, result;
  sc_iterator3 *it1, *it2, *it3;
  sc_bool sys_off = SC_TRUE;

  if (!sc_memory_get_arc_end(s_default_ctx, arg, &action))
    return SC_RESULT_ERROR_INVALID_PARAMS;

  // check action type
  if (sc_helper_check_arc(
          s_default_ctx, keynode_action_all_input_const_pos_arc_with_rel, action, sc_type_arc_pos_const_perm)
      == SC_FALSE)
    return SC_RESULT_ERROR_INVALID_TYPE;

  result = create_result_node();

  // get action argument
  it1 = sc_iterator3_f_a_a_new(s_default_ctx, action, sc_type_arc_pos_const_perm, 0);
  if (sc_iterator3_next(it1) == SC_TRUE)
  {
    if (IS_SYSTEM_ELEMENT(sc_iterator3_value(it1, 2)))
      sys_off = SC_FALSE;

    // iterate incoming sc-arcs
    it2 = sc_iterator3_a_a_f_new(s_default_ctx, 0, sc_type_arc_pos_const_perm, sc_iterator3_value(it1, 2));
    while (sc_iterator3_next(it2) == SC_TRUE)
    {
      if (sys_off == SC_TRUE
          && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it2, 1))))
        continue;

      // iterate relations
      it3 = sc_iterator3_a_a_f_new(s_default_ctx, 0, sc_type_arc_pos_const_perm, sc_iterator3_value(it2, 1));
      while (sc_iterator3_next(it3) == SC_TRUE)
      {
        if (sys_off == SC_TRUE
            && (IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 0)) || IS_SYSTEM_ELEMENT(sc_iterator3_value(it3, 1))))
          continue;

        appendIntoResult(result, sc_iterator3_value(it3, 0));
        appendIntoResult(result, sc_iterator3_value(it3, 1));
      }
      sc_iterator3_free(it3);

      appendIntoResult(result, sc_iterator3_value(it2, 0));
      appendIntoResult(result, sc_iterator3_value(it2, 1));
    }
    sc_iterator3_free(it2);

    appendIntoResult(result, sc_iterator3_value(it1, 2));
  }
  sc_iterator3_free(it1);

  connect_result_to_action(action, result);
  finish_action(action);

  return SC_RESULT_OK;
}
