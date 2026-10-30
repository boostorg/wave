/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Tests the hooks called for #elifdef and #elifndef

//O --c++23

#define DEFINED_MACRO

#if 0
#elifdef UNDEFINED_MACRO
#elifndef UNDEFINED_MACRO
#elifdef DEFINED_MACRO
#endif

#ifdef DEFINED_MACRO
#elifdef DEFINED_MACRO
#elifndef DEFINED_MACRO
#endif

#if 0
#elifndef DEFINED_MACRO
#elifdef DEFINED_MACRO
#endif

#ifdef UNDEFINED_MACRO
#if 0
#elifdef DEFINED_MACRO
#elifndef UNDEFINED_MACRO
#endif
#endif

//H 10: t_2_039.cpp(14): #define
//H 08: t_2_039.cpp(14): DEFINED_MACRO=
//H 10: t_2_039.cpp(16): #if
//H 11: t_2_039.cpp(16): #if 0: 0
//H 10: t_2_039.cpp(17): #elifdef
//H 11: t_2_039.cpp(17): #elifdef UNDEFINED_MACRO: 0
//H 10: t_2_039.cpp(18): #elifndef
//H 11: t_2_039.cpp(18): #elifndef UNDEFINED_MACRO: 0
//H 10: t_2_039.cpp(19): #elifdef
//H 10: t_2_039.cpp(22): #ifdef
//H 11: t_2_039.cpp(22): #ifdef DEFINED_MACRO: 1
//H 10: t_2_039.cpp(23): #elifdef
//H 10: t_2_039.cpp(24): #elifndef
//H 10: t_2_039.cpp(27): #if
//H 11: t_2_039.cpp(27): #if 0: 0
//H 10: t_2_039.cpp(28): #elifndef
//H 11: t_2_039.cpp(28): #elifndef DEFINED_MACRO: 1
//H 10: t_2_039.cpp(29): #elifdef
//H 11: t_2_039.cpp(29): #elifdef DEFINED_MACRO: 1
//H 10: t_2_039.cpp(30): #endif
//H 10: t_2_039.cpp(32): #ifdef
//H 11: t_2_039.cpp(32): #ifdef UNDEFINED_MACRO: 0
