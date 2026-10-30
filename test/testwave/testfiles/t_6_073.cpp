/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Tests error reporting: tokens following the argument of #elifdef.

//O --c++23

//E t_6_073.cpp(16): error: ill formed preprocessor directive: #elifdef MACRO Junk
#if 0
#elifdef MACRO Junk
#endif
