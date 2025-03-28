/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <array>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "sc-memory/sc_defines.hpp"

template <typename T>
inline std::string DebugPrint(T const & t);

_SC_EXTERN std::string DebugPrint(std::string const & t);
inline std::string DebugPrint(char const * t);
inline std::string DebugPrint(char t);

template <typename U, typename V>
inline std::string DebugPrint(std::pair<U, V> const & p);
template <typename T>
inline std::string DebugPrint(std::list<T> const & v);
template <typename T>
inline std::string DebugPrint(std::vector<T> const & v);
template <typename T, typename C = std::less<T>>
inline std::string DebugPrint(std::set<T, C> const & v);
template <typename T, typename C = std::less<T>>
inline std::string DebugPrint(std::multiset<T, C> const & v);
template <typename U, typename V, typename C = std::less<U>>
inline std::string DebugPrint(std::map<U, V, C> const & v);
template <typename T>
inline std::string DebugPrint(std::initializer_list<T> const & v);

template <class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
inline std::string DebugPrint(std::unordered_set<Key, Hash, Pred> const & v);
template <class Key, class T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>>
inline std::string DebugPrint(std::unordered_map<Key, T, Hash, Pred> const & v);

inline std::string DebugPrint(char const * t)
{
  if (t)
  {
    return DebugPrint(std::string(t));
  }
  else
  {
    return std::string("NULL string pointer");
  }
}

inline std::string DebugPrint(char t)
{
  return DebugPrint(std::string(1, t));
}

inline std::string DebugPrint(signed char t)
{
  return DebugPrint(static_cast<int>(t));
}

inline std::string DebugPrint(unsigned char t)
{
  return DebugPrint(static_cast<unsigned int>(t));
}

template <typename U, typename V>
inline std::string DebugPrint(std::pair<U, V> const & p)
{
  std::ostringstream out;
  out << "(" << DebugPrint(p.first) << ", " << DebugPrint(p.second) << ")";
  return out.str();
}

namespace utils
{
namespace impl
{
template <typename IterT>
inline std::string DebugPrintSequence(IterT beg, IterT end)
{
  std::ostringstream out;
  out << "[" << std::distance(beg, end) << ":";
  for (; beg != end; ++beg)
    out << " " << DebugPrint(*beg);
  out << " ]";
  return out.str();
}

}  // namespace impl
}  // namespace utils

template <typename T, size_t N>
inline std::string DebugPrint(T (&arr)[N])
{
  return ::utils::impl::DebugPrintSequence(arr, arr + N);
}

template <typename T, size_t N>
inline std::string DebugPrint(std::array<T, N> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename T>
inline std::string DebugPrint(std::vector<T> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename T>
inline std::string DebugPrint(std::list<T> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename T, typename C>
inline std::string DebugPrint(std::set<T, C> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename T, typename C>
inline std::string DebugPrint(std::multiset<T, C> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename U, typename V, typename C>
inline std::string DebugPrint(std::map<U, V, C> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename T>
inline std::string DebugPrint(std::initializer_list<T> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <class Key, class Hash, class Pred>
inline std::string DebugPrint(std::unordered_set<Key, Hash, Pred> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <class Key, class T, class Hash, class Pred>
inline std::string DebugPrint(std::unordered_map<Key, T, Hash, Pred> const & v)
{
  return ::utils::impl::DebugPrintSequence(v.begin(), v.end());
}

template <typename T>
inline std::string DebugPrint(T const & t)
{
  std::ostringstream out;
  out << t;
  return out.str();
}

namespace utils
{
namespace impl
{
inline std::string Message()
{
  return std::string();
}

template <typename T>
std::string Message(T const & t)
{
  return DebugPrint(t);
}

template <typename T, typename... ARGS>
std::string Message(T const & t, ARGS const &... others)
{
  return DebugPrint(t) + Message(others...);
}
}  // namespace impl
}  // namespace utils
