/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(CPP_EXPRESSION_VALUE_HPP_452FE66D_8754_4107_AF1E_E42255A0C18A_INCLUDED)
#define CPP_EXPRESSION_VALUE_HPP_452FE66D_8754_4107_AF1E_E42255A0C18A_INCLUDED

#if defined (BOOST_SPIRIT_DEBUG)
#include <iostream>
#endif // defined(BOOST_SPIRIT_DEBUG)

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace grammars {
namespace closures {

///////////////////////////////////////////////////////////////////////////////
//
//  The closure_value class represents the closure type, which is used for the 
//  expression grammar. 
//
//      This class was introduced to allow the expression grammar to respect 
//      the numeric type of a numeric literal or expression result.
//
///////////////////////////////////////////////////////////////////////////////
class closure_value {

    typedef unsigned long ulong_t;

public:

    enum value_type {
        is_int = 1,
        is_uint = 2,
        is_bool = 3
    };
    
    closure_value() : type(is_int) { value.i = 0; }
    explicit closure_value(int i) : type(is_int) { value.i = i; }
    explicit closure_value(unsigned int ui) : type(is_uint) { value.ui = ui; }
    explicit closure_value(long i) : type(is_int) { value.i = i; }
    explicit closure_value(ulong_t ui) : type(is_uint) { value.ui = ui; }
    explicit closure_value(bool b) : type(is_bool) { value.b = b; }

    value_type get_type() const { return type; }
    
// implicit conversion
    operator int() const 
    {
        switch (type) {
        case is_uint:   return value.ui;
        case is_bool:   return value.b ? 1 : 0;
        case is_int:    break;
        }
        return value.i;
    }
    operator unsigned int() const 
    {
        switch (type) {
        case is_uint:   return value.ui;
        case is_bool:   return value.b ? 1 : 0;
        case is_int:    break;
        }
        return value.i;
    }
    operator long() const 
    {
        switch (type) {
        case is_uint:   return value.ui;
        case is_bool:   return value.b ? 1 : 0;
        case is_int:    break;
        }
        return value.i;
    }
    operator ulong_t() const 
    {
        switch (type) {
        case is_uint:   return value.ui;
        case is_bool:   return value.b ? 1 : 0;
        case is_int:    break;
        }
        return value.i;
    }
    operator bool() const 
    {
        switch (type) {
        case is_uint:   return value.ui != 0;
        case is_bool:   return value.b;
        case is_int:    break;
        }
        return value.i != 0.0;
    }

// assignment    
    closure_value &operator= (closure_value const &rhs)
    {
        switch (rhs.get_type()) {
        case is_int:    
            value.i = long(rhs); 
            type = is_int;
            break;
        
        case is_uint:   
            value.ui = ulong_t(rhs); 
            type = is_uint;
            break;
        
        case is_bool:   
            value.b = bool(rhs);
            type = is_bool;
            break;
        }
        return *this;
    }
    closure_value &operator= (int rhs)
    {
        type = is_int;
        value.i = rhs;
        return *this;
    }
    closure_value &operator= (unsigned int rhs)
    {
        type = is_uint;
        value.ui = rhs;
        return *this;
    }
    closure_value &operator= (long rhs)
    {
        type = is_int;
        value.i = rhs;
        return *this;
    }
    closure_value &operator= (ulong_t rhs)
    {
        type = is_uint;
        value.ui = rhs;
        return *this;
    }
    closure_value &operator= (bool rhs)
    {
        type = is_bool;
        value.b = rhs;
        return *this;
    }

// arithmetics
    closure_value &operator+= (closure_value const &rhs)
    {
        switch (type) {
        case is_int:    value.i += long(rhs); break;
        case is_uint:   value.ui += ulong_t(rhs); break;
        case is_bool:   
            value.i = value.b + bool(rhs);
            type = is_int;
        }
        return *this;
    }
    closure_value &operator-= (closure_value const &rhs)
    {
        switch (type) {
        case is_int:    value.i -= long(rhs); break;
        case is_uint:   value.ui -= ulong_t(rhs); break;
        case is_bool:   
            value.i = value.b - bool(rhs);
            type = is_int;
        }
        return *this;
    }
    closure_value &operator*= (closure_value const &rhs)
    {
        switch (type) {
        case is_int:    value.i *= long(rhs); break;
        case is_uint:   value.ui *= ulong_t(rhs); break;
        case is_bool:   
            value.i = value.b * bool(rhs);
            type = is_int;
        }
        return *this;
    }
    closure_value &operator/= (closure_value const &rhs)
    {
        switch (type) {
        case is_int:    value.i /= long(rhs); break;
        case is_uint:   value.ui /= ulong_t(rhs); break;
        case is_bool:   
            value.i = int(value.b) / int(bool(rhs));
            type = is_int;
        }
        return *this;
    }
    friend closure_value 
    operator- (closure_value const &rhs)
    {
        switch (rhs.type) {
        case is_int:    return closure_value(-long(rhs));
        case is_bool:   return closure_value(!bool(rhs)); 
        case is_uint:   break;
        }
        return closure_value(-(int)ulong_t(rhs));
    }
    friend closure_value 
    operator! (closure_value const &rhs)
    {
        switch (rhs.type) {
        case is_int:    return closure_value(!long(rhs));
        case is_bool:   return closure_value(!bool(rhs)); 
        case is_uint:   break;
        }
        return closure_value(!ulong_t(rhs));
    }
    
// comparison
    friend closure_value 
    operator== (closure_value const &lhs, closure_value const &rhs)
    {
        bool cmp = false;
        switch (lhs.type) {
        case is_int:    cmp = long(lhs) == long(rhs); break;
        case is_uint:   cmp = ulong_t(lhs) == ulong_t(rhs); break;
        case is_bool:   cmp = bool(lhs) == bool(rhs); break;
        }
        return closure_value(cmp);
    }
    friend closure_value 
    operator!= (closure_value const &lhs, closure_value const &rhs)
    {
        return closure_value(!bool(lhs == rhs));
    }
    friend closure_value 
    operator> (closure_value const &lhs, closure_value const &rhs)
    {
        bool cmp = false;
        switch (lhs.type) {
        case is_int:    cmp = long(lhs) > long(rhs); break;
        case is_uint:   cmp = ulong_t(lhs) > ulong_t(rhs); break;
        case is_bool:   cmp = bool(lhs) > bool(rhs); break;
        }
        return closure_value(cmp);
    }
    friend closure_value 
    operator< (closure_value const &lhs, closure_value const &rhs)
    {
        bool cmp = false;
        switch (lhs.type) {
        case is_int:    cmp = long(lhs) < long(rhs); break;
        case is_uint:   cmp = ulong_t(lhs) < ulong_t(rhs); break;
        case is_bool:   cmp = bool(lhs) < bool(rhs); break;
        }
        return closure_value(cmp);
    }
    friend closure_value 
    operator<= (closure_value const &lhs, closure_value const &rhs)
    {
        return closure_value(!bool(lhs > rhs));
    }
    friend closure_value 
    operator>= (closure_value const &lhs, closure_value const &rhs)
    {
        return closure_value(!bool(lhs < rhs));
    }

    friend closure_value 
    operator> (closure_value const &lhs, int const rhs)
    {
        bool cmp = false;
        switch (lhs.type) {
        case is_int:    cmp = long(lhs) > rhs; break;
        case is_uint:   cmp = ulong_t(lhs) > ulong_t(rhs); break;
        case is_bool:   cmp = (int)bool(lhs) > rhs; break;
        }
        return closure_value(cmp);
    }
    friend closure_value 
    operator< (closure_value const &lhs, int const rhs)
    {
        bool cmp = false;
        switch (lhs.type) {
        case is_int:    cmp = long(lhs) < rhs; break;
        case is_uint:   cmp = ulong_t(lhs) < ulong_t(rhs); break;
        case is_bool:   cmp = (int)bool(lhs) < rhs; break;
        }
        return closure_value(cmp);
    }

#if defined (BOOST_SPIRIT_DEBUG)
    friend std::ostream&
    operator<< (std::ostream &o, closure_value const &val)
    {
        switch (val.type) {
        case is_int:    o << "int(" << long(val) << ")"; break;
        case is_uint:   o << "unsigned int(" << ulong_t(val) << ")"; break;
        case is_bool:   o << "bool(" << bool(val) << ")"; break;
        }
        return o;
    }
#endif // defined(BOOST_SPIRIT_DEBUG)

private:
    value_type type;
    union {
        long i;
        ulong_t ui;
        bool b;
    } value;
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace closures
}   // namespace grammars
}   // namespace wave
}   // namespace boost

#endif // !defined(CPP_EXPRESSION_VALUE_HPP_452FE66D_8754_4107_AF1E_E42255A0C18A_INCLUDED)
