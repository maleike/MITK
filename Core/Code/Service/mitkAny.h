/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$


Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
Extracted from Boost 1.46.1 and adapted for MITK

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

=========================================================================*/

#ifndef MITK_ANY_H
#define MITK_ANY_H

#include <algorithm>
#include <typeinfo>
#include <sstream>
#include <vector>
#include <list>

#include <MitkExports.h>

namespace mitk {

template<class T>
std::string any_value_to_string(const T& val);

MITK_CORE_EXPORT std::string any_value_to_string(const std::vector<std::string>& val);
MITK_CORE_EXPORT std::string any_value_to_string(const std::list<std::string>& val);

/**
 * An Any class represents a general type and is capable of storing any type, supporting type-safe extraction
 * of the internally stored data.
 *
 * Code taken from the Boost 1.46.1 library. Original copyright by Kevlin Henney. Modified for MITK.
 */
class Any
{
public:

  /**
   * Creates an empty any type.
   */
  Any(): _content(0)
  { }

  /**
   * Creates an Any which stores the init parameter inside.
   *
   * \param value The content of the Any
   *
   * Example:
   * \code
   * Any a(13);
   * Any a(string("12345"));
   * \endcode
   */
  template <typename ValueType>
  Any(const ValueType& value)
    : _content(new Holder<ValueType>(value))
  { }

  /**
   * Copy constructor, works with empty Anys and initialized Any values.
   *
   * \param other The Any to copy
   */
  Any(const Any& other)
    : _content(other._content ? other._content->Clone() : 0)
  { }

  ~Any()
  {
    delete _content;
  }

  /**
   * Swaps the content of the two Anys.
   *
   * \param rhs The Any to swap this Any with.
   */
  Any& Swap(Any& rhs)
  {
    std::swap(_content, rhs._content);
    return *this;
  }

  /**
   * Assignment operator for all types != Any.
   *
   * \param rhs The value which should be assigned to this Any.
   *
   * Example:
   * \code
   * Any a = 13;
   * Any a = string("12345");
   * \endcode
   */
  template <typename ValueType>
  Any& operator = (const ValueType& rhs)
  {
    Any(rhs).Swap(*this);
    return *this;
  }

  /**
   * Assignment operator for Any.
   *
   * \param rhs The Any which should be assigned to this Any.
   */
  Any& operator = (const Any& rhs)
  {
    Any(rhs).Swap(*this);
    return *this;
  }

  /**
   * returns true if the Any is empty
   */
  bool Empty() const
  {
    return !_content;
  }

  /**
   * Returns a string representation for the content.
   *
   * Custom types should either provide a <code>std::ostream& operator<<(std::ostream& os, const CustomType& ct)</code>
   * function or specialize the any_value_to_string template function for meaningful output.
   */
  std::string ToString() const
  {
    return _content->ToString();
  }

  /**
   * Returns the type information of the stored content.
   * If the Any is empty typeid(void) is returned.
   * It is suggested to always query an Any for its type info before trying to extract
   * data via an any_cast/ref_any_cast.
   */
  const std::type_info& Type() const
  {
    return _content ? _content->Type() : typeid(void);
  }

private:

  class Placeholder
  {
  public:
    virtual ~Placeholder()
    { }

    virtual std::string ToString() const = 0;

    virtual const std::type_info& Type() const = 0;
    virtual Placeholder* Clone() const = 0;
  };

  template <typename ValueType>
  class Holder: public Placeholder
  {
  public:
    Holder(const ValueType& value)
      : _held(value)
    { }

    virtual std::string ToString() const
    {
      return any_value_to_string(_held);
    }

    virtual const std::type_info& Type() const
    {
      return typeid(ValueType);
    }

    virtual Placeholder* Clone() const
    {
      return new Holder(_held);
    }

    ValueType _held;

  private: // intentionally left unimplemented
    Holder& operator=(const Holder &);
  };

private:
    template <typename ValueType>
    friend ValueType* any_cast(Any*);

    template <typename ValueType>
    friend ValueType* unsafe_any_cast(Any*);

    Placeholder* _content;
};

class BadAnyCastException : public std::bad_cast
{
public:

  BadAnyCastException(const std::string& msg = "")
    : std::bad_cast(), _msg(msg)
  {}

  ~BadAnyCastException() throw() {}

  virtual const char * what() const throw()
  {
    if (_msg.empty())
      return "mitk::BadAnyCastException: "
             "failed conversion using mitk::any_cast";
    else
      return _msg.c_str();
  }

private:

