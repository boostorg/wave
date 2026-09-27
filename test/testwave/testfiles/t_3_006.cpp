/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library

    Copyright (c) 2026 Martin Medler. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

// Verify the C++98 values of of the predefined macros

// C++98 is the default and can't be set explicitly via '--c++XX' in this test framework

//R #line 14 "t_3_006.cpp"
__cplusplus //R 199711L 

//H 01: <built-in>(1): __cplusplus
//H 02: 199711L
//H 03: 199711L
