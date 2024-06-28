/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _search_semantic_neighborhood_h_
#define _search_semantic_neighborhood_h_

#include <sc-core/sc_types.h>

/*!
 * Function that implements sc-agent to search full semantic neighborhood of specified sc-element
 */
sc_result agent_search_full_semantic_neighborhood(sc_event const * event, sc_addr arg);

/*!
 * Function that implements sc-agent to search all link of given relation, which are connected with given element
 */
sc_result agent_search_links_of_relation_connected_with_element(sc_event const * event, sc_addr arg);

#endif