  std::string _msg;
};

/**
 * any_cast operator used to extract the ValueType from an Any*. Will return a pointer
 * to the stored value.
 *
 * Example Usage:
 * \code
 * MyType* pTmp = any_cast<MyType*>(pAny)
 * \endcode
 * Will return NULL if the cast fails, i.e. types don't match.
 */
template <typename ValueType>
ValueType* any_cast(Any* operand)
{
  return operand && operand->Type() == typeid(ValueType)
      ? &static_cast<Any::Holder<ValueType>*>(operand->_content)->_held
      : 0;
}

/**
 * any_cast operator used to extract a const ValueType pointer from an const Any*. Will return a const pointer
 * to the stored value.
 *
 * Example Usage:
 * \code
 * const MyType* pTmp = any_cast<MyType*>(pAny)
 * \endcode
 * Will return NULL if the cast fails, i.e. types don't match.
 */
template <typename ValueType>
const ValueType* any_cast(const Any* operand)
{
  return any_cast<ValueType>(const_cast<Any*>(operand));
}

/**
 * any_cast operator used to extract a copy of the ValueType from an const Any&.
 *
 * Example Usage:
 * \code
 * MyType tmp = any_cast<MyType>(anAny)
 * \endcode
 * Will throw a BadCastException if the cast fails.
 * Dont use an any_cast in combination with references, i.e. MyType& tmp = ... or const MyType& = ...
 * Some compilers will accept this code although a copy is returned. Use the ref_any_cast in
 * these cases.
 */
template <typename ValueType>
ValueType any_cast(const Any& operand)
{
  ValueType* result = any_cast<ValueType>(const_cast<Any*>(&operand));
  if (!result) throw BadAnyCastException("Failed to convert between const Any types");
  return *result;
}

/**
 * any_cast operator used to extract a copy of the ValueType from an Any&.
 *
 * Example Usage:
 * \code
 * MyType tmp = any_cast<MyType>(anAny)
 * \endcode
 * Will throw a BadCastException if the cast fails.
 * Dont use an any_cast in combination with references, i.e. MyType& tmp = ... or const MyType& tmp = ...
 * Some compilers will accept this code although a copy is returned. Use the ref_any_cast in
 * these cases.
 */
template <typename ValueType>
ValueType any_cast(Any& operand)
{
  ValueType* result = any_cast<ValueType>(&operand);
  if (!result) throw BadAnyCastException("Failed to convert between Any types");
  return *result;
}

/**
 * ref_any_cast operator used to return a const reference to the internal data.
 *
 * Example Usage:
 * \code
 * const MyType& tmp = ref_any_cast<MyType>(anAny);
 * \endcode
 */
template <typename ValueType>
const ValueType& ref_any_cast(const Any & operand)
{
  ValueType* result = any_cast<ValueType>(const_cast<Any*>(&operand));
  if (!result) throw BadAnyCastException("RefAnyCast: Failed to convert between const Any types");
  return *result;
}

/**
 * ref_any_cast operator used to return a reference to the internal data.
 *
 * Example Usage:
 * \code
 * MyType& tmp = ref_any_cast<MyType>(anAny);
 * \endcode
 */
template <typename ValueType>
ValueType& ref_any_cast(Any& operand)
{
  ValueType* result = any_cast<ValueType>(&operand);
  if (!result) throw BadAnyCastException("RefAnyCast: Failed to convert between Any types");
  return *result;
}

/**
 * \internal
 *
 * The "unsafe" versions of any_cast are not part of the
 * public interface and may be removed at any time. They are
 * required where we know what type is stored in the any and can't
 * use typeid() comparison, e.g., when our types may travel across
 * different shared libraries.
 */
template <typename ValueType>
ValueType* unsafe_any_cast(Any* operand)
{
  return &static_cast<Any::Holder<ValueType>*>(operand->_content)->_held;
}

/**
 * \internal
 *
 * The "unsafe" versions of any_cast are not part of the
 * public interface and may be removed at any time. They are
 * required where we know what type is stored in the any and can't
 * use typeid() comparison, e.g., when our types may travel across
 * different shared libraries.
 */
template <typename ValueType>
const ValueType* unsafe_any_cast(const Any* operand)
{
  return any_cast<ValueType>(const_cast<Any*>(operand));
}

MITK_CORE_EXPORT std::string any_value_to_string(const std::vector<Any>& val);

template<class T>
std::string any_value_to_string(const T& val)
{
  std::stringstream ss;
  ss << val;
  return ss.str();
}

} // namespace mitk

inline std::ostream& operator<< (std::ostream& os, const mitk::Any& any)
{
  return os << any.ToString();
}

#endif // MITK_ANY_H
