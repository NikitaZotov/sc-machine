/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_iterator3_h_
#define _sc_iterator3_h_

#include "sc_defines.h"
#include "sc_types.h"
#include "sc_element.h"

//! sc-iterator types
typedef enum
{
  sc_iterator3_f_a_a = 0,  // outgoing edges
  sc_iterator3_a_a_f,      // ingoing edges
  sc_iterator3_f_a_f,      // edge between source and target
  sc_iterator3_a_f_a,      // find source/target elements of edge
  // just for clean template search
  sc_iterator3_f_f_a,
  sc_iterator3_a_f_f,
  sc_iterator3_f_f_f,
  sc_iterator3_count

} sc_iterator3_type;

/*! Iterator parameter
 */
struct _sc_iterator_param
{
  sc_bool is_type;

  union
  {
    sc_addr addr;
    sc_type type;
  };
};

/*! Iterator result
 */
struct _sc_iterator_result
{
  sc_addr addr;
  sc_bool is_accessed;
};

#define SC_ITERATOR_RESULT_EMPTY \
  (sc_iterator_result) \
  { \
    SC_ADDR_EMPTY, SC_TRUE \
  }

/*! Structure to store iterator information
 */
struct _sc_iterator3
{
  sc_iterator3_type type;         // iterator type (search template)
  sc_iterator_param params[3];    // parameters array
  sc_iterator_result results[3];  // results array (same size as params)
  sc_memory_context const * ctx;  // pointer to used memory context
  sc_bool finished;
};

/*! Create iterator to find outgoing sc-arcs for specified element
 * @param el sc-addr of element to iterate outgoing sc-arcs
 * @param arc_type Type of outgoing sc-arc to iterate (0 - all types)
 * @param end_type Type of end element for outgoing sc-arcs, to iterate
 * @return If iterator created, then return pointer to it; otherwise return null
 */
_SC_EXTERN sc_iterator3 * sc_iterator3_f_a_a_new(
    sc_memory_context const * ctx,
    sc_addr el,
    sc_type arc_type,
    sc_type end_type);

/*! Create iterator to find incoming sc-arcs for specified element
 * @param beg_type Type of begin element for incoming sc-arcs, to iterate
 * @param arc_type Type of incoming sc-arc to iterate (0 - all types)
 * @param el sc-addr of element to iterate incoming sc-arcs
 * @return If iterator created, then return pointer to it; otherwise return null
 */
_SC_EXTERN sc_iterator3 * sc_iterator3_a_a_f_new(
    sc_memory_context const * ctx,
    sc_type beg_type,
    sc_type arc_type,
    sc_addr el);

/*! Create iterator to find arcs between two specified elements
 * @param el_beg sc-addr of begin element
 * @param arc_type Type of arcs to iterate (0 - all types)
 * @param el_end sc-addr of end element
 * @return If iterator created, then return pointer to it; otherwise return null
 */
_SC_EXTERN sc_iterator3 * sc_iterator3_f_a_f_new(
    sc_memory_context const * ctx,
    sc_addr el_beg,
    sc_type arc_type,
    sc_addr el_end);

/*! Create iterator to determine edge source and target
 */
_SC_EXTERN sc_iterator3 * sc_iterator3_a_f_a_new(
    sc_memory_context const * ctx,
    sc_type beg_type,
    sc_addr arc_addr,
    sc_type end_type);

// Required for clean template search algorithm
_SC_EXTERN sc_iterator3 * sc_iterator3_f_f_a_new(
    sc_memory_context const * ctx,
    sc_addr beg_addr,
    sc_addr edge_addr,
    sc_type end_type);
_SC_EXTERN sc_iterator3 * sc_iterator3_a_f_f_new(
    sc_memory_context const * ctx,
    sc_type beg_type,
    sc_addr edge_addr,
    sc_addr end_addr);
_SC_EXTERN sc_iterator3 * sc_iterator3_f_f_f_new(
    sc_memory_context const * ctx,
    sc_addr beg_addr,
    sc_addr edge_addr,
    sc_addr end_addr);

/*! Create new sc-iterator-3
 * @param type Iterator type (search template)
 * @param p1 First iterator parameter
 * @param p2 Second iterator parameter
 * @param p3 Third iterator parameter
 * @return Pointer to created iterator. If parameters invalid for specified iterator type, or type is not a
 * sc-iterator-3, then return 0
 */
_SC_EXTERN sc_iterator3 * sc_iterator3_new(
    sc_memory_context const * ctx,
    sc_iterator3_type type,
    sc_iterator_param p1,
    sc_iterator_param p2,
    sc_iterator_param p3);

/*! Destroy iterator and free allocated memory
 * @param it Pointer to sc-iterator that need to be destroyed
 */
_SC_EXTERN void sc_iterator3_free(sc_iterator3 * it);

/*! Go to next iterator result
 * @param it Pointer to iterator that we need to go next result
 * @return Return SC_TRUE, if iterator moved to new results; otherwise return SC_FALSE.
 * @code
 * while(sc_iterator_next(it)) { <your code> }
 * @endcode
 */
_SC_EXTERN sc_bool sc_iterator3_next(sc_iterator3 * it);

/*! Go to next iterator result
 * @param it Pointer to iterator that we need to go next result
 * @param result Pointer to error caused during search
 * @return Return SC_TRUE, if iterator moved to new results; otherwise return SC_FALSE.
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context has not read
 * permissions.
 * @code
 * sc_result result;
 * while(sc_iterator_next(it, &result)) { <your code> }
 * @endcode
 */
_SC_EXTERN sc_bool sc_iterator3_next_ext(sc_iterator3 * it, sc_result * result);

/*! Get iterator value
 * @param it Pointer to iterator for getting value
 * @param index Value id (can't be more that 3 for sc-iterator3)
 * @return Return sc-addr of search result value
 */
_SC_EXTERN sc_addr sc_iterator3_value(sc_iterator3 * it, sc_uint index);

/*! Get iterator value
 * @param it Pointer to iterator for getting value
 * @param index Value id (can't be more that 3 for sc-iterator3)
 * @param result Pointer to a variable that will store the result of the operation.
 *               It can be NULL if the result is not needed.
 * @return Return sc-addr of search result value.
 *
 * Possible values for the `result` parameter:
 * @retval SC_RESULT_OK The function executed successfully.
 * @retval SC_RESULT_NO The specified sc-iterator3 is not valid.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_IS_NOT_AUTHORIZED The specified sc-memory context is not authorized.
 * @retval SC_RESULT_ERROR_SC_MEMORY_CONTEXT_HAS_NO_READ_PERMISSIONS The specified sc-memory context has not read
 * permissions to get sc-element by specified index.
 */
_SC_EXTERN sc_addr sc_iterator3_value_ext(sc_iterator3 * it, sc_uint index, sc_result * result);

/*! Check if specified element type passed into
 * iterator selection.
 * @param el_type Compared element type
 * @param it_type Template type from iterator parameter
 * @return If el_type passed checking, then return SC_TRUE, else return SC_FALSE
 */
_SC_EXTERN sc_bool sc_iterator_compare_type(sc_type el_type, sc_type it_type);

#endif
