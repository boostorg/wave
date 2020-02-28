/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    http://www.boost.org/

    Copyright (c) 2020 Jeff Trull. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

//O --c++20
//O -Werror

// __VA_OPT__ examples from the proposal doc P00306
// Those examples have a few weird typos. I've tried to match the
// behavior of gcc in the expected results - JET

#define LOG(msg, ...) printf(msg __VA_OPT__(,) __VA_ARGS__)
#define SDEF(sname, ...) S sname __VA_OPT__(= { __VA_ARGS__ })
#define LOG2(...)                                     \
    printf("at line=%d" __VA_OPT__(": "), __LINE__);  \
    __VA_OPT__(printf(__VA_ARGS__);)                  \
    printf("\n")

//R #line 28 "t_8_001.cpp"
//R printf("hello world\n" );
//R printf("hello world\n" );
//R printf("hello %d\n" , n);
LOG("hello world\n");
LOG("hello world\n", );
LOG("hello %d\n", n);

//R #line 35 "t_8_001.cpp"
//R S foo;
//R S bar = { 1, 2, 3 };
SDEF(foo);
SDEF(bar, 1, 2, 3);

//R #line 43 "t_8_001.cpp"
//R printf("at line=%d" , 43); printf("\n");
// P00306 example suggests the adjacent string literals are concatenated in
// this test case but that's the *compiler's* job:
//R printf("at line=%d" ": ", 44); printf("All well in zone %d", n); printf("\n");
LOG2();
LOG2("All well in zone %d", n);
