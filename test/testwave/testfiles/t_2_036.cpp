/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Tests the evaluation order of conditional chains containing #elifdef and
// #elifndef, including nested chains and chains in skipped blocks

//O --c++23
//O -Werror

#define A
#define B

// once a branch was taken, all following branches are skipped
//R #line 23 "t_2_036.cpp"
//R taken_1
#if 1
taken_1
#elifdef A
not_taken_1
#elifndef UNDEFINED
not_taken_2
#elif 1
not_taken_3
#else
not_taken_4
#endif

//R #line 37 "t_2_036.cpp"
//R taken_2
#ifdef A
taken_2
#elifdef B
not_taken_5
#elifndef UNDEFINED
not_taken_6
#endif

//R #line 47 "t_2_036.cpp"
//R taken_3
#ifndef UNDEFINED
taken_3
#elifdef A
not_taken_7
#else
not_taken_8
#endif

// the first matching branch of a mixed chain is taken
//R #line 66 "t_2_036.cpp"
//R taken_4
#if 0
not_taken_9
#elif 0
not_taken_10
#elifdef UNDEFINED
not_taken_11
#elifndef A
not_taken_12
#elifdef B
taken_4
#elifdef A
not_taken_13
#elifndef UNDEFINED
not_taken_14
#else
not_taken_15
#endif

// the first matching branch of a chain is #elifndef
//R #line 83 "t_2_036.cpp"
//R taken_5
#if 0
not_taken_16
#elifdef UNDEFINED
not_taken_17
#elifndef UNDEFINED
taken_5
#elifdef A
not_taken_18
#endif

// nested chains
//R #line 105 "t_2_036.cpp"
//R taken_6
//R #line 109 "t_2_036.cpp"
//R taken_7
#ifdef UNDEFINED
#  if 1
not_taken_19
#  elifdef A
not_taken_20
#  elifndef UNDEFINED
not_taken_21
#  endif
#elifdef A
#  ifdef UNDEFINED
not_taken_22
#  elifndef UNDEFINED
taken_6
#  elifdef A
not_taken_23
#  endif
taken_7
#elifdef B
not_taken_24
#endif

// a branch of a chain which is skipped as a whole must not be taken
//R #line 130 "t_2_036.cpp"
//R taken_8
#if 0
#  if 0
not_taken_25
#  elifdef A
not_taken_26
#  elifndef UNDEFINED
not_taken_27
#  else
not_taken_28
#  endif
#elifndef A
not_taken_29
#else
taken_8
#endif

// #elifdef and #elifndef can be used with an #else branch in a skipped chain
//R #line 145 "t_2_036.cpp"
//R taken_9
#ifdef UNDEFINED
#  ifdef A
not_taken_30
#  elifdef B
not_taken_31
#  else
not_taken_32
#  endif
#else
taken_9
#endif
