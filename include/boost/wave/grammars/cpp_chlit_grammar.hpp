/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(CPP_CHLIT_GRAMMAR_HPP_9527D349_6592_449A_A409_42A001E6C64C_INCLUDED)
#define CPP_CHLIT_GRAMMAR_HPP_9527D349_6592_449A_A409_42A001E6C64C_INCLUDED

#include <boost/spirit/core.hpp>
#include <boost/spirit/attribute/closure.hpp>
#if SPIRIT_VERSION >= 0x1700
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#endif // SPIRIT_VERSION >= 0x1700

#include <boost/spirit/phoenix/operators.hpp>
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/statements.hpp>

#include <boost/wave/wave_config.hpp>   
#include <boost/wave/cpp_exceptions.hpp>   
#include <boost/wave/grammars/cpp_literal_grammar_gen.hpp>

#if !defined(spirit_append_actor)
#if SPIRIT_VERSION >= 0x1700
#define spirit_append_actor(actor) boost::spirit::push_back_a(actor)
#define spirit_assign_actor(actor) boost::spirit::assign_a(actor)
#else
#define spirit_append_actor(actor) boost::spirit::append(actor)
#define spirit_assign_actor(actor) boost::spirit::assign(actor)
#endif // SPIRIT_VERSION >= 0x1700
#endif // !defined(spirit_append_actor)

///////////////////////////////////////////////////////////////////////////////
//
//  Reusable grammar to parse a C++ style character literal 
//
///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave { 
namespace grammars {

namespace closures {

    struct chlit_closure 
    :   boost::spirit::closure<chlit_closure, unsigned int> 
    {
        member1 val;
    };
}

///////////////////////////////////////////////////////////////////////////////
//  define, whether the rule's should generate some debug output
#define TRACE_CHLIT_GRAMMAR \
    bool(BOOST_SPIRIT_DEBUG_FLAGS_CPP & BOOST_SPIRIT_DEBUG_FLAGS_CHLIT_GRAMMAR) \
    /**/

struct chlit_grammar :
    public boost::spirit::grammar<chlit_grammar, 
        closures::chlit_closure::context_t>
{
    chlit_grammar()
    {
        BOOST_SPIRIT_DEBUG_TRACE_GRAMMAR_NAME(*this, "chlit_grammar", 
            TRACE_CHLIT_GRAMMAR);
    }
    
    template <typename ScannerT>
    struct definition
    {
        typedef boost::spirit::rule<ScannerT> rule_t;

        rule_t ch_lit;

        definition(chlit_grammar const &self)
        {
            using namespace boost::spirit;
            
            ch_lit
                =  !ch_p('L') 
                    >>  ch_p('\'')
                    >>  (   ch_p('\\') 
                        >>  (   ch_p('a')[self.val = 0x07]    // BEL
                            |   ch_p('b')[self.val = 0x08]    // BS
                            |   ch_p('t')[self.val = 0x09]    // HT
                            |   ch_p('n')[self.val = 0x0a]    // NL
                            |   ch_p('v')[self.val = 0x0b]    // VT
                            |   ch_p('f')[self.val = 0x0c]    // FF
                            |   ch_p('r')[self.val = 0x0d]    // CR
                            |   ch_p('?')[spirit_assign_actor(self.val)]
                            |   ch_p('\'')[spirit_assign_actor(self.val)]
                            |   ch_p('\"')[spirit_assign_actor(self.val)]
                            |   ch_p('\\')[spirit_assign_actor(self.val)]
                            |   ch_p('x') 
                                >>  hex_p[spirit_assign_actor(self.val)]
                            |   uint_parser<unsigned int, 8, 1, 3>()
                                [
                                    spirit_assign_actor(self.val)
                                ]
                            |   ch_p('u') 
                                >>  uint_parser<unsigned int, 16, 4, 4>()
                                    [
                                        spirit_assign_actor(self.val)
                                    ]
                            |   ch_p('U')
                                >>  uint_parser<unsigned int, 16, 8, 8>()
                                    [
                                        spirit_assign_actor(self.val)
                                    ]
                            )
                        |   anychar_p[spirit_assign_actor(self.val)]
                        )
                    >>  ch_p('\'')
                ;

            BOOST_SPIRIT_DEBUG_TRACE_RULE(ch_lit, TRACE_CHLIT_GRAMMAR);
        }

    // start rule of this grammar
        rule_t const& start() const
        { return ch_lit; }
    };
};

#undef TRACE_CHLIT_GRAMMAR

///////////////////////////////////////////////////////////////////////////////
//  
//  The following function is defined here, to allow the separation of 
//  the compilation of the intlit_grammap from the function using it.
//  
///////////////////////////////////////////////////////////////////////////////

#if BOOST_WAVE_SEPARATE_GRAMMAR_INSTANTIATION != 0
#define BOOST_WAVE_CHLITGRAMMAR_GEN_INLINE
#else
#define BOOST_WAVE_CHLITGRAMMAR_GEN_INLINE inline
#endif 

template <typename TokenT>
BOOST_WAVE_CHLITGRAMMAR_GEN_INLINE 
unsigned int
chlit_grammar_gen<TokenT>::evaluate(TokenT const &token)
{
    using namespace boost::spirit;
    
static chlit_grammar g;
unsigned int result = 0;
typename TokenT::string_t const &token_val = token.get_value();
parse_info<typename TokenT::string_t::const_iterator> hit =
    parse(token_val.begin(), token_val.end(), g[spirit_assign_actor(result)]);

    if (!hit.hit) {
        BOOST_WAVE_THROW(preprocess_exception, ill_formed_expression, 
            token_val, token.get_position());
    }
    return result;
}

#undef BOOST_WAVE_CHLITGRAMMAR_GEN_INLINE

///////////////////////////////////////////////////////////////////////////////
}   // namespace grammars
}   // namespace wave
}   // namespace boost

#endif // !defined(CPP_CHLIT_GRAMMAR_HPP_9527D349_6592_449A_A409_42A001E6C64C_INCLUDED)
