/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Verify that #elifdef is no known directive before C++23: it is lexed as
// #elif followed by the identifier 'def'

//O --c++20

#if 0
#elifdef UNDEFINED_MACRO
#elifndef UNDEFINED_MACRO
#endif

//H 10: t_2_038.cpp(15): #if
//H 11: t_2_038.cpp(15): #if 0: 0
//H 10: t_2_038.cpp(16): #elif
//H 11: t_2_038.cpp(16): #elif def UNDEFINED_MACRO: 0
//H 10: t_2_038.cpp(17): #elif
//H 11: t_2_038.cpp(17): #elif ndef UNDEFINED_MACRO: 0
