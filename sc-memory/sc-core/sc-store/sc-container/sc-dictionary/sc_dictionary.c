/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include <ctype.h>

#include "sc_dictionary.h"
#include "sc_dictionary_private.h"

#include "../../sc-base/sc_allocator.h"
#include "../../sc-container/sc-string/sc_string.h"

#define SC_DICTIONARY_NODE_IS_VALID(__node) ((__node) != null_ptr)
#define SC_DICTIONARY_NODE_IS_NOT_VALID(__node) ((__node) == null_ptr)

sc_bool sc_dictionary_initialize(
    sc_dictionary ** dictionary,
    sc_uint8 children_size,
    void (*char_to_int)(sc_char, sc_uint8 *, const sc_uint8 *))
{
  *dictionary = sc_mem_new(sc_dictionary, 1);
  (*dictionary)->size = children_size;
  (*dictionary)->root = _sc_dictionary_node_initialize(children_size);
  (*dictionary)->char_to_int = char_to_int;

  return SC_TRUE;
}

sc_bool sc_dictionary_destroy(sc_dictionary * dictionary, sc_bool (*node_destroy)(sc_dictionary_node *, void **))
{
  if (dictionary == null_ptr)
    return SC_FALSE;

  sc_dictionary_visit_up_nodes(dictionary, node_destroy, null_ptr);
  node_destroy(dictionary->root, null_ptr);
  sc_mem_free(dictionary);

  return SC_TRUE;
}

inline sc_dictionary_node * _sc_dictionary_node_initialize(sc_uint8 children_size)
{
  sc_dictionary_node * node = sc_mem_new(sc_dictionary_node, 1);
  node->next = sc_mem_new(sc_dictionary_node *, children_size);

  node->data = null_ptr;
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_dc_node_access_lvl_make_read(node);

  return node;
}

sc_bool sc_dictionary_node_destroy(sc_dictionary_node * node, void ** args)
{
  (void)args;

  node->data = null_ptr;

  sc_mem_free(node->next);
  node->next = null_ptr;

  sc_mem_free(node->offset);
  node->offset = null_ptr;
  node->offset_size = 0;

  sc_mem_free(node);

  return SC_TRUE;
}

inline sc_dictionary_node * _sc_dictionary_get_next_node(
    const sc_dictionary * dictionary,
    const sc_dictionary_node * node,
    const sc_char ch)
{
  sc_uint8 num;
  dictionary->char_to_int(ch, &num, &node->mask);

  return node->next == null_ptr ? null_ptr : node->next[num];
}

sc_dictionary_node * sc_dictionary_append_to_node(sc_dictionary * dictionary, const sc_char * string, sc_uint32 size)
{
  sc_dictionary_node * node = dictionary->root;

  sc_char ** string_ptr = sc_mem_new(sc_char *, 1);
  *string_ptr = (sc_char *)string;

  sc_uint32 i = 0;
  sc_uint32 saved_offset_size = size;
  while (i < size)
  {
    sc_uint8 num;
    dictionary->char_to_int(**string_ptr, &num, &node->mask);
    // define prefix
    if (SC_DICTIONARY_NODE_IS_NOT_VALID(node->next[num]))
    {
      node->next[num] = _sc_dictionary_node_initialize(dictionary->size);

      sc_dictionary_node * temp = node->next[num];

      temp->offset_size = size - i;
      sc_str_cpy(temp->offset, *string_ptr, temp->offset_size);

      node = temp;

      break;
    }
    // visit next substring
    else if (node->next[num]->offset != null_ptr)
    {
      sc_dictionary_node * moving = node->next[num];

      sc_uint32 j = 0;
      for (; i < size && j < moving->offset_size && tolower(moving->offset[j]) == tolower(**string_ptr);
           ++i, ++j, ++*string_ptr)
        ;

      if (j != moving->offset_size)
      {
        saved_offset_size = moving->offset_size;

        node->next[num] = _sc_dictionary_node_initialize(dictionary->size);

        sc_dictionary_node * temp = node->next[num];

        temp->offset_size = j;
        sc_str_cpy(temp->offset, moving->offset, temp->offset_size);
      }

      node = node->next[num];

      // insert intermediate node for prefix end branching
      if (j < moving->offset_size)
      {
        sc_char * offset_ptr = &*(moving->offset + j);

        dictionary->char_to_int(*offset_ptr, &num, &node->mask);
        sc_char * moving_offset_copy = moving->offset;
        node->next[num] = &*moving;

        sc_dictionary_node * temp = node->next[num];

        temp->offset_size = saved_offset_size - j;
        sc_str_cpy(temp->offset, offset_ptr, temp->offset_size);
        sc_mem_free(moving_offset_copy);
      }
    }
    else
    {
      node = node->next[num];
      ++*string_ptr;
      ++i;
    }
  }

  sc_mem_free(string_ptr);
  return node;
}

sc_dictionary_node * sc_dictionary_append(
    sc_dictionary * dictionary,
    const sc_char * string,
    sc_uint32 size,
    void * value)
{
  sc_dictionary_node * node = sc_dictionary_append_to_node(dictionary, string, size);
  node->data = value;
  return node;
}

