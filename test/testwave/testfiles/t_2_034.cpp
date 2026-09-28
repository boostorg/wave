/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Jeff Trull. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Verify that an object-like macro that expands to defined works as expected

#define FOO
#define DEFND defined

#if DEFND FOO
// this is good
#else
#warning defined expansion w/o parenthesis fails
#endif

#if !DEFND FOO
#warning negated defined expansion w/o parenthesis fails
#else
// this is good
#endif

#if DEFND(FOO)
// this is good
#else
#warning defined expansion with parenthesis fails
#endif

#if !DEFND(FOO)
#warning negated defined expansion with parenthesis fails
#else
// this is good
#endif
