/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_dictionary.h"

#include <ctype.h>
#include <stdio.h>

#include "sc_dictionary_private.h"

#include "../../sc-base/sc_allocator.h"
#include "../../sc-container/sc-string/sc_string.h"

static const sc_uint8 s_min_sc_char = 1;
static const sc_uint8 s_max_sc_char = 255;

#define SC_DICTIONARY_NODE_IS_VALID(__node) ((__node) != null_ptr)
#define SC_DICTIONARY_NODE_IS_NOT_VALID(__node) ((__node) == null_ptr)

sc_bool sc_dictionary_initialize(sc_dictionary ** dictionary)
{
  if (*dictionary != null_ptr)
    return SC_FALSE;

  *dictionary = sc_mem_new(sc_dictionary, 1);
  (*dictionary)->root = _sc_dictionary_node_initialize();

  return SC_TRUE;
}

sc_bool sc_dictionary_destroy(sc_dictionary * dictionary, void (*node_destroy)(sc_dictionary_node *, void **))
{
  if (dictionary == null_ptr)
    return SC_FALSE;

  sc_dictionary_visit_up_nodes(dictionary, node_destroy, null_ptr);
  node_destroy(dictionary->root, null_ptr);
  sc_mem_free(dictionary);

  return SC_TRUE;
}

inline sc_uint8 _sc_dictionary_children_size()
{
  return s_max_sc_char - s_min_sc_char + 1;
}

inline sc_dictionary_node * _sc_dictionary_node_initialize()
{
  sc_dictionary_node * node = sc_mem_new(sc_dictionary_node, 1);
  node->next = sc_mem_new(sc_dictionary_node *, _sc_dictionary_children_size());

  node->data_list = null_ptr;
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_dc_node_access_lvl_make_read(node);

  return node;
}

void sc_dictionary_node_destroy(sc_dictionary_node * node, void ** args)
{
  (void)args;

  sc_list_clear(node->data_list);
  sc_list_destroy(node->data_list);
  node->data_list = null_ptr;

  sc_mem_free(node->next);
  node->next = null_ptr;

  sc_mem_free(node->offset);
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_mem_free(node);
}

inline sc_dictionary_node * _sc_dictionary_get_next_node(sc_dictionary_node * node, sc_char ch)
{
  sc_uint8 num;
  sc_char_to_sc_int(ch, &num, &node->mask);

  return num == 0 || node->next == null_ptr ? null_ptr : node->next[num];
}

sc_dictionary_node * sc_dictionary_append_to_node(sc_dictionary_node * node, sc_char * sc_string, sc_uint32 size)
{
  sc_char ** sc_string_ptr = sc_mem_new(sc_char *, 1);
  *sc_string_ptr = sc_string;

  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    sc_char_to_sc_int(**sc_string_ptr, &num, &node->mask);
    // define prefix
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[num]))
    {
      node->next[num] = _sc_dictionary_node_initialize();

      sc_dictionary_node * temp = node->next[num];

      temp->offset_size = size - i;
      sc_str_cpy(temp->offset, *sc_string_ptr, temp->offset_size);

      node = temp;

      break;
    }
    // visit next substring
    else if (node->next[num]->offset != null_ptr)
    {
      sc_dictionary_node * moving = node->next[num];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && tolower(moving->offset[j]) == tolower(**sc_string_ptr);
           ++i, ++j, ++*sc_string_ptr)
        ;

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[num] = _sc_dictionary_node_initialize();

        sc_dictionary_node * temp = node->next[num];

        temp->offset_size = j;
        temp->offset = moving->offset;
      }

      node = node->next[num];

      // insert intermediate node for prefix end branching
      if (j < moving->offset_size)
      {
        sc_char * offset_ptr = &*(moving->offset + j);

        sc_char_to_sc_int(*offset_ptr, &num, &node->mask);
        node->next[num] = &*moving;

        sc_dictionary_node * temp = node->next[num];

        temp->offset_size = saved_offset_size - j;
        sc_str_cpy(temp->offset, offset_ptr, temp->offset_size);
      }
    }
    else
    {
      node = node->next[num];
      ++*sc_string_ptr;
      ++i;
    }
  }

  sc_mem_free(sc_string_ptr);
  return node;
}

