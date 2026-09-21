/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

//O -Werror

// Verify a function-like macro can be invoked through an object-like alias.

#define SQUARE_FUNCTION(x) (x) * (x)
#define SQUARE_FUNCTION_ALIAS SQUARE_FUNCTION

#if SQUARE_FUNCTION_ALIAS(2) == 4
#define EXECUTED_SQUARE_FUNCTION_PART_1
#else
#warning executing functional macro failed Part 1
#endif

//H 10: t_2_033.cpp(14): #define
//H 08: t_2_033.cpp(14): SQUARE_FUNCTION(x)=(x) * (x)
//H 10: t_2_033.cpp(15): #define
//H 08: t_2_033.cpp(15): SQUARE_FUNCTION_ALIAS=SQUARE_FUNCTION
//H 10: t_2_033.cpp(17): #if
//H 01: t_2_033.cpp(15): SQUARE_FUNCTION_ALIAS
//H 02: SQUARE_FUNCTION
//H 03: SQUARE_FUNCTION
//H 00: t_2_033.cpp(15): SQUARE_FUNCTION(2), [t_2_033.cpp(14): SQUARE_FUNCTION(x)=(x) * (x)]
//H 02: (2) * (2)
//H 03: (2) * (2)
//H 11: t_2_033.cpp(17): #if SQUARE_FUNCTION_ALIAS(2) == 4: 1
//H 10: t_2_033.cpp(18): #define
//H 08: t_2_033.cpp(18): EXECUTED_SQUARE_FUNCTION_PART_1=
//H 10: t_2_033.cpp(19): #else

// Verify we did not take the desired branch by chance
#if SQUARE_FUNCTION_ALIAS(3) != 9
#warning executing functional macro failed Part 2
#else
#define EXECUTED_SQUARE_FUNCTION_PART_2
#endif

//H 10: t_2_033.cpp(40): #if
//H 01: t_2_033.cpp(15): SQUARE_FUNCTION_ALIAS
//H 02: SQUARE_FUNCTION
//H 03: SQUARE_FUNCTION
//H 00: t_2_033.cpp(15): SQUARE_FUNCTION(3), [t_2_033.cpp(14): SQUARE_FUNCTION(x)=(x) * (x)]
//H 02: (3) * (3)
//H 03: (3) * (3)
//H 11: t_2_033.cpp(40): #if SQUARE_FUNCTION_ALIAS(3) != 9: 0
//H 10: t_2_033.cpp(43): #define
//H 08: t_2_033.cpp(43): EXECUTED_SQUARE_FUNCTION_PART_2=
//H 10: t_2_033.cpp(44): #endif
