/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Definition of the preprocessor context
    
    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(CPP_CONTEXT_HPP_907485E2_6649_4A87_911B_7F7225F3E5B8_INCLUDED)
#define CPP_CONTEXT_HPP_907485E2_6649_4A87_911B_7F7225F3E5B8_INCLUDED

#include <string>
#include <vector>
#include <stack>

#include <boost/concept_check.hpp>

#include <boost/wave/wave_config.hpp>
#include <boost/wave/token_ids.hpp>

#include <boost/wave/util/unput_queue_iterator.hpp>
#include <boost/wave/util/cpp_ifblock.hpp>
#include <boost/wave/util/cpp_include_pathes.hpp>
#include <boost/wave/util/iteration_context.hpp>
#include <boost/wave/util/cpp_iterator.hpp>
#include <boost/wave/util/cpp_macromap.hpp>

#include <boost/wave/trace_policies.hpp>
#include <boost/wave/cpp_iteration_context.hpp>
#include <boost/wave/language_support.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {

///////////////////////////////////////////////////////////////////////////////
// 
//  The C preprocessor context template class
//
//      The boost::wave::context template is the main interface class to 
//      control the behaviour of the preprocessing engine.
//
//      The following template parameters has to be supplied:
//
//      IteratorT       The iterator type of the underlying input stream
//      LexIteratorT    The lexer iterator type to use as the token factory
//      InputPolicyT    The input policy type to use for loading the files
//                      to be included. This template parameter is optional and 
//                      defaults to the 
//                          iteration_context_policies::load_file_to_string
//                      type
//      TraceT          The trace policy to use for trace and include file
//                      notification callback.
//
///////////////////////////////////////////////////////////////////////////////

template <
    typename IteratorT,
    typename LexIteratorT, 
    typename InputPolicyT = iteration_context_policies::load_file_to_string,
    typename TraceT = trace_policies::default_tracing
>
class context {

public:

// concept checks
// the given iterator shall be at least a forward iterator type
    BOOST_CLASS_REQUIRE(IteratorT, boost, ForwardIteratorConcept);
    
// public typedefs
    typedef typename LexIteratorT::token_t          token_t;
    typedef context<IteratorT, LexIteratorT, InputPolicyT, TraceT> 
        self_t;
    
    typedef IteratorT                               target_iterator_t;
    typedef LexIteratorT                            lex_t;
    typedef pp_iterator<self_t>                     iterator_t;

    typedef InputPolicyT                            input_policy_t;
    typedef typename token_t::position_t            position_t;
        
// type of a token sequence
    typedef std::list<token_t, boost::fast_pool_allocator<token_t> > 
        token_sequence_t;

// types of the policies
    typedef TraceT                                  trace_policy_t;
    
private:
// stack of shared_ptr's to the pending iteration contexts 
    typedef boost::shared_ptr<base_iteration_context<lex_t> > iteration_ptr_t;
    typedef boost::wave::util::iteration_context_stack<iteration_ptr_t> 
            iteration_context_stack_t;
    typedef typename iteration_context_stack_t::size_type iter_size_t;

public:
    context(target_iterator_t const &first_, target_iterator_t const &last_, 
            char const *fname = "<Unknown>", TraceT const &trace_ = TraceT())
    :   first(first_), last(last_), filename(fname)
#if BOOST_WAVE_SUPPORT_PRAGMA_ONCE != 0
        , current_filename(fname)
#endif 
        , macros(*this), language(boost::wave::support_cpp), trace(trace_)
    {
        macros.init_predefined_macros();
        includes.init_initial_path();
        includes.set_current_directory(filename.c_str());
    }

// iterator interface
    iterator_t begin() 
    { 
        return iterator_t(*this, first, last, position_t(filename.c_str()),
            get_language()); 
    }
    iterator_t end() const 
        { return iterator_t(); }

// maintain include paths
    bool add_include_path(char const *path_)
        { return includes.add_include_path(path_, false);}
    bool add_sysinclude_path(char const *path_)
        { return includes.add_include_path(path_, true);}
    void set_sysinclude_delimiter() { includes.set_sys_include_delimiter(); }
    typename iteration_context_stack_t::size_type get_iteration_depth() const 
        { return iter_ctxs.size(); }

// maintain defined macros
#if BOOST_WAVE_ENABLE_COMMANDLINE_MACROS != 0
    bool add_macro_definition(std::string macrostring, 
            bool is_predefined = false)
        { return boost::wave::util::add_macro_definition(*this, macrostring, 
            is_predefined, get_language()); }
#endif 
    bool add_macro_definition(token_t const &name, bool has_params,
            std::vector<token_t> &parameters, token_sequence_t &definition,
            bool is_predefined = false)
        { return macros.add_macro(name, has_params, parameters, definition, 
            is_predefined); }
    template <typename IteratorT2>
    bool is_defined_macro(IteratorT2 const &begin, IteratorT2 const &end) 
        { return macros.is_defined(begin, end); }
    bool remove_macro_definition(typename token_t::string_t const &name, 
            bool even_predefined = false)
        { return macros.remove_macro(name, even_predefined); }
    void reset_macro_definitions() 
        { macros.reset_macromap(); macros.init_predefined_macros(); }

// get the pp-iterator version information 
    static std::string get_version()  
        { return boost::wave::util::predefined_macros::get_fullversion(false); }
    static std::string get_version_string()  
        { return boost::wave::util::predefined_macros::get_versionstr(false); }

