/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    http://spirit.sourceforge.net/

    Copyright (c) 2001-2004 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(INTERPRET_PRAGMA_HPP_B1F2315E_C5CE_4ED1_A343_0EF548B7942A_INCLUDED)
#define INTERPRET_PRAGMA_HPP_B1F2315E_C5CE_4ED1_A343_0EF548B7942A_INCLUDED

#include <cstdlib>
#include <cstdio>
#include <string>
#include <list>

#include <boost/spirit/core.hpp>
#if SPIRIT_VERSION >= 0x1700
#include <boost/spirit/actor/assign_actor.hpp>
#include <boost/spirit/actor/push_back_actor.hpp>
#endif // SPIRIT_VERSION >= 0x1700

#include <boost/wave/wave_config.hpp>

#include <boost/wave/util/pattern_parser.hpp>
#include <boost/wave/util/macro_helpers.hpp>

#include <boost/wave/token_ids.hpp>
#include <boost/wave/cpp_exceptions.hpp>
#include <boost/wave/cpp_iteration_context.hpp>
#include <boost/wave/language_support.hpp>
#include <boost/wave/trace_policies.hpp>

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
namespace boost {
namespace wave {
namespace util {

///////////////////////////////////////////////////////////////////////////////
namespace impl {

///////////////////////////////////////////////////////////////////////////////
//
//  Interpret the different Wave specific pragma directives/operators
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT, typename ContainerT>
inline bool 
interpret_pragma_trace(ContextT &ctx, ContainerT const &values,
    typename ContextT::token_type const &act_token)
{
    typedef typename ContextT::token_type token_type;
    typedef typename token_type::string_type string_type;

bool valid_option = false;

    if (1 == values.size()) {
    token_type const &value = values.front();
    
        using namespace boost::wave::trace_policies;
        if (value.get_value() == "enable" ||
            value.get_value() == "on" || 
            value.get_value() == "1") 
        {
            ctx.enable_tracing(static_cast<trace_flags>(
                ctx.tracing_enabled() | trace_macros));
            valid_option = true;
        }
        else if (value.get_value() == "disable" ||
            value.get_value() == "off" || 
            value.get_value() == "0") 
        {
            ctx.enable_tracing(static_cast<trace_flags>(
                ctx.tracing_enabled() & ~trace_macros));
            valid_option = true;
        }
    }
    if (!valid_option) {
    // unknown option value
    string_type option_str ("trace");

        if (values.size() > 0) {
            option_str += "(";
            option_str += impl::as_string(values);
            option_str += ")";
        }
        BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option,
            option_str, act_token.get_position());
    }
    return true;
}

template <typename ContextT, typename ContainerT>
inline bool
interpret_pragma_system(ContextT &/*ctx*/, ContainerT &pending,
    ContainerT const &values, typename ContextT::token_type const &act_token,
    boost::wave::language_support language)
{
    typedef typename ContextT::token_type token_type;
    typedef typename token_type::string_type string_type;

    if (0 == values.size()) {
        BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option,
            "system", act_token.get_position());
    }
    
string_type stdout_file(std::tmpnam(0));
string_type stderr_file(std::tmpnam(0));
string_type system_str(impl::as_string(values));
string_type native_cmd(system_str);

    system_str += " >" + stdout_file + " 2>" + stderr_file;
    if (0 != std::system(system_str.c_str())) {
    // unable to spawn the command
    string_type error_str("unable to spawn command: ");
    
        error_str += native_cmd;
        BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option,
            error_str, act_token.get_position());
    }
    
// rescan the content of the stdout_file and insert it as the 
// _Pragma replacement
    typedef typename ContextT::lexer_type lexer_type;
    typedef typename ContextT::input_policy_type input_policy_type;
    typedef boost::wave::iteration_context<lexer_type, input_policy_type> iteration_context_t;

iteration_context_t iter_ctx(stdout_file.c_str(), act_token.get_position(), 
    language);
ContainerT pragma;

