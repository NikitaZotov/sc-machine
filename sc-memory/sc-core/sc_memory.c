/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.h"
#include "sc_memory_params.h"

#include "sc-store/sc_types.h"
#include "sc-store/sc-base/sc_allocator.h"

#include "sc-store/sc_storage.h"
#include "sc_memory_private.h"
#include "sc_memory_context_manager.h"
#include "sc_helper.h"
#include "sc_helper_private.h"
#include "sc_memory_ext.h"

typedef struct _sc_memory
{
  sc_memory_context_manager * context_manager;
  sc_addr add_action_addr;
  sc_addr search_action_addr;
  sc_addr remove_action_addr;
} sc_memory;

sc_memory * memory = null_ptr;
sc_memory_context * s_memory_default_ctx = null_ptr;

sc_memory_context * sc_memory_initialize(sc_memory_params const * params)
{
  sc_memory_info("Initialize");

  sc_char * string = sc_version_string_new(&params->version);
  sc_memory_info("Version: %s", string);
  sc_version_string_free(string);

  sc_memory_info("Logger:");
  sc_message("\tLog type: %s", params->log_type);
  sc_message("\tLog file: %s", params->log_file);
  sc_message("\tLog level: %s", params->log_level);

  if (sc_storage_initialize(params) == SC_FALSE)
  {
    sc_memory_error("Error while initialize sc-storage");
    goto error;
  }

  memory = sc_mem_new(sc_memory, 1);

  sc_storage_start_new_process();

  _sc_memory_context_manager_initialize(&memory->context_manager);

  if (sc_helper_init(s_memory_default_ctx) != SC_RESULT_OK)
  {
    sc_memory_error("Error while initialize sc-helper");
    goto error;
  }

  _sc_memory_context_manager_load_actors_and_actions(memory->context_manager);

  sc_helper_resolve_system_identifier(s_memory_default_ctx, "add_action", &memory->add_action_addr);
  sc_helper_resolve_system_identifier(s_memory_default_ctx, "search_action", &memory->search_action_addr);
  sc_helper_resolve_system_identifier(s_memory_default_ctx, "remove_action", &memory->remove_action_addr);

  sc_memory_info("Build configuration:");
  sc_message("\tResult structure upload: %s", params->init_memory_generated_upload ? "On" : "Off");
  sc_message("\tInit memory generated structure: %s", params->init_memory_generated_structure);
  sc_message("\tExtensions path: %s", params->ext_path);

  sc_addr init_memory_generated_structure = SC_ADDR_EMPTY;
  if (params->init_memory_generated_upload)
    sc_helper_resolve_system_identifier(
        s_memory_default_ctx, params->init_memory_generated_structure, &init_memory_generated_structure);

  if (sc_memory_init_ext(params->ext_path, params->enabled_exts, init_memory_generated_structure) != SC_RESULT_OK)
  {
    sc_memory_error("Error while initialize extensions");
    goto error;
  }

  sc_storage_end_new_process();
  sc_memory_info("Successfully initialized");
  return s_memory_default_ctx;

error:
  sc_storage_end_new_process();
  sc_memory_info("Initialized with errors");
  return null_ptr;
}

sc_result sc_memory_init_ext(
    sc_char const * ext_path,
    sc_char const ** enabled_list,
    sc_addr const init_memory_generated_structure)
{
  sc_memory_info("Initialize extensions");

  sc_result const ext_res = sc_ext_initialize(ext_path, enabled_list, init_memory_generated_structure);

  switch (ext_res)
  {
  case SC_RESULT_OK:
    sc_memory_info("Extensions initialized");
    break;

  case SC_RESULT_ERROR_INVALID_PARAMS:
    sc_memory_warning("Extensions directory `%s` doesn't exist", ext_path);
    break;

  default:
    sc_memory_warning("Unknown error while extensions initializing");
    break;
  }

  return ext_res;
}

void sc_memory_shutdown(sc_bool save_state)
{
  sc_memory_info("Shutdown");

  sc_memory_shutdown_ext();
  sc_helper_shutdown();

  sc_storage_shutdown(save_state);

  _sc_memory_context_manager_shutdown(memory->context_manager);
  memory->context_manager = null_ptr;

  sc_mem_free(memory);
  memory = null_ptr;

  sc_memory_info("Shutdown");
}

void sc_memory_shutdown_ext()
{
  sc_ext_shutdown();
}

sc_memory_context * sc_memory_context_new(sc_access_levels levels)
{
  return _sc_memory_context_new_impl(memory->context_manager, levels, SC_ADDR_EMPTY);
}

sc_memory_context * sc_memory_context_new_ext(sc_addr actor_addr)
{
  return _sc_memory_context_new_impl(memory->context_manager, 0, actor_addr);
}

void sc_memory_context_free(sc_memory_context * ctx)
{
  _sc_memory_context_free_impl(memory->context_manager, ctx);
}

