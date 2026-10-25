/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Tests the C++23 directives #elifdef and #elifndef

//O --c++23
//O -Werror

#define DEFINED_MACRO
#define DEFINED_MACRO_2

// #elifdef takes the branch for a defined macro
//R #line 24 "t_2_035.cpp"
//R taken_1
#if 0
not_taken_1
#elifdef DEFINED_MACRO
taken_1
#endif

// #elifdef skips the branch for an undefined macro
#if 0
not_taken_2
#elifdef UNDEFINED_MACRO
not_taken_3
#endif

// #elifndef takes the branch for an undefined macro
//R #line 40 "t_2_035.cpp"
//R taken_2
#if 0
not_taken_4
#elifndef UNDEFINED_MACRO
taken_2
#endif

// #elifndef skips the branch for a defined macro
#if 0
not_taken_5
#elifndef DEFINED_MACRO
not_taken_6
#endif

// the #else branch is used if no #elifdef and #elifndef was taken
//R #line 60 "t_2_035.cpp"
//R taken_3
#if 0
not_taken_7
#elifdef UNDEFINED_MACRO
not_taken_8
#elifndef DEFINED_MACRO
not_taken_9
#else
taken_3
#endif

// #ifdef and #ifndef can start the chain as well
//R #line 69 "t_2_035.cpp"
//R taken_4
#ifdef UNDEFINED_MACRO
not_taken_10
#elifdef DEFINED_MACRO
taken_4
#endif

//R #line 77 "t_2_035.cpp"
//R taken_5
#ifndef DEFINED_MACRO
not_taken_11
#elifndef UNDEFINED_MACRO
taken_5
#endif

// the directives are not sensitive to the whitespace after the '#'
//R #line 86 "t_2_035.cpp"
//R taken_6
#if 0
not_taken_12
#  elifdef    DEFINED_MACRO
taken_6
# elifndef	UNDEFINED_MACRO
not_taken_13
#endif

// the name is not macro expanded
#define EXPANDED_NAME UNDEFINED_MACRO
//R #line 98 "t_2_035.cpp"
//R taken_7
#if 0
not_taken_14
#elifdef EXPANDED_NAME
taken_7
#endif

// macros with an empty or any other replacement list are defined
#define EMPTY
#define ZERO 0
#define FUNC_LIKE(x) x
//R #line 110 "t_2_035.cpp"
//R taken_8
#if 0
not_taken_15
#elifdef EMPTY
taken_8
#endif

//R #line 118 "t_2_035.cpp"
//R taken_9
#if 0
not_taken_16
#elifdef ZERO
taken_9
#endif

//R #line 126 "t_2_035.cpp"
//R taken_10
#if 0
not_taken_17
#elifdef FUNC_LIKE
taken_10
#endif

// a macro which got undefined is not defined anymore
#undef DEFINED_MACRO_2
//R #line 138 "t_2_035.cpp"
//R taken_11
#if 0
not_taken_18
#elifdef DEFINED_MACRO_2
not_taken_19
#elifndef DEFINED_MACRO_2
taken_11
#endif
