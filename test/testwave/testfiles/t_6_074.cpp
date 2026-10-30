/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Tests error reporting: #elifdef without a matching #if.

//O --c++23

//E t_6_074.cpp(15): error: the #if for this directive is missing: #elifdef
#elifdef MACRO
#endif
