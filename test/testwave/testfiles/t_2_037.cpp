/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Tests #elifdef and #elifndef in C++26 mode together with predefined macros,
// special macro names and macros defined or undefined inside of the branches

//O --c++26
//O -Werror

// predefined macros are defined
//R #line 22 "t_2_037.cpp"
//R taken_1
#if 0
not_taken_1
#elifdef __cplusplus
taken_1
#endif

//R #line 30 "t_2_037.cpp"
//R taken_2
#if 0
not_taken_2
#elifdef __FILE__
taken_2
#endif

//R #line 40 "t_2_037.cpp"
//R taken_3
#if 0
not_taken_3
#elifndef __cplusplus
not_taken_3_1
#else
taken_3
#endif

// __has_include is defined in C++17 and later
//R #line 49 "t_2_037.cpp"
//R taken_4
#if 0
not_taken_4
#elifdef __has_include
taken_4
#endif

// keywords are no macro names
//R #line 58 "t_2_037.cpp"
//R taken_5
#if 0
not_taken_5
#elifndef while
taken_5
#endif

// a macro defined inside of a branch can be tested by a later chain
#define COUNTER 0
#if 0
#elifdef UNDEFINED_MACRO
#else
#define DEFINED_IN_ELSE
#endif

//R #line 74 "t_2_037.cpp"
//R taken_6
#if 0
not_taken_6
#elifdef DEFINED_IN_ELSE
taken_6
#endif

#if 0
#elifndef UNDEFINED_MACRO
#define DEFINED_IN_ELIFNDEF
#undef DEFINED_IN_ELSE
#endif

//R #line 90 "t_2_037.cpp"
//R taken_7
#if 0
not_taken_7
#elifdef DEFINED_IN_ELSE
not_taken_8
#elifndef DEFINED_IN_ELSE
taken_7
#endif

//R #line 98 "t_2_037.cpp"
//R taken_8
#ifndef DEFINED_IN_ELIFNDEF
not_taken_9
#elifdef DEFINED_IN_ELIFNDEF
taken_8
#endif

// macros defined inside of a branch which is not taken stay undefined
#if 1
#elifdef UNDEFINED_MACRO
#define DEFINED_IN_SKIPPED_BRANCH
#endif

#if 0
#elifndef UNDEFINED_MACRO
#else
#define DEFINED_IN_SKIPPED_ELSE
#endif

//R #line 120 "t_2_037.cpp"
//R taken_9
#ifdef DEFINED_IN_SKIPPED_BRANCH
not_taken_10
#elifdef DEFINED_IN_SKIPPED_ELSE
not_taken_11
#else
taken_9
#endif
