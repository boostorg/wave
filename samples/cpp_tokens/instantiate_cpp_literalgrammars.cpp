/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Sample: prints out the preprocessed tokens returned by the pp iterator

    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)

    See Copyright.txt for full acknowledgements.
=============================================================================*/

#include "cpp_tokens.hpp"          // config data

#if BOOST_WAVE_SEPARATE_GRAMMAR_INSTANTIATION != 0

#include <string>

#include <boost/wave/token_ids.hpp>

#include "slex_token.hpp"
#include "slex_iterator.hpp"

#include <boost/wave/grammars/cpp_literal_grammar_gen.hpp>  
#include <boost/wave/grammars/cpp_intlit_grammar.hpp>
#include <boost/wave/grammars/cpp_chlit_grammar.hpp>

///////////////////////////////////////////////////////////////////////////////
//  
//  Explicit instantiation of the intlit_grammar_gen, chlit_grammar_gen and
//  floatlit_grammar_gen templates with the correct token type. This 
//  instantiates the corresponding parse function, which in turn instantiates 
//  the corresponding parser object.
//
///////////////////////////////////////////////////////////////////////////////

typedef boost::wave::cpp_token_sample::slex_token<> token_t;

template struct boost::wave::grammars::intlit_grammar_gen<token_t>;
template struct boost::wave::grammars::chlit_grammar_gen<token_t>;

#endif // #if BOOST_WAVE_SEPARATE_GRAMMAR_INSTANTIATION != 0

