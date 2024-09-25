/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2001-2012 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// ifdef and ifndef should not emit line directives in the middle of a multiline string

"a1
#ifdef A
    a2a"
#else
    a12b
    a13"
#endif

"b1
#ifndef A
    b2a"
#else
    b2b
    b3"
#endif

c

//R #line 12 "t_9_029.cpp"
//R "a1
//R a12b
//R a13"
//R #line 20 "t_9_029.cpp"
//R "b1
//R b2a"
//R #line 28 "t_9_029.cpp"
//R c

//H 10: t_9_029.cpp(13): #ifdef
//H 11: t_9_029.cpp(13): #ifdef A: 0
//H 10: t_9_029.cpp(18): #endif
//H 10: t_9_029.cpp(21): #ifndef
//H 11: t_9_029.cpp(21): #ifndef A: 0
//H 10: t_9_029.cpp(23): #else