    void set_language(boost::wave::language_support language_) 
    { 
        language = language_; 
        reset_macro_definitions();
    }
    boost::wave::language_support get_language() const { return language; }
        
// change and ask for maximal possible include nesting depth
    void set_max_include_nesting_depth(iter_size_t new_depth)
        { iter_ctxs.set_max_include_nesting_depth(new_depth); }
    iter_size_t get_max_include_nesting_depth() const
        { return iter_ctxs.get_max_include_nesting_depth(); }

// enable/disable tracing
    void enable_tracing(trace_policies::trace_flags enable) 
    { trace.enable_tracing(enable); }
    trace_policies::trace_flags tracing_enabled()
    { return trace.tracing_enabled(); }
    trace_policy_t &get_trace_policy() { return trace; }

#if !defined(BOOST_NO_MEMBER_TEMPLATE_FRIENDS)
protected:
    friend class boost::wave::pp_iterator<
        boost::wave::context<IteratorT, lex_t, InputPolicyT, TraceT> >;
    friend class boost::wave::impl::pp_iterator_functor<
        boost::wave::context<IteratorT, lex_t, InputPolicyT, TraceT> >;
#endif
    
// maintain include pathes (helper functions)
    bool find_include_file (std::string &s, bool is_system, 
        char const *current_file) const
    { return includes.find_include_file(s, is_system, current_file); }
    void set_current_directory(char const *path_) 
    { includes.set_current_directory(path_); }
    
// conditional compilation contexts
    bool get_if_block_status() const { return ifblocks.get_status(); }
    void enter_if_block(bool new_status) 
        { ifblocks.enter_if_block(new_status); }
    bool enter_elif_block(bool new_status) 
        { return ifblocks.enter_elif_block(new_status); }
    bool enter_else_block() { return ifblocks.enter_else_block(); }
    bool exit_if_block() { return ifblocks.exit_if_block(); }
    typename boost::wave::util::if_block_stack::size_type get_if_block_depth() const 
        { return ifblocks.get_if_block_depth(); }

// stack of iteration contexts
    iteration_ptr_t pop_iteration_context()
        { iteration_ptr_t top = iter_ctxs.top(); iter_ctxs.pop(); return top; }
    void push_iteration_context(position_t const &act_pos, iteration_ptr_t iter_ctx)
        { iter_ctxs.push(act_pos, iter_ctx); }

    position_t &get_main_pos() { return macros.get_main_pos(); }
    
///////////////////////////////////////////////////////////////////////////////
//
//  expand_tokensequence(): 
//      expands all macros contained in a given token sequence, handles '##' 
//      and '#' pp operators and re-scans the resulting sequence 
//      (essentially preprocesses the token sequence).
//
//      The expand_undefined parameter is true during macro expansion inside
//      a C++ expression given for a #if or #elif statement. 
//
///////////////////////////////////////////////////////////////////////////////
    template <typename IteratorT2>
    token_t expand_tokensequence(IteratorT2 &first, IteratorT2 const &last, 
        token_sequence_t &pending, token_sequence_t &expanded, 
        bool expand_undefined = false)
    {
        return macros.expand_tokensequence(first, last, pending, expanded, 
            expand_undefined);
    }

    template <typename IteratorT2>
    void expand_whole_tokensequence(IteratorT2 &first, IteratorT2 const &last, 
        token_sequence_t &expanded, bool expand_undefined = true)
    {
        bool seen_qualified_name = false;
        macros.expand_whole_tokensequence(expanded, first, last, 
            expand_undefined, &seen_qualified_name);

    // remove any contained placeholder
        boost::wave::util::impl::remove_placeholders(expanded);
    }

#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
// experimental: macro scoping support
    template <typename IteratorT2>
    void import_name(IteratorT2 const &begin, IteratorT2 const &end) 
        { macros.import_name(begin, end); }
    template <typename IteratorT2>
    void begin_scope(IteratorT2 const &begin, IteratorT2 const &end) 
        { macros.begin_scope(begin, end); }
    void begin_unnamed_scope() { macros.begin_unnamed_scope(); }
    void end_scope() { macros.end_scope(); }
#endif 

public:
#if BOOST_WAVE_SUPPORT_PRAGMA_ONCE != 0
// support for #pragma once
// maintain the real name of the current preprocessed file
    void set_current_filename(char const *real_name)
        { current_filename = real_name; }
    std::string const &get_current_filename() const 
        { return current_filename; }

// maintain the list of known headers containing #pragma once 
    bool has_pragma_once(std::string const &filename)
        { return includes.has_pragma_once(filename); }
    bool add_pragma_once_header(std::string const &filename)
        { return includes.add_pragma_once_header(filename); }
#endif 
    
    template <typename ContainerT>
    bool interpret_pragma(ContainerT &pending, token_t const &option, 
        ContainerT const &values, token_t const &act_token, 
        boost::wave::language_support language)
    {
        return trace.interpret_pragma(pending, option, values, act_token, 
            language);
    }

private:
// the main input stream
    target_iterator_t const &first; // underlying input stream
    target_iterator_t const &last;
    std::string filename;           // associated main filename
#if BOOST_WAVE_SUPPORT_PRAGMA_ONCE != 0
    std::string current_filename;   // real name of current preprocessed file
#endif 
    
    boost::wave::util::if_block_stack ifblocks;    // conditional compilation contexts
    boost::wave::util::include_pathes includes;    // lists of include directories to search
    iteration_context_stack_t iter_ctxs;    // iteration contexts
    boost::wave::util::macromap<self_t> macros;    // map of defined macros
    boost::wave::language_support language;        // supported language/extensions
    trace_policy_t trace;                   // trace policy instance
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace wave
}   // namespace boost

#endif // !defined(CPP_CONTEXT_HPP_907485E2_6649_4A87_911B_7F7225F3E5B8_INCLUDED)