    for (/**/; iter_ctx.first != iter_ctx.last; ++iter_ctx.first) 
        pragma.push_back(*iter_ctx.first);

// prepend the newly generated token sequence to the 'pending' container
    pending.splice(pending.begin(), pragma);

// erase the created tempfiles
    std::remove(stdout_file.c_str());
    std::remove(stderr_file.c_str());
    return true;
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
//
//  The function interpret_pragma interprets the given token sequence as the
//  body of a #pragma directive (or parameter to the _Pragma operator) and 
//  executes the actions associated with recognized Wave specific options.
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT, typename IteratorT, typename ContainerT>
inline bool 
interpret_pragma(ContextT &ctx, typename ContextT::token_type const &act_token,
    IteratorT it, IteratorT const &end, ContainerT &pending,
    boost::wave::language_support language)
{
    typedef typename ContextT::token_type token_type;
    typedef typename token_type::string_type string_type;
    
    using namespace cpplexer;
    if (T_IDENTIFIER == token_id(*it) && "wave" == (*it).get_value()) {
    //  this is a wave specific option, it should have the form:
    //      #pragma wave option(value)
    //  where '(value)' is required only for some pragma directives
    //
    //  supported #pragma directives so far:
    //      wave trace(enable) or wave trace(1)
    //      wave trace(disable) or wave trace(0)
    //      wave stop(error message)
    //      wave system(command)
    
        using namespace boost::spirit;
        token_type option;
        ContainerT values;
        
        if (!parse (++it, end, 
                        (   ch_p(T_IDENTIFIER)
                            [spirit_assign_actor(option)] 
                        |   pattern_p(KeywordTokenType, TokenTypeMask)
                            [spirit_assign_actor(option)]
                        )
                    >> !(   ch_p(T_LEFTPAREN) 
                        >>  lexeme_d[
                                *(anychar_p[spirit_append_actor(values)] - ch_p(T_RIGHTPAREN))
                            ]
                        >>  ch_p(T_RIGHTPAREN)
                        ),
                pattern_p(WhiteSpaceTokenType, TokenTypeMask)).hit)
        {
            return false;
        }
    
    // remove the falsely matched closing parenthesis
        if (values.size() > 0) {
            if (T_RIGHTPAREN == values.back()) {
            typename ContainerT::reverse_iterator rit = values.rbegin();
            
                values.erase((++rit).base());
            }
            else {
                BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option,
                    "missing matching ')'", act_token.get_position());
            }
        }
        
    // decode the option
        if (option.get_value() == "trace") {
        // enable/disable tracing option
            return impl::interpret_pragma_trace(ctx, values, act_token);
        }
        else if (option.get_value() == "stop") {
        // stop the execution and output the argument
            BOOST_WAVE_THROW(preprocess_exception, error_directive,
                impl::as_string(values), act_token.get_position());
        }
        else if (option.get_value() == "system") {
        // try to spawn the given argument as a system command and return the
        // std::cout of this process as the replacement of this _Pragma
            return impl::interpret_pragma_system(ctx, pending, values, 
                act_token, language);
        }
        else if (!ctx.interpret_pragma(
            pending, option, values, act_token, language)) 
        {
        // unknown #pragma option 
        string_type option_str (option.get_value());

            if (values.size() > 0) {
                option_str += "(";
                option_str += impl::as_string(values);
                option_str += ")";
            }
            BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option,
                option_str, act_token.get_position());
        }
        return true;
    }
#if BOOST_WAVE_SUPPORT_PRAGMA_ONCE != 0
    else if (T_IDENTIFIER == token_id(*it) && "once" == (*it).get_value()) {
    // #pragma once
        return ctx.add_pragma_once_header(ctx.get_current_filename());
    }
#endif 

    return false;
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace wave
}   // namespace boost

#endif // !defined(INTERPRET_PRAGMA_HPP_B1F2315E_C5CE_4ED1_A343_0EF548B7942A_INCLUDED)
