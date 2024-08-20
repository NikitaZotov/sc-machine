/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _seach_outgoing_arcs_h_
#define _seach_outgoing_arcs_h_

#include "sc-core/sc_memory.h"

/*!
 * Function that implement sc-agent to search all positive, constant and permanent
 * outgoing sc-arcs for specified sc-element
 */
sc_result agent_search_all_const_pos_outgoing_arc(sc_event_subscription const * event, sc_addr arg);

/*! Function that implemets sc-agent to search all positive, constant and permanent
 * outgoing sc-arcs with relations for specified sc-element
 */
sc_result agent_search_all_const_pos_outgoing_arc_with_rel(sc_event_subscription const * event, sc_addr arg);

#endif
