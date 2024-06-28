/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_memory_context_private_h_
#define _sc_memory_context_private_h_

#include "sc-core/sc_types.h"

#include "sc-core/sc-base/sc_monitor.h"

/*! Structure representing a memory context manager.
 * @note This structure manages memory contexts and user authentications in the sc-memory.
 */
struct _sc_memory_context_manager
{
  sc_hash_table * context_hash_table;  ///< Hash table storing memory contexts based on user addresses.
  sc_uint32 context_count;             ///< Number of currently active memory contexts.
  sc_monitor context_monitor;          ///< Monitor for synchronizing access to the hash table storing memory contexts.

  sc_event * on_new_identified_user_subscription;  /// < Subscription for identified user events.

  sc_event * on_authentication_request_user_subscription;  ///< Subscription for user authentication request events.
  sc_event * on_remove_authenticated_user_subscription;    ///< Subscription for user unauthentication events.

  ///< Hash table storing global permissions (within the knowledge base) for users.
  sc_hash_table * user_global_permissions;
  ///< Monitor for synchronizing access to the hash table storing global permissions within the knowledge base.
  sc_monitor user_global_permissions_monitor;
  sc_hash_table * basic_action_classes;  ///< Hash table storing permissions for action classes in sc-memory.
  sc_event * on_new_user_action_class;   ///< Event subscription for adding new permitted action classes for users.
  sc_event * on_new_users_set_action_class;
  sc_event * on_remove_user_action_class;  ///< Event subscription for removing new permitted action classes for users.
  sc_event * on_remove_users_set_action_class;

  ///< Hash table storing local permissions (within sc-structures) for users.
  sc_hash_table * user_local_permissions;
  ///< Monitor for synchronizing access to the hash table storing local permissions within sc-structures.
  sc_monitor user_local_permissions_monitor;
  sc_event * on_new_user_action_class_within_sc_structure;
  sc_event * on_new_users_set_action_class_within_sc_structure;
  sc_event * on_remove_user_action_class_within_sc_structure;
  sc_event * on_remove_users_set_action_class_within_sc_structure;

  ///< sc-address representing class for authentication request users.
  sc_addr concept_guest_user_addr;
  ///< sc-address representing relation between quests and theirs identified users.
  sc_addr nrel_identified_user_addr;
  ///< sc-address representing class for authentication request users.
  sc_addr concept_authentication_request_user_addr;
  ///< sc-address representing class for authenticated users for authenticated users.
  sc_addr concept_authenticated_user_addr;
  ///< sc-address representing relation between users and theirs permitted actions on the knowledge base.
  sc_addr nrel_user_action_class_addr;
  sc_addr nrel_users_set_action_class_addr;
  ///< sc-address representing relation between users and theirs permitted actions on sc-structures.
  sc_addr nrel_user_action_class_within_sc_structure_addr;
  sc_addr nrel_users_set_action_class_within_sc_structure_addr;

  sc_bool user_mode;  ///< Boolean indicating whether the system is in user mode (SC_TRUE) or not (SC_FALSE).
};

/*! Structure representing a memory context.
 * @note This structure represents a memory context associated with a specific user in the sc-memory.
 */
struct _sc_memory_context
{
  sc_addr user_addr;                  ///< sc-address representing the user associated with the sc-memory context.
  sc_uint32 ref_count;                ///< Reference count to manage the number of references to the sc-memory context.
  sc_permissions global_permissions;  ///< Global permissions within the knowledge base.
  sc_hash_table * local_permissions;  ///< Local permissions within sc-structures.
  sc_uint8 flags;                     ///< Flags indicating the state of the sc-memory context.
  sc_hash_table_list * pend_events;   ///< List of pending events to be emitted in the sc-memory context.
  sc_monitor monitor;                 ///< Monitor for synchronizing access to the sc-memory context.
};

/*!
 * @brief Retrieves the sc-memory context manager.
 *
 * @returns Returns a pointer to the sc-memory context manager.
 *
 * @note This function returns the sc-memory context manager, which manages memory contexts and user authorizations.
 */
_SC_EXTERN void * sc_memory_get_context_manager();

#endif