sc_dictionary_node * sc_dictionary_remove_from_node(
    sc_dictionary * dictionary,
    sc_dictionary_node * node,
    const sc_char * string,
    const sc_uint32 string_size,
    sc_uint32 i)
{
  // check prefixes matching
  if (i < string_size)
  {
    sc_uint8 num = 0;
    dictionary->char_to_int(string[i], &num, &node->mask);
    sc_dictionary_node * next = node->next[num];
    if (SC_DICTIONARY_NODE_IS_VALID(node->next[num]) &&
        (sc_str_has_prefix(string + i, next->offset) || sc_str_cmp(string + i, next->offset)))
    {
      sc_dictionary_node * removable =
          sc_dictionary_remove_from_node(dictionary, next, string, string_size, i + next->offset_size);

      if (SC_DICTIONARY_NODE_IS_VALID(next))
        return removable;
    }
  }

  // check suffixes matching
  if (i == string_size &&
      (node->offset == null_ptr || sc_str_cmp(node->offset, string + (string_size - node->offset_size))))
  {
    return node;
  }

  return null_ptr;
}

const sc_dictionary_node * sc_dictionary_get_last_node_from_node(
    const sc_dictionary * dictionary,
    const sc_dictionary_node * node,
    const sc_char * string,
    const sc_uint32 string_size)
{
  // check prefixes matching
  sc_uint32 i = 0;
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(dictionary, node, string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next) &&
        (sc_str_has_prefix(string + i, next->offset) || sc_str_cmp(string + i, next->offset)))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  // check suffixes matching
  if (i == string_size &&
      (node->offset == null_ptr || sc_str_cmp(node->offset, string + (string_size - node->offset_size))))
  {
    return node;
  }

  return null_ptr;
}

sc_bool sc_dictionary_has(const sc_dictionary * dictionary, const sc_char * string, sc_uint32 string_size)
{
  const sc_dictionary_node * last =
      sc_dictionary_get_last_node_from_node(dictionary, dictionary->root, string, string_size);

  return SC_DICTIONARY_NODE_IS_VALID(last);
}

void * sc_dictionary_get_by_key(const sc_dictionary * dictionary, const sc_char * string, const sc_uint32 string_size)
{
  const sc_dictionary_node * last =
      sc_dictionary_get_last_node_from_node(dictionary, dictionary->root, string, string_size);

  if (SC_DICTIONARY_NODE_IS_VALID(last))
    return last->data;

  return null_ptr;
}

sc_bool sc_dictionary_get_by_key_prefix(
    const sc_dictionary * dictionary,
    const sc_char * string,
    const sc_uint32 string_size,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_dictionary_node * node = dictionary->root;

  sc_uint32 i = 0;
  while (i < string_size)
  {
    sc_dictionary_node * next = _sc_dictionary_get_next_node(dictionary, node, string[i]);
    if (SC_DICTIONARY_NODE_IS_VALID(next) &&
        (sc_str_has_prefix(string + i, next->offset) || sc_str_cmp(string + i, next->offset)))
    {
      node = next;
      i += node->offset_size;
    }
    else
      break;
  }

  if (i == string_size)
    callable(node, dest);

  sc_uint8 j;
  for (j = 0; j < dictionary->size; ++j)
  {
    sc_dictionary_node * next = node->next[j];
    if (SC_DICTIONARY_NODE_IS_VALID(next) && (i == string_size || sc_str_has_prefix(next->offset, string + i)))
    {
      if (!callable(next, dest))
        return SC_FALSE;

      if (!sc_dictionary_visit_down_node_from_node(dictionary, next, callable, dest))
        return SC_FALSE;
    }
  }
  return SC_TRUE;
}

sc_bool sc_dictionary_visit_down_node_from_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 0; i < dictionary->size; ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      if (!callable(next, dest))
        return SC_FALSE;

      if (!sc_dictionary_visit_down_node_from_node(dictionary, next, callable, dest))
        return SC_FALSE;
    }
  }

  return SC_TRUE;
}

sc_bool sc_dictionary_visit_down_nodes(
    sc_dictionary const * dictionary,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  return sc_dictionary_visit_down_node_from_node(dictionary, dictionary->root, callable, dest);
}

sc_bool sc_dictionary_visit_up_node_from_node(
    sc_dictionary const * dictionary,
    sc_dictionary_node * node,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  sc_uint8 i;
  for (i = 0; i < dictionary->size; ++i)
  {
    sc_dictionary_node * next = node->next[i];
    if (SC_DICTIONARY_NODE_IS_VALID(next))
    {
      if (!sc_dictionary_visit_up_node_from_node(dictionary, next, callable, dest))
        return SC_FALSE;

      if (!callable(next, dest))
        return SC_FALSE;
    }
  }

  return SC_TRUE;
}

sc_bool sc_dictionary_visit_up_nodes(
    sc_dictionary const * dictionary,
    sc_bool (*callable)(sc_dictionary_node *, void **),
    void ** dest)
{
  return sc_dictionary_visit_up_node_from_node(dictionary, dictionary->root, callable, dest);
}
