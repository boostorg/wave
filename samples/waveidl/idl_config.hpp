/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Sample: IDL oriented preprocessor
            Global application configuration of the Wave driver command
    
    http://spirit.sourceforge.net/

    Copyright (c) 2001-2004 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(IDL_CONFIG_HPP_012D7524_FF3F_482F_9123_91966C72F4EA_INCLUDED)
#define IDL_CONFIG_HPP_012D7524_FF3F_482F_9123_91966C72F4EA_INCLUDED

///////////////////////////////////////////////////////////////////////////////
//  Define the maximal include nesting depth allowed. If this value isn't 
//  defined it defaults to 1024
//
//  To define a new initial include nesting depth uncomment the following and 
//  supply a new integer value.
//
//#define BOOST_WAVE_MAX_INCLUDE_LEVEL_DEPTH 1024

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to support variadics and placemarkers
//
//  To implement support variadics and placemarkers uncomment the following
//
#define BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS 0

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to implement a #warning directive as 
//
//  To implement #warning directives, uncomment the following
//
#define BOOST_WAVE_SUPPORT_WARNING_DIRECTIVE 0

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to implement #pragma once 
//
//  To implement #pragma once, uncomment the following
//
#define BOOST_WAVE_SUPPORT_PRAGMA_ONCE 0

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to implement #include_next
//  Please note, that this is an extension to the C++ Standard.
//
//  To implement #include_next, uncomment the following
//
#define BOOST_WAVE_SUPPORT_INCLUDE_NEXT 0

///////////////////////////////////////////////////////////////////////////////
//  Allow the message body of the #error and #warning directives to be 
//  preprocessed before the diagnostic is issued.
//
//  Uncommenting the following will preprocess the message bodies of #error and
//  #warning messages before the error (warning) is issued
//
#define BOOST_WAVE_PREPROCESS_ERROR_MESSAGE_BODY 1

///////////////////////////////////////////////////////////////////////////////
//  Allow the #pragma directives to be returned to the caller (optionally after 
//  preprocessing the body) 
//
//  Undefining the following will skip #pragma directives, so that the caller
//  will not see them.
//
#define BOOST_WAVE_EMIT_PRAGMA_DIRECTIVES 1

///////////////////////////////////////////////////////////////////////////////
//  Allow the body of a #pragma directive to be preprocessed before the 
//  directive is returned to the caller.
//
//  Uncommenting the following will preprocess the bodies of #pragma directives
//
#define BOOST_WAVE_PREPROCESS_PRAGMA_BODY 1

///////////////////////////////////////////////////////////////////////////////
//  Allow to define macros with the command line syntax (-DMACRO(x)=definition)
//
//  Uncommenting the following will enable the possibility to define macros
//  based on the command line syntax
//
#define BOOST_WAVE_ENABLE_COMMANDLINE_MACROS 1

///////////////////////////////////////////////////////////////////////////////
//  Define the string type to be used to store the token values and the file 
//  names inside a file_position template class
//
#define BOOST_WAVE_STRINGTYPE boost::wave::util::flex_string<                 \
        char, std::char_traits<char>, std::allocator<char>,                   \
        boost::wave::util::CowString<                                         \
            boost::wave::util::AllocatorStringStorage<char>                   \
        >                                                                     \
    >                                                                         \
    /**/

//  This include is needed for the flex_string class used in the 
//  BOOST_WAVE_STRINGTYPE above.
#include <boost/wave/util/flex_string.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Uncomment the following, if you need debug output, the 
//  BOOST_SPIRIT_DEBUG_FLAGS constants below help to fine control the amount of 
//  the generated debug output
//#define BOOST_SPIRIT_DEBUG

///////////////////////////////////////////////////////////////////////////////
//  debug rules, subrules and grammars only, for possible flags see 
//  spirit/debug.hpp
#if defined(BOOST_SPIRIT_DEBUG)

#define BOOST_SPIRIT_DEBUG_FLAGS ( \
        BOOST_SPIRIT_DEBUG_FLAGS_NODES | \
        BOOST_SPIRIT_DEBUG_FLAGS_CLOSURES \
    ) \
    /**/

