/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc-memory/utils/sc_logger.hpp"
#include "sc-memory/utils/sc_message.hpp"
#include "sc-memory/utils/sc_console.hpp"

extern "C"
{
#include <sc-core/sc_types.h>
}

namespace utils
{
/// -----------------------

class ScException : public std::exception
{
public:
  _SC_EXTERN ScException(std::string description, std::string msg);
  _SC_EXTERN ~ScException() noexcept override;

  _SC_EXTERN sc_char const * Description() const noexcept;
  _SC_EXTERN sc_char const * Message() const noexcept;

private:
  std::string m_description;
  std::string m_msg;
};

class ExceptionAssert final : public ScException
{
public:
  ExceptionAssert(std::string const & description, std::string const & msg)
    : ScException("Assert: " + description, msg)
  {
  }
};

class ExceptionCritical final : public ScException
{
public:
  ExceptionCritical(std::string const & description, std::string const & msg)
    : ScException("Critical: " + description, msg)
  {
  }
};

class ExceptionInvalidParams final : public ScException
{
public:
  ExceptionInvalidParams(std::string const & description, std::string const & msg)
    : ScException("InvalidParams: " + description, msg)
  {
  }
};

class ExceptionInvalidState final : public ScException
{
public:
  ExceptionInvalidState(std::string const & description, std::string const & msg)
    : ScException("InvalidState: " + description, msg)
  {
  }
};

class ExceptionItemNotFound final : public ScException
{
public:
  ExceptionItemNotFound(std::string const & description, std::string const & msg)
    : ScException("ItemNotFound: " + description, msg)
  {
  }
};

class ExceptionParseError final : public ScException
{
public:
  ExceptionParseError(std::string const & description, std::string const & msg)
    : ScException("ParseError: " + description, msg)
  {
  }
};

class ExceptionNotImplemented final : public ScException
{
public:
  ExceptionNotImplemented(std::string const & description, std::string const & msg)
    : ScException("NotImplemented: " + description, msg)
  {
  }
};

class ExceptionInvalidType final : public ScException
{
public:
  explicit ExceptionInvalidType(std::string const & description, std::string const & msg)
    : ScException("InvalidType: " + description, msg)
  {
  }
};

// Asserts
#define THROW_EXCEPTION(_exception_class, _msg, _file, _line) \
  ({ \
    std::stringstream _str_message; \
    _str_message << _msg; \
    std::string _msg_raw = _str_message.str(); \
    _str_message << std::endl << "File: " << _file << std::endl << "Line:" << _line << std::endl; \
    throw _exception_class(_str_message.str(), _msg_raw); \
  })

#define SC_THROW_EXCEPTION(_exception_class, _msg) THROW_EXCEPTION(_exception_class, _msg, __FILE__, __LINE__)

#define SC_NOT_IMPLEMENTED(_msg) SC_THROW_EXCEPTION(utils::ExceptionNotImplemented, _msg)

#define _ASSERT_IMPL(_exception_class, _expr, _msg, _file, _line) \
  ({ \
    if (!(_expr)) \
    { \
      std::string _message = ::utils::impl::Message("SC_ASSERT(" #_expr ")", ::utils::impl::Message _msg); \
      THROW_EXCEPTION(_exception_class, _message, _file, _line); \
    } \
  })

#if SC_DEBUG_MODE
#  define SC_ASSERT(_expr, _msg) ({ _ASSERT_IMPL(::utils::ExceptionAssert, _expr, (_msg), __FILE__, __LINE__); })
#else
#  define SC_ASSERT(_expr, _msg) ((void)0)
#endif

#define SC_CHECK_PARAM(_expr, _msg) \
  if (!(_expr).IsValid()) \
  SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, _msg)

#define _CHECK_IMPL(_expr, _msg, _name) \
  ({ \
    if (!_expr) \
    { \
      std::string _message = ::utils::impl::Message(_name "(" #_expr ")", _msg); \
      SC_THROW_EXCEPTION(::utils::ExceptionAssert, _message); \
    } \
  })

#define SC_CHECK(_expr, _msg) _CHECK_IMPL(_expr, _msg, "CHECK")
#define SC_CHECK_NOT(_expr, _msg) _CHECK_IMPL(!_expr, _msg, "CHECK_NOT")
#define SC_CHECK_EQUAL(_a, _b, _msg) \
  ({ \
    if ((_a) != (_b)) \
    { \
      std::string _message = ::utils::impl::Message(std::string("SC_CHECK_EQUAL") + "(" #_a " == " #_b ")", _msg); \
      SC_THROW_EXCEPTION(::utils::ExceptionAssert, _message); \
    } \
  })

#define SC_CHECK_NOT_EQUAL(_a, _b, _msg) \
  ({ \
    if ((_a) == (_b)) \
    { \
      std::string _message = ::utils::impl::Message(std::string("SC_CHECK_NOT_EQUAL") + "(" #_a " != " #_b ")", _msg); \
      SC_THROW_EXCEPTION(::utils::ExceptionAssert, _message); \
    } \
  })

#define SC_CHECK_GREAT(_a, _b, _msg) \
  ({ \
    if ((_a) == (_b)) \
    { \
      std::string _message = ::utils::impl::Message(std::string("SC_CHECK_NOT_EQUAL") + "(" #_a " == " #_b ")", _msg); \
      SC_THROW_EXCEPTION(::utils::ExceptionAssert, _message); \
    } \
  })
}  // namespace utils