sc_dictionary_node * sc_dictionary_append(sc_dictionary * dictionary, sc_char * sc_string, sc_uint32 size, void * value)
{
  sc_dictionary_node * node = sc_dictionary_append_to_node(dictionary->root, sc_string, size);

  if (node->data_list == null_ptr)
    sc_list_init(&node->data_list);

  void * copy = sc_mem_new(void, sizeof(value));
  *(void **)copy = *(void **)value;
  sc_list_push_back(node->data_list, copy);
  return node;
}

sc_dictionary_node * sc_dictionary_remove_from_node(
    sc_dictionary_node * node,
    sc_uint8 node_index,
    sc_dictionary_node * parent,
    const sc_char * sc_string,
    sc_uint32 index,
    sc_uint8 * removable_index)
{
  // check prefixes matching
  sc_uint32 string_size = strlen(sc_string);
  sc_uint8 num = 0;
  sc_char_to_sc_int(sc_string[index], &num, &node->mask);
  if (num != 0 && SC_DICTIONARY_NODE_IS_VALID(node->next[num]))
  {
    sc_dictionary_node * removable = sc_dictionary_remove_from_node(
        node->next[num], num, node, sc_string, index + node->next[num]->offset_size, removable_index);

    if (SC_DICTIONARY_NODE_IS_VALID(node->next[num]))
      return removable;
  }

  // check suffixes matching
  if (index == string_size)
  {
    sc_char * str = sc_mem_new(sc_char, node->offset_size + 1);
    sc_mem_cpy(str, node->offset, node->offset_size);

    if (strcmp(str, sc_string + (string_size - node->offset_size)) == 0)
    {
      sc_mem_free(str);

      *removable_index = node_index;
      return parent;
    }
    sc_mem_free(str);
  }

  return null_ptr;
}

sc_bool sc_dictionary_remove(sc_dictionary * dictionary, const sc_char * sc_string)
{
  sc_uint8 index = 0;
  sc_dictionary_node * node = sc_dictionary_remove_from_node(dictionary->root, 0, null_ptr, sc_string, 0, &index);

  sc_bool result = node->next[index] != null_ptr;
  if (index != 0 && result == SC_TRUE)
  {
    sc_dictionary_node_destroy(node->next[index], null_ptr);
    node->next[index] = null_ptr;
  }

  return result;
}

sc_dictionary_node * sc_dictionary_get_node_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  // check prefixes matching
  sc_uint32 i = 0;
  sc_uint32 string_size = strlen(sc_string);
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(node, sc_string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  if (i <= string_size)
  {
    sc_char * str = sc_mem_new(sc_char, node->offset_size + 1);
    sc_mem_cpy(str, node->offset, node->offset_size);

    if (strstr(str, sc_string + (string_size - node->offset_size)) != null_ptr)
    {
      sc_mem_free(str);
      return node;
    }

    sc_mem_free(str);
    return null_ptr;
  }

  return node;
}

sc_dictionary_node * sc_dictionary_get_last_node_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  if (sc_string == null_ptr)
    return null_ptr;

  // check prefixes matching
  sc_uint32 i = 0;
  sc_uint32 string_size = strlen(sc_string);
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(node, sc_string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  // check suffixes matching
  if (i == string_size)
  {
    sc_char * str = sc_mem_new(sc_char, node->offset_size + 1);
    sc_mem_cpy(str, node->offset, node->offset_size);

    if (strcmp(str, sc_string + (string_size - node->offset_size)) == 0)
    {
      sc_mem_free(str);
      return node;
    }
    sc_mem_free(str);
  }

  return null_ptr;
}

sc_bool sc_dictionary_is_in_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(node, sc_string);

  return SC_DICTIONARY_NODE_IS_VALID(last) && last->data_list != null_ptr;
}

sc_bool sc_dictionary_is_in(sc_dictionary * dictionary, const sc_char * sc_string)
{
  return sc_dictionary_is_in_from_node(dictionary->root, sc_string);
}