///////////////////////////////////////////////////////////////////////////////
//  debug flags for the pp-iterator library, possible flags (defined in 
//  wave_config.hpp):
//
//  #define BOOST_SPIRIT_DEBUG_FLAGS_CPP_GRAMMAR            0x0001
//  #define BOOST_SPIRIT_DEBUG_FLAGS_TIME_CONVERSION        0x0002
//  #define BOOST_SPIRIT_DEBUG_FLAGS_CPP_EXPR_GRAMMAR       0x0004
//  #define BOOST_SPIRIT_DEBUG_FLAGS_INTLIT_GRAMMAR         0x0008
//  #define BOOST_SPIRIT_DEBUG_FLAGS_CHLIT_GRAMMAR          0x0010
//  #define BOOST_SPIRIT_DEBUG_FLAGS_DEFINED_GRAMMAR        0x0020
//  #define BOOST_SPIRIT_DEBUG_FLAGS_PREDEF_MACROS_GRAMMAR  0x0040

#define BOOST_SPIRIT_DEBUG_FLAGS_CPP (\
        /* insert the required flags from above */ \
    ) \
    /**/
#endif 

///////////////////////////////////////////////////////////////////////////////
//
//  For all recognized preprocessor statements the output parse trees 
//  formatted as xml are printed. The formatted parse trees are streamed to the 
//  std::ostream defined by the BOOST_WAVE_DUMP_PARSE_TREE_OUT constant.
//
//  Uncomment the following, if you want to see these parse trees. 
//
//#define BOOST_WAVE_DUMP_PARSE_TREE 1
//#define BOOST_WAVE_DUMP_PARSE_TREE_OUT std::cerr

///////////////////////////////////////////////////////////////////////////////
//
//  For all #if and #elif directives the preprocessed expressions are printed.
//  These expressions are streamed to the std::ostream defined by the 
//  BOOST_WAVE_DUMP_CONDITIONAL_EXPRESSIONS_OUT constant.
//
//  Uncomment the following, if you want to see the preprocessed expressions
//
//#define BOOST_WAVE_DUMP_CONDITIONAL_EXPRESSIONS 1
//#define BOOST_WAVE_DUMP_CONDITIONAL_EXPRESSIONS_OUT std::cerr

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to use the separate compilation model for the instantiation 
//  of the C++ lexer objects.
//
//  If this is defined, you should explicitly instantiate the C++ lexer
//  template with the correct parameters in a separate compilation unit of
//  your program (see the files instantiate_slex_lexer.cpp and
//  instantiate_re2c_lexer.cpp). 
//
//  To use the lexer inclusion model, uncomment the following 
//
#define BOOST_WAVE_SEPARATE_LEXER_INSTANTIATION 1

///////////////////////////////////////////////////////////////////////////////
//  Decide, whether to use the separate compilation model for the instantiation 
//  of the grammar objects.
//
//  If this is defined, you should explicitly instantiate the grammar
//  templates with the correct parameters in a separate compilation unit of
//  your program (see the files instantiate_cpp_grammar.cpp). 
//
//  To use the grammar inclusion model, uncomment the following 
//
#define BOOST_WAVE_SEPARATE_GRAMMAR_INSTANTIATION 1

///////////////////////////////////////////////////////////////////////////////
//  configure Boost.Pool thread support (for now: no thread support at all)
#define BOOST_NO_MT

///////////////////////////////////////////////////////////////////////////////
//  MSVC specific #pragma's
#if defined(BOOST_MSVC)
#pragma warning (disable: 4355) // 'this' used in base member initializer list
#pragma warning (disable: 4800) // forcing value to bool 'true' or 'false'
#pragma inline_depth(255)
#pragma inline_recursion(on)
#endif // defined(BOOST_MSVC)

///////////////////////////////////////////////////////////////////////////////
//  Now include the cofiguration stuff for the Wave library itself
#include <boost/wave/wave_config.hpp>

#endif // !defined(IDL_CONFIG_HPP_012D7524_FF3F_482F_9123_91966C72F4EA_INCLUDED)
