/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _sc_string_tree_h_
#define _sc_string_tree_h_

#include "../sc_types.h"

typedef struct _sc_string_tree_node_data
{
  void *value;
  sc_uint8 value_size;
} sc_string_tree_node_data;

typedef struct _sc_string_tree_node
{
  struct _sc_string_tree_node **next;
  sc_char *offset;
  sc_uint16 offset_size;
  sc_string_tree_node_data *data;
  sc_uint8 mask;
} sc_string_tree_node;

typedef struct _sc_string_tree
{
  sc_string_tree_node *root;
  sc_uint8 size;
} sc_string_tree;

typedef struct _sc_link_content
{
  sc_string_tree_node *node;
  sc_char *sc_string;
  sc_uint32 string_size;
} sc_link_content;

sc_bool sc_string_tree_initialize();

sc_bool _sc_string_tree_links_hashes_initialize();

sc_bool sc_string_tree_shutdown();

sc_uint8 sc_string_tree_children_size();

sc_string_tree_node* _sc_string_tree_node_initialize();

sc_string_tree_node* _sc_string_tree_get_next_node(sc_string_tree_node *node, sc_char ch);

sc_string_tree_node* sc_string_tree_append_to_node(sc_string_tree_node *node, sc_char *sc_string, sc_uint32 size);

sc_string_tree_node* sc_string_tree_append(sc_addr addr, sc_char *sc_string, sc_uint32 size);

sc_string_tree_node* sc_string_tree_remove_from_node(sc_string_tree_node *node, const sc_char *sc_string, sc_uint32 index);

sc_bool sc_string_tree_remove(const sc_char *sc_string);

sc_bool sc_string_tree_is_in_from_node(sc_string_tree_node *node, const sc_char *sc_string);

sc_bool sc_string_tree_is_in(const sc_char *sc_string);

sc_string_tree_node* sc_string_tree_get_last_node_from_node(sc_string_tree_node *node, const sc_char *sc_string);

void* sc_string_tree_get_data_from_node(sc_string_tree_node *node, const sc_char *sc_string);

sc_addr sc_string_tree_get_sc_link(const sc_char *sc_string);

sc_bool sc_string_tree_get_datas_from_node(sc_string_tree_node *node, const sc_char *sc_string, void **data, sc_uint32 *size);

sc_bool sc_string_tree_get_sc_links(const sc_char *sc_string, sc_addr **links, sc_uint32 *size);

sc_char* sc_string_tree_get_sc_string_from_node(sc_string_tree_node *node, sc_addr addr, sc_uint32 *count);

sc_char* sc_string_tree_get_sc_string(sc_addr addr);

sc_string_tree_node* sc_string_tree_get_node(sc_addr addr);

sc_link_content* sc_string_tree_get_content(sc_addr addr);

void sc_string_tree_get_sc_string_ext(sc_addr addr, sc_char **sc_string, sc_uint32 *size);

void sc_string_tree_show_from_node(sc_string_tree_node *node, sc_char *tab);

void sc_string_tree_show();

void sc_string_tree_visit_node_from_node(
      sc_string_tree_node *node,
      void (*callable)(sc_string_tree_node*, void**),
      void **dest);

void sc_string_tree_visit_nodes(void (*callable)(sc_string_tree_node*, void**), void **dest);

void sc_string_tree_links_hashes_show();

void sc_char_to_sc_int(sc_char ch, sc_uint8 *ch_num, sc_uint8 *mask);

sc_char sc_int_to_sc_char(sc_uint8 num, sc_uint8 mask);

sc_uint32 sc_addr_to_hash(sc_addr addr);

sc_char* sc_addr_to_str(sc_addr addr);

sc_char* itora(sc_uint32 num);

#endif