void * sc_dictionary_get_first_data_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(node, sc_string);

  if (SC_DICTIONARY_NODE_IS_VALID(last) && last->data_list != null_ptr && last->data_list->begin != null_ptr)
    return last->data_list->begin->data;

  return null_ptr;
}

sc_list * sc_dictionary_get_datas_from_node(sc_dictionary_node * node, const sc_char * sc_string)
{
  sc_dictionary_node * last = sc_dictionary_get_last_node_from_node(node, sc_string);

  if (SC_DICTIONARY_NODE_IS_VALID(last))
    return last->data_list;

  return null_ptr;
}

sc_list * sc_dictionary_get(sc_dictionary * dictionary, const sc_char * sc_string)
{
  return sc_dictionary_get_datas_from_node(dictionary->root, sc_string);
}

void _sc_dictionary_update_list(sc_dictionary_node * node, void ** args)
{
  if (node->data_list != null_ptr)
  {
    sc_list * full_list = args[0];

    sc_iterator * it = sc_list_iterator(node->data_list);
    while (sc_iterator_next(it))
      sc_list_push_back(full_list, sc_iterator_get(it));

    sc_iterator_destroy(it);
  }
}

sc_list * sc_dictionary_get_by_substr(sc_dictionary * dictionary, const sc_char * sc_substr)
{
  sc_dictionary_node * node = sc_dictionary_get_node_from_node(dictionary->root, sc_substr);

  sc_list * full_list;
  sc_list_init(&full_list);

  if (SC_DICTIONARY_NODE_IS_VALID(node))
  {
    sc_dictionary_visit_down_node_from_node(node, _sc_dictionary_update_list, (void **)&full_list);
    _sc_dictionary_update_list(node, (void **)&full_list);
  }

  return full_list;
}

void sc_dictionary_show_from_node(sc_dictionary_node * node, sc_char * tab)
{
  sc_uint8 i;
  for (i = 1; i < _sc_dictionary_children_size(); ++i)
  {
    sc_dictionary_node * next = node->next[i];

    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      sc_char * str = sc_mem_new(sc_char, next->offset_size + 1);
      sc_mem_cpy(str, next->offset, next->offset_size);
      sc_uchar ch = sc_int_to_sc_char(i, 0);

      if (next->data_list != null_ptr && next->data_list->size != 0)
        printf("%s%c[%d] {%s}\n", tab, ch, next->data_list->size, str);
      else
        printf("%s%c {%s}\n", tab, ch, str);

      sc_mem_free(str);

      sc_char * new_tab = sc_mem_new(sc_char, strlen(tab) + 6);
      strcpy(new_tab, tab);
      strcat(new_tab, "|----\0");

      sc_dictionary_show_from_node(next, new_tab);

      sc_mem_free(new_tab);
    }
  }
}

void sc_dictionary_show(sc_dictionary * dictionary)
{
  printf("----------------sc-dictionary----------------\n");
  sc_dictionary_show_from_node(dictionary->root, "\0");
  printf("---------------------------------------------\n");
}

void sc_dictionary_visit_down_node_from_node(
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 0; i < _sc_dictionary_children_size(); ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      callable(next, dest);

      sc_dictionary_visit_down_node_from_node(next, callable, dest);
    }
  }
}

void sc_dictionary_visit_down_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_visit_down_node_from_node(dictionary->root, callable, dest);
}

void sc_dictionary_visit_up_node_from_node(
    sc_dictionary_node * node,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 0; i < _sc_dictionary_children_size(); ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      sc_dictionary_visit_up_node_from_node(next, callable, dest);

      callable(next, dest);
    }
  }
}

void sc_dictionary_visit_up_nodes(
    sc_dictionary * dictionary,
    void (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_visit_up_node_from_node(dictionary->root, callable, dest);
}

inline void sc_char_to_sc_int(sc_char ch, sc_uint8 * ch_num, sc_uint8 * mask)
{
  // @todo: Implement mask setups for rights checking and memory optimization
  *ch_num = 128 + (sc_uint8)ch;
}

inline sc_char sc_int_to_sc_char(sc_uint8 num, sc_uint8 mask)
{
  return (sc_char)(num - 128);
}
