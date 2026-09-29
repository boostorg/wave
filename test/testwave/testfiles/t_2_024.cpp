/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2020 Jeff Trull. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

//O --c++17
//O -Werror

// Test __has_include() with system paths

// point system path to this directory
//O -S.

#ifdef __has_include
#if __has_include(<t_2_024.cpp>)
#define FOUND_SELF_VIA_SYSTEM_PATH
#else
#warning could not find this file via system path
#endif
#else
#warning has_include is not defined
#endif

//H 10: t_2_024.cpp(18): #ifdef
//H 11: t_2_024.cpp(18): #ifdef __has_include: 1
//H 10: t_2_024.cpp(19): #if
//H 11: t_2_024.cpp(19): #if __has_include(<t_2_024.cpp>): 1
//H 10: t_2_024.cpp(20): #define
//H 08: t_2_024.cpp(20): FOUND_SELF_VIA_SYSTEM_PATH=
//H 10: t_2_024.cpp(21): #else
//H 10: t_2_024.cpp(24): #else

#ifdef __has_include
#if 0
#elif __has_include(<t_2_024.cpp>)
#define FOUND_SELF_VIA_SYSTEM_PATH_ELIF
#else
#warning could not find this file via system path - elif
#endif
#endif

//H 10: t_2_024.cpp(37): #ifdef
//H 11: t_2_024.cpp(37): #ifdef __has_include: 1
//H 10: t_2_024.cpp(38): #if
//H 11: t_2_024.cpp(38): #if 0: 0
//H 10: t_2_024.cpp(39): #elif
//H 11: t_2_024.cpp(39): #elif __has_include(<t_2_024.cpp>): 1
//H 10: t_2_024.cpp(40): #define
//H 08: t_2_024.cpp(40): FOUND_SELF_VIA_SYSTEM_PATH_ELIF=
//H 10: t_2_024.cpp(41): #else
//H 10: t_2_024.cpp(44): #endif

#ifdef __has_include
#if !__has_include(<some_include_file.h>)
#define NOTFOUND_AS_EXPECTED
#else
#warning found nonexistent file
#endif
#else
#warning has_include is not defined
#endif

//H 10: t_2_024.cpp(57): #ifdef
//H 11: t_2_024.cpp(57): #ifdef __has_include: 1
//H 10: t_2_024.cpp(58): #if
//H 11: t_2_024.cpp(58): #if !__has_include(<some_include_file.h>): 1
//H 10: t_2_024.cpp(59): #define
//H 08: t_2_024.cpp(59): NOTFOUND_AS_EXPECTED=
//H 10: t_2_024.cpp(60): #else
//H 10: t_2_024.cpp(63): #else

#ifdef __has_include
#if 0
#elif !__has_include(<some_include_file.h>)
#define NOTFOUND_AS_EXPECTED_ELIF
#else
#warning found nonexistent file - elif
#endif
#endif

//H 10: t_2_024.cpp(76): #ifdef
//H 11: t_2_024.cpp(76): #ifdef __has_include: 1
//H 10: t_2_024.cpp(77): #if
//H 11: t_2_024.cpp(77): #if 0: 0
//H 10: t_2_024.cpp(78): #elif
//H 11: t_2_024.cpp(78): #elif !__has_include(<some_include_file.h>): 1
//H 10: t_2_024.cpp(79): #define
//H 08: t_2_024.cpp(79): NOTFOUND_AS_EXPECTED_ELIF=
//H 10: t_2_024.cpp(80): #else
//H 10: t_2_024.cpp(83): #endif

// test object-like macro defined as __has_include
#define HI __has_include
#if HI(<t_2_024.cpp>)
#define FOUND_SELF_VIA_SYSTEM_PATH_AND_OBJLIKE
#else
#warning could not find this file via system path when expanded via an object-like macro
#endif

//H 10: t_2_024.cpp(97): #define
//H 08: t_2_024.cpp(97): HI=__has_include
//H 10: t_2_024.cpp(98): #if
//H 01: t_2_024.cpp(97): HI
//H 02: __has_include
//H 03: __has_include
//H 11: t_2_024.cpp(98): #if HI(<t_2_024.cpp>): 1
//H 10: t_2_024.cpp(99): #define
//H 08: t_2_024.cpp(99): FOUND_SELF_VIA_SYSTEM_PATH_AND_OBJLIKE=
//H 10: t_2_024.cpp(100): #else

#if HI("made_up_name_dont_create.hpp")
#warning this made up file should not exist but has_include thinks otherwise
#else
// the good case
#endif

//H 10: t_2_024.cpp(115): #if
//H 01: t_2_024.cpp(97): HI
//H 02: __has_include
//H 03: __has_include
//H 11: t_2_024.cpp(115): #if HI("made_up_name_dont_create.hpp"): 0
//H 10: t_2_024.cpp(119): #endif

// test function-like macro that wraps __has_include
#define HIF(x) __has_include(x)
#if HIF(<t_2_024.cpp>)
#define FOUND_SELF_VIA_SYSTEM_PATH_AND_FUNCLIKE
#else
#warning could not find this file via system path when expanded via a function-like macro
#endif

//H 10: t_2_024.cpp(129): #define
//H 08: t_2_024.cpp(129): HIF(x)=__has_include(x)
//H 10: t_2_024.cpp(130): #if
//H 00: t_2_024.cpp(130): HIF(<t_2_024.cpp>), [t_2_024.cpp(129): HIF(x)=__has_include(x)]
//H 02: __has_include(<t_2_024.cpp>)
//H 03: 1
//H 11: t_2_024.cpp(130): #if HIF(<t_2_024.cpp>): 1
//H 10: t_2_024.cpp(131): #define
//H 08: t_2_024.cpp(131): FOUND_SELF_VIA_SYSTEM_PATH_AND_FUNCLIKE=
//H 10: t_2_024.cpp(132): #else

#define HIF(x) __has_include(x)
#if HIF("made_up_name_dont_create.hpp")
#warning found a file that should not exists but has_include thinks otherwise via function-like expansion
#else
#define DID_NOT_FIND_SELF_VIA_SYSTEM_PATH_AND_FUNCLIKE
#endif

//H 10: t_2_024.cpp(147): #define
//H 10: t_2_024.cpp(148): #if
//H 00: t_2_024.cpp(148): HIF("made_up_name_dont_create.hpp"), [t_2_024.cpp(129): HIF(x)=__has_include(x)]
//H 02: __has_include("made_up_name_dont_create.hpp")
//H 03: 0
//H 11: t_2_024.cpp(148): #if HIF("made_up_name_dont_create.hpp"): 0
//H 10: t_2_024.cpp(151): #define
//H 08: t_2_024.cpp(151): DID_NOT_FIND_SELF_VIA_SYSTEM_PATH_AND_FUNCLIKE=
//H 10: t_2_024.cpp(152): #endif