void sc_memory_context_pending_begin(sc_memory_context * ctx)
{
  _sc_memory_context_pending_begin(ctx);
}

void sc_memory_context_pending_end(sc_memory_context * ctx)
{
  _sc_memory_context_pending_end(ctx);
}

sc_bool sc_memory_is_initialized()
{
  return sc_storage_is_initialized();
}

sc_bool sc_memory_is_element(sc_memory_context const * ctx, sc_addr addr)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_FALSE;

  return sc_storage_is_element(ctx, addr);
}

sc_uint32 sc_memory_get_element_output_arcs_count(sc_memory_context const * ctx, sc_addr addr)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return 0;

  return sc_storage_get_element_output_arcs_count(ctx, addr);
}

sc_uint32 sc_memory_get_element_input_arcs_count(sc_memory_context const * ctx, sc_addr addr)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return 0;

  return sc_storage_get_element_input_arcs_count(ctx, addr);
}

sc_result sc_memory_element_free(sc_memory_context * ctx, sc_addr addr)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->remove_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_element_free(ctx, addr);
}

sc_addr sc_memory_node_new(sc_memory_context const * ctx, sc_type type)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_ADDR_EMPTY;

  return sc_storage_node_new(ctx, type);
}

sc_addr sc_memory_link_new(sc_memory_context const * ctx)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_ADDR_EMPTY;

  return sc_memory_link_new2(ctx, sc_type_const);
}

sc_addr sc_memory_link_new2(sc_memory_context const * ctx, sc_type type)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_ADDR_EMPTY;

  return sc_storage_link_new(ctx, type);
}

sc_addr sc_memory_arc_new(sc_memory_context * ctx, sc_type type, sc_addr beg, sc_addr end)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_ADDR_EMPTY;

  return sc_storage_arc_new(ctx, type, beg, end);
}

sc_result sc_memory_get_element_type(sc_memory_context const * ctx, sc_addr addr, sc_type * result)
{
  *result = 0;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_get_element_type(ctx, addr, result);
}

sc_result sc_memory_change_element_subtype(sc_memory_context const * ctx, sc_addr addr, sc_type type)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_change_element_subtype(ctx, addr, type);
}

sc_result sc_memory_get_arc_begin(sc_memory_context const * ctx, sc_addr addr, sc_addr * result)
{
  *result = SC_ADDR_EMPTY;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_get_arc_begin(ctx, addr, result);
}

sc_result sc_memory_get_arc_end(sc_memory_context const * ctx, sc_addr addr, sc_addr * result)
{
  *result = SC_ADDR_EMPTY;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_get_arc_end(ctx, addr, result);
}

sc_result sc_memory_get_arc_info(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_addr * result_start_addr,
    sc_addr * result_end_addr)
{
  *result_start_addr = SC_ADDR_EMPTY;
  *result_end_addr = SC_ADDR_EMPTY;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_get_arc_info(ctx, addr, result_start_addr, result_end_addr);
}

sc_result sc_memory_set_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream const * stream)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->remove_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_memory_set_link_content_ext(ctx, addr, stream, SC_TRUE);
}

sc_result sc_memory_set_link_content_ext(
    sc_memory_context const * ctx,
    sc_addr addr,
    sc_stream const * stream,
    sc_bool is_searchable_string)
{
  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->remove_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->add_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_set_link_content_ext(ctx, addr, stream, is_searchable_string);
}

sc_result sc_memory_get_link_content(sc_memory_context const * ctx, sc_addr addr, sc_stream ** stream)
{
  *stream = null_ptr;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_get_link_content(ctx, addr, stream);
}

sc_result sc_memory_find_links_with_content_string(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result)
{
  *result = null_ptr;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_find_links_with_content_string(ctx, stream, result);
}

sc_result sc_memory_find_links_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result,
    sc_uint32 max_length_to_search_as_prefix)
{
  *result = null_ptr;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_find_links_by_content_substring(ctx, stream, result, max_length_to_search_as_prefix);
}

sc_result sc_memory_find_links_contents_by_content_substring(
    sc_memory_context const * ctx,
    sc_stream const * stream,
    sc_list ** result,
    sc_uint32 max_length_to_search_as_prefix)
{
  *result = null_ptr;

  if (_sc_memory_context_check_rights(memory->context_manager, ctx, memory->search_action_addr) == SC_FALSE)
    return SC_RESULT_NO_RIGHTS;

  return sc_storage_find_links_contents_by_content_substring(ctx, stream, result, max_length_to_search_as_prefix);
}

sc_result sc_memory_stat(sc_memory_context const * ctx, sc_stat * stat)
{
  return sc_storage_get_elements_stat(stat);
}

sc_result sc_memory_save(sc_memory_context const * ctx)
{
  return sc_storage_save(ctx);
}
