/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Macro expansion engine
    
    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(CPP_MACROMAP_HPP_CB8F51B0_A3F0_411C_AEF4_6FF631B8B414_INCLUDED)
#define CPP_MACROMAP_HPP_CB8F51B0_A3F0_411C_AEF4_6FF631B8B414_INCLUDED

#include <cstdlib>
#include <ctime>

#include <list>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <algorithm>

#include <boost/assert.hpp>
#include <boost/wave/wave_config.hpp>

#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
#include <stack>
#endif 

#include <boost/filesystem/path.hpp>

#include <boost/wave/util/time_conversion_helper.hpp>
#include <boost/wave/util/unput_queue_iterator.hpp>
#include <boost/wave/util/macro_helpers.hpp>
#include <boost/wave/util/macro_definition.hpp>
#include <boost/wave/util/symbol_table.hpp>
#include <boost/wave/grammars/cpp_defined_grammar_gen.hpp>

#include <boost/wave/wave_version.hpp>
#include <boost/wave/cpp_exceptions.hpp>
#include <boost/wave/language_support.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace util {

namespace on_exit {

    // on destruction pop the first element of the list given as the argument 
    template <typename ContainerT>
    struct pop_front {

        pop_front(ContainerT &list_) : list(list_) {}
        ~pop_front() { list.pop_front(); }
        
        ContainerT &list;
    };

    // append a given list to the list given as argument 
    // on destruction pop the first element of the list given as argument 
    template <typename ContainerT>
    struct splice_pop_front {

        splice_pop_front(ContainerT &list_, ContainerT &queue) 
        :   list(list_) 
        {
            list.splice(list.end(), queue);
        }
        ~splice_pop_front() { list.pop_front(); }
        
        ContainerT &list;
    };

    // on destruction reset a referenced value to its initial state
    template <typename TypeT>
    struct reset {
    
        reset(TypeT &target_value_, TypeT new_value)
        :   target_value(target_value_), old_value(target_value_)
        {
            target_value_ = new_value;
        }
        ~reset() { target_value = old_value; }
        
        TypeT &target_value;
        TypeT old_value;
    };

    // on destruction assign the given iterator back
    template <typename IteratorT, typename UnputIteratorT>
    struct assign {

        assign(IteratorT &it_, UnputIteratorT const &uit_) 
        :   it(it_), uit(uit_) {}
        ~assign() { it = uit.base(); }
        
        IteratorT &it;
        UnputIteratorT const &uit;
    };

}   // namespace on_exit

///////////////////////////////////////////////////////////////////////////////
//
//  macromap
// 
//      This class holds all currently defined macros and on demand expands 
//      those macrodefinitions 
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
class macromap {

    typedef macromap<ContextT>                      self_t;
    typedef typename ContextT::token_t              token_t;
    typedef typename token_t::string_t              string_t;
    typedef typename token_t::position_t            position_t;
        
// type of a token sequence
    typedef typename ContextT::token_sequence_t     definition_container_t;
    
    typedef macro_definition<token_t, definition_container_t> 
        macro_definition_t;
    
    typedef symbol_table<string_t, macro_definition_t>  defined_macros_t;
    typedef typename defined_macros_t::value_type::second_type macro_ref_t;

    typedef std::vector<token_t>                    parameter_container_t;

public:
    macromap(ContextT &ctx_) 
    :   current_macros(0), defined_macros(new defined_macros_t(1)), 
        main_pos("", 0), ctx(ctx_), macro_uid(1)
    {
        current_macros = defined_macros.get();
    }
    ~macromap() {}

    bool add_macro(token_t const &name, bool has_parameters, 
        parameter_container_t &parameters, definition_container_t &definition, 
        bool is_predefined = false, defined_macros_t *scope = 0);
    bool is_defined(string_t const &name, 
        typename defined_macros_t::iterator &it, 
        defined_macros_t *scope = 0) const;
    template <typename IteratorT>
    bool is_defined(IteratorT const &begin, IteratorT const &end);
    bool remove_macro(string_t const &name, bool even_predefined = false);
    
    template <typename IteratorT, typename ContainerT>
    token_t const &expand_tokensequence(IteratorT &first, IteratorT const &last,
        ContainerT &pending, ContainerT &expanded, bool expand_undefined);

    template <typename IteratorT, typename ContainerT>
    void expand_whole_tokensequence(ContainerT &expanded, 
        IteratorT const &first, IteratorT const &last, bool expand_undefined,
        bool *seen_qualified_name = 0);

    void init_predefined_macros(defined_macros_t *scope = 0, 
        bool at_global_scope = true);
    void predefine_macro(defined_macros_t *scope, string_t const &name, 
        token_t const &t);
    void reset_macromap();

    position_t &get_main_pos() { return main_pos; }
    
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
// experimental: macro scoping support
    template <typename IteratorT>
    void import_name(IteratorT const &begin, IteratorT const &end);
    template <typename IteratorT>
    void begin_scope(IteratorT const &begin, IteratorT const &end);
    void begin_unnamed_scope();
    void end_scope();
#endif 

protected:
    template <typename IteratorT, typename ContainerT>
    token_t const &expand_tokensequence_worker(ContainerT &pending, 
        unput_queue_iterator<IteratorT, token_t, ContainerT> &first, 
        unput_queue_iterator<IteratorT, token_t, ContainerT> const &last, 
        bool expand_undefined, bool *seen_qualified_name = 0);
    template <typename IteratorT, typename ContainerT>
    token_t const &expand_tokensequence_worker_classical(ContainerT &pending, 
        unput_queue_iterator<IteratorT, token_t, ContainerT> &first, 
        unput_queue_iterator<IteratorT, token_t, ContainerT> const &last, 
        bool expand_undefined);
        
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    template <typename IteratorT, typename ContainerT>
    token_t const &expand_tokensequence_worker_cpp0x(ContainerT &pending, 
        unput_queue_iterator<IteratorT, token_t, ContainerT> &first, 
        unput_queue_iterator<IteratorT, token_t, ContainerT> const &last, 
        bool expand_undefined, bool *seen_qualified_name = 0);
    void open_scope(string_t const &name, defined_macros_t *current_scope, 
        boost::shared_ptr<defined_macros_t> &new_scope);
#endif

    template <typename IteratorT, typename ContainerT, typename SizeT>
    typename std::vector<ContainerT>::size_type collect_arguments (
        token_t const curr_token, std::vector<ContainerT> &arguments, 
        IteratorT &next, IteratorT const &end, SizeT const &parameter_count);

    template <typename IteratorT, typename ContainerT>
    bool expand_macro(ContainerT &pending, token_t const &name, 
        typename defined_macros_t::iterator it, 
        IteratorT &first, IteratorT const &last, bool expand_undefined,
        defined_macros_t *scope = 0, ContainerT *queue_symbol = 0,
        bool *seen_qualified_name = 0);

    template <typename ContainerT>
    bool expand_predefined_macro(token_t const &curr_token, 
        ContainerT &expanded);

    template <typename ContainerT>
    void expand_argument (typename std::vector<ContainerT>::size_type arg,
        std::vector<ContainerT> &arguments, 
        std::vector<ContainerT> &expanded_args, bool expand_undefined,
        std::vector<bool> &has_expanded_args);

    template <typename ContainerT>
    void expand_replacement_list(
        macro_definition_t const &macrodefinition,
        std::vector<ContainerT> &arguments, 
        bool expand_undefined, ContainerT &expanded);

    template <typename ContainerT>
    void rescan_replacement_list(token_t const &curr_token, 
        macro_definition_t &macrodef, ContainerT &replacement_list, 
        ContainerT &expanded, bool expand_undefined, bool *seen_qualified_name);

    template <typename IteratorT, typename ContainerT>
    token_t const &resolve_defined(IteratorT &first, IteratorT const &last,
        ContainerT &expanded);
    template <typename IteratorT, typename ContainerT>
    bool resolve_operator_pragma(IteratorT &first, 
        IteratorT const &last, ContainerT &expanded);

    template <typename ContainerT>
    void concat_tokensequence(ContainerT &expanded);

    template <typename ContainerT>
    bool is_invalid_concat(string_t new_value, 
        position_t const &pos, ContainerT &rescanned);

    static bool 
    definition_equals(definition_container_t const &definition,
      definition_container_t const &new_definition);

    static bool 
    token_equals(token_t const &left, token_t const &right);

private:
    defined_macros_t *current_macros;                   // current symbol table
    boost::shared_ptr<defined_macros_t> defined_macros; // global symbol table
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    std::stack<boost::shared_ptr<defined_macros_t> > scopes;   // opened scopes
#endif 

    token_t act_token;      // current token
    position_t main_pos;    // last token position in the pp_iterator
    ContextT &ctx;          // context object associated with the macromap
    long macro_uid;
};

///////////////////////////////////////////////////////////////////////////////
// 
//  add_macro(): adds a new macro to the macromap
//
///////////////////////////////////////////////////////////////////////////////
namespace {

    //  Test is a given identifier resolves to a predefined name
    template <typename StringT>
    inline bool 
    is_special_macroname (StringT const &name)
    {
        if (name.size() < 7)
            return false;
            
        if ("defined" == name)
            return true;
            
        if ('_' == name[0] && '_' == name[1]) {
        StringT str = name.substr(2);
        
            if (str == "cplusplus"  || str == "STDC__" || 
                str == "TIME__"     || str == "DATE__" ||
                str == "LINE__"     || str == "FILE__" ||
                str == "INCLUDE_LEVEL__"
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
                || str == "STDC_FULL_" BOOST_WAVE_PP_REGION_UC "__"
                || str == "STDC_CURRENT_" BOOST_WAVE_PP_REGION_UC "__"
#endif 
                )
            {
                return true;
            }
        }
        return false;
    }
}

template <typename ContextT>
inline bool 
macromap<ContextT>::token_equals(token_t const &left, token_t const &right)
{
    using namespace boost::wave;
    
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
    if (T_PARAMETERBASE == token_id(left) || 
        T_EXTPARAMETERBASE == token_id(left)) 
#else
    if (T_PARAMETERBASE == token_id(left))
#endif 
    {
    //  if the existing token is of type T_PARAMETERBASE, then the right token 
    //  must be of type T_IDENTIFIER or a keyword
        return (T_IDENTIFIER == token_id(right) || 
                IS_CATEGORY(right, KeywordTokenType)) && 
            left.get_value() == right.get_value();
    }

    // if the left token has whitespace, the value is irrelevant
    return token_id(left) == token_id(right) && (
            IS_CATEGORY(left, WhiteSpaceTokenType) ||
            left.get_value() == right.get_value()
        );
}

template <typename ContextT>
inline bool 
macromap<ContextT>::definition_equals(definition_container_t const &definition,
  definition_container_t const &new_definition)
{
typename definition_container_t::const_iterator first1 = definition.begin();
typename definition_container_t::const_iterator last1 = definition.end();
typename definition_container_t::const_iterator first2 = new_definition.begin();
typename definition_container_t::const_iterator last2 = new_definition.end();
    
    while (first1 != last1 && token_equals(*first1, *first2)) {
    // all consecutive whitespace tokens count as one whitespace
        impl::skip_whitespace(first1, last1);
        impl::skip_whitespace(first2, last2);
    }
    return (first1 == last1 && first2 == last2) ? true : false;
}

template <typename ContextT>
inline bool 
macromap<ContextT>::add_macro(token_t const &name, bool has_parameters, 
    parameter_container_t &parameters, definition_container_t &definition, 
    bool is_predefined, defined_macros_t *scope)
{
    if (!is_predefined && is_special_macroname (name.get_value())) {
    // exclude special macro names
        BOOST_WAVE_THROW(preprocess_exception, illegal_redefinition, 
            name.get_value(), main_pos);
    }
    
// try to define the new macro
defined_macros_t *current_scope = scope ? scope : current_macros;
typename defined_macros_t::iterator it = current_scope->find(name.get_value());

    if (it != current_scope->end()) {
    // redefinition, should not be different
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
        if ((*it).second->is_functionlike != has_parameters ||
            (*it).second->macroparameters != parameters ||
            !definition_equals((*it).second->macrodefinition, definition))
#else
        if ((*it).data()->is_functionlike != has_parameters ||
            (*it).data()->macroparameters != parameters ||
            !definition_equals((*it).data()->macrodefinition, definition))
#endif 
        {
            BOOST_WAVE_THROW(preprocess_exception, macro_redefinition, 
                name.get_value(), main_pos);
        }
        return false;
    }

// test the validity of the parameter names
    if (has_parameters) {
        std::set<typename token_t::string_t> names;
    
        typedef typename parameter_container_t::iterator 
            parameter_iterator_t;
        typedef typename std::set<typename token_t::string_t>::iterator 
            name_iterator_t;
            
        parameter_iterator_t end = parameters.end();
        for (parameter_iterator_t it = parameters.begin(); it != end; ++it) {
        name_iterator_t pit = names.find((*it).get_value());
        
            if (pit != names.end()) {
            // duplicate parameter name
                BOOST_WAVE_THROW(preprocess_exception, duplicate_parameter_name, 
                    (*pit), main_pos);
            }
            names.insert((*it).get_value());
        }
    }
    
// insert a new macro node
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
    std::pair<typename defined_macros_t::iterator, bool> p = 
        current_scope->insert(
            defined_macros_t::value_type(
                name.get_value(), 
                macro_ref_t(new macro_definition_t(name, 
                    has_parameters, is_predefined, ++macro_uid)
                )
            )
        );

    if (!p.second) {
        BOOST_WAVE_THROW(preprocess_exception, macro_insertion_error, 
            name.get_value(), main_pos);
    }

// add the parameters and the definition
    std::swap((*p.first).second->macroparameters, parameters);
    std::swap((*p.first).second->macrodefinition, definition);
#else
    std::pair<typename defined_macros_t::iterator, bool> p = 
        current_scope->insert(name.get_value());

    if (!p.second) {
        BOOST_WAVE_THROW(preprocess_exception, macro_insertion_error, 
            name.get_value(), main_pos);
    }
    (*p.first).data() = macro_ref_t(new macro_definition_t(
        name, has_parameters, is_predefined, ++macro_uid));

// add the parameters and the definition
    std::swap((*p.first).data()->macroparameters, parameters);
    std::swap((*p.first).data()->macrodefinition, definition);
#endif // BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// 
//  is_defined(): returns, whether a given macro is already defined
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
inline bool 
macromap<ContextT>::is_defined(typename token_t::string_t const &name,
    typename defined_macros_t::iterator &it, 
    defined_macros_t *scope) const
{
    if (0 == scope) scope = current_macros;
    
    if ((it = scope->find(name)) != scope->end())
        return true;        // found in symbol table

// quick pre-check
    if (name.size() < 8 || '_' != name[0] || '_' != name[1])
        return false;       // quick check failed

    return name == "__LINE__" || name == "__FILE__" || 
        name == "__INCLUDE_LEVEL__";
}

template <typename ContextT>
template <typename IteratorT>
inline bool 
macromap<ContextT>::is_defined(IteratorT const &begin, 
    IteratorT const &end) 
{
    using namespace cpplexer;
    
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    if (boost::wave::need_cpp0x(ctx.get_language())) {
    defined_macros_t *current_scope = 0;
    bool was_colon_colon = false;

        for (IteratorT it = begin; it != end; /**/) {
            if (token_id(*it) == T_COLON_COLON) {
                if (was_colon_colon) {
                // error: two consecutive '::'
                    BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                        get_full_name(begin, end), main_pos);
                }
                if (0 == current_scope) {
                // initialize starting scope (global scope)
                    current_scope = defined_macros.get();
                }
                was_colon_colon = true;
                ++it;
            }
            else if (T_IDENTIFIER == token_id(*it) ||
                IS_CATEGORY(*it, KeywordTokenType))
            {
                if (0 == current_scope) {
                // initialize starting scope
                    current_scope = current_macros;     // start at current scope
                }
                else if (!was_colon_colon) {
                // error: missing '::' between two parts of the macro name
                    BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                        get_full_name(begin, end), main_pos);
                }
                was_colon_colon = false;
                
            string_t name ((*it).get_value());
            boost::shared_ptr<defined_macros_t> next_scope;
            typename defined_macros_t::iterator cit(
                current_scope -> find(name));
            
                if (cit != current_scope -> end()) {
                // defined as a macro name
                
                // should be the last given name part 
                    if (++it != end) 
                        break;      // consider as not defined
                    return true;    // defined as a macro
                }
                else if (current_scope -> get_scope(name, next_scope)) {
                // if this part was the last one, then import the whole scope
                    if (++it != end) {
                    // defined as a scope name
                        current_scope = next_scope.get();
                    }  
                    else {
                        break;      // not found
                    }              
                }
                else {
                    break;          // name is not defined
                }
            }
            else {
                if (!IS_CATEGORY(*it, WhiteSpaceTokenType)) {
                // invalid token type inside a qualified name
                    BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                        get_full_name(begin, end), main_pos);
                }
                ++it;
            }
        }
    }
    else 
#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    {
    // in normal mode the name under inspection should consist of an identifier
    // only
        if (T_IDENTIFIER != token_id(*begin) && 
            !IS_CATEGORY(*begin, KeywordTokenType)) 
        {
            if (is_qualified_name(begin, end)) {
                BOOST_WAVE_THROW(preprocess_exception, unexpected_qualified_name, 
                    get_full_name(begin, end), main_pos);
            }
            else {
                BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                    get_full_name(begin, end), main_pos);
            }
        }

    IteratorT it = begin;
    string_t name ((*it).get_value());
    typename defined_macros_t::iterator cit(
        current_macros -> find(name));

        if (++it != end) {
        // there should be only one token as the inspected name
            if (is_qualified_name(begin, end)) {
                BOOST_WAVE_THROW(preprocess_exception, unexpected_qualified_name, 
                    get_full_name(begin, end), main_pos);
            }
            else {
                BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                    get_full_name(begin, end), main_pos);
            }
        }
        return cit != current_macros -> end();
    }
    return false;       // not defined
}

///////////////////////////////////////////////////////////////////////////////
// 
//  remove_macro(): remove a macro from the macromap
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
inline bool 
macromap<ContextT>::remove_macro(string_t const &name, 
    bool even_predefined)
{
    typename defined_macros_t::iterator it = current_macros->find(name);
    
    if (it != current_macros->end()) {
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
        if ((*it).second->is_predefined) {
#else
        if ((*it).data()->is_predefined) {
#endif 
            if (!even_predefined || is_special_macroname(name)) {
                BOOST_WAVE_THROW(preprocess_exception, bad_undefine_statement, 
                    name, main_pos);
            }
        }
        current_macros->erase(it);
        return true;
    }
    else if (is_special_macroname(name)) {
        BOOST_WAVE_THROW(preprocess_exception, bad_undefine_statement, 
            name, main_pos);
    }
    return false;       // macro was not defined
}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_tokensequence
//
//      This function is a helper function which wraps the given iterator 
//      range into corresponding unput_iterator's and calls the main workhorse
//      of the macro expansion engine (the function expand_tokensequence_worker)
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline typename ContextT::token_t const &
macromap<ContextT>::expand_tokensequence(IteratorT &first, 
    IteratorT const &last, ContainerT &pending, ContainerT &expanded, 
    bool expand_undefined)
{
    typedef impl::gen_unput_queue_iterator<IteratorT, token_t, ContainerT> gen_t;
    typedef typename gen_t::return_t iterator_t;

    ContainerT eof_queue;
    iterator_t first_it = gen_t::generate(expanded, first);
    iterator_t last_it = gen_t::generate(eof_queue, last);

bool seen_qualified_name = false;
on_exit::assign<IteratorT, iterator_t> on_exit(first, first_it);

    return expand_tokensequence_worker(pending, first_it, last_it, 
        expand_undefined, &seen_qualified_name);
}

///////////////////////////////////////////////////////////////////////////////
//
//  expand_tokensequence_worker
//
//      This function is the main workhorse of the macro expansion engine. It
//      expands as much tokens as needed to identify the next preprocessed 
//      token to return to the caller. 
//      It returns the next preprocessed token.
//
//      The iterator 'first' is adjusted accordingly.
//
///////////////////////////////////////////////////////////////////////////////

namespace {

    // strip leading and trailing whitespace
    template <typename ContainerT>
    inline void
    trim_replacement_list (ContainerT &replacement_list)
    {
        using namespace boost::wave;

    // strip leading whitespace
        if (replacement_list.size() > 0) {
        typename ContainerT::iterator end = replacement_list.end();
        typename ContainerT::iterator it = replacement_list.begin();
        
            while (it != end && IS_CATEGORY(*it, WhiteSpaceTokenType)) { 
                if (T_PLACEHOLDER != token_id(*it)) {
                    typename ContainerT::iterator next = it;
                    ++next;
                    replacement_list.erase(it);
                    it = next;
                }
                else {
                    ++it;
                }
            }
        }
        
    // strip trailing whitespace
        if (replacement_list.size() > 0) {
        typename ContainerT::reverse_iterator rend = replacement_list.rend();
        typename ContainerT::reverse_iterator rit = replacement_list.rbegin();
        
            while (rit != rend && IS_CATEGORY(*rit, WhiteSpaceTokenType)) 
                ++rit;

        typename ContainerT::iterator end = replacement_list.end();
        typename ContainerT::iterator it = rit.base();
        
            while (it != end && IS_CATEGORY(*it, WhiteSpaceTokenType)) { 
                if (T_PLACEHOLDER != token_id(*it)) {
                    typename ContainerT::iterator next = it;
                    ++next;
                    replacement_list.erase(it);
                    it = next;
                }
                else {
                    ++it;
                }
            }
        }
    }
}   // namespace

namespace impl {

    // remove all placeholder tokens from the given token sequence
    template <typename ContainerT>
    inline void
    remove_placeholders (ContainerT &replacement_list)
    {
        using namespace boost::wave;

    // strip leading whitespace
        if (replacement_list.size() > 0) {
        typename ContainerT::iterator end = replacement_list.end();
        typename ContainerT::iterator it = replacement_list.begin();
        
            while (it != end) {
                if (T_PLACEHOLDER == token_id(*it)) {
                    typename ContainerT::iterator next = it;
                    ++next;
                    replacement_list.erase(it);
                    it = next;
                }
                else {
                    ++it;
                }
            }
            
        // remove all 'new' leading and trailing whitespace 
            trim_replacement_list(replacement_list);
        }
    }
}

template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline typename ContextT::token_t const &
macromap<ContextT>::expand_tokensequence_worker(
    ContainerT &pending, 
    unput_queue_iterator<IteratorT, token_t, ContainerT> &first, 
    unput_queue_iterator<IteratorT, token_t, ContainerT> const &last, 
    bool expand_undefined, bool *seen_qualified_name)
{
// if there exist pending tokens (tokens, which are already preprocessed), then
// return the next one from there
    if (!pending.empty()) {
    on_exit::pop_front<definition_container_t> pop_front_token(pending);
    
        return act_token = pending.front();
    }

#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    if (boost::wave::need_cpp0x(ctx.get_language()))
        return expand_tokensequence_worker_cpp0x(pending, first, last, 
            expand_undefined, seen_qualified_name);
#endif 

    return expand_tokensequence_worker_classical(pending, first, last, 
        expand_undefined);
}

template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline typename ContextT::token_t const &
macromap<ContextT>::expand_tokensequence_worker_classical(
    ContainerT &pending, 
    unput_queue_iterator<IteratorT, token_t, ContainerT> &first, 
    unput_queue_iterator<IteratorT, token_t, ContainerT> const &last, 
    bool expand_undefined)
{
    using namespace boost::wave;
    typedef unput_queue_iterator<IteratorT, token_t, ContainerT> iterator_t;
    
//  analyze the next element of the given sequence, if it is an 
//  T_IDENTIFIER token, try to replace this as a macro etc.
    if (first != last) {
    token_id id = token_id(*first);

    // ignore placeholder tokens
        if (T_PLACEHOLDER == id) {
        token_t placeholder = *first;
        
            ++first;
            if (first == last)
                return act_token = placeholder;
            id = token_id(*first);
        }
            
        if (T_IDENTIFIER == id || IS_CATEGORY(id, KeywordTokenType)) {
        // try to replace this identifier as a macro
            if (expand_undefined && (*first).get_value() == "defined") {
            // resolve operator defined()
                return resolve_defined(first, last, pending);
            }
            else if (boost::wave::need_variadics(ctx.get_language()) && 
                (*first).get_value() == "_Pragma") 
            {
            // in C99 mode only: resolve the operator _Pragma
            token_t curr_token = *first;
            
                if (!resolve_operator_pragma(first, last, pending) ||
                    pending.size() > 0) 
                {
                // unknown to us pragma or supplied replacement, return the 
                // next token
                on_exit::pop_front<definition_container_t> pop_token(pending);

                    return act_token = pending.front();
                }
                
            // the operator _Pragma() was eaten completely, continue
                return act_token = token_t(T_PLACEHOLDER, "_", 
                    curr_token.get_position());
            }

        token_t name_token (*first);
        typename defined_macros_t::iterator it;
        
            if (is_defined(name_token.get_value(), it)) {
            // the current token contains an identifier, which is currently 
            // defined as a macro
                if (expand_macro(pending, name_token, it, first, last, 
                    expand_undefined)) 
                {
                // the tokens returned by expand_macro should be rescanned
                // beginning at the last token of the returned replacement list
                    if (first != last) {
                    // splice the last token back into the input queue
                    typename ContainerT::reverse_iterator rit = pending.rbegin();
                    
                        first.get_unput_queue().splice(
                            first.get_unput_queue().begin(), pending,
                            (++rit).base(), pending.end());
                    }
                                            
                // fall through ...
                }
                else if (!pending.empty()) {
                // return the first token from the pending queue
                on_exit::pop_front<definition_container_t> pop_queue (pending);
                
                    return act_token = pending.front();
                }
                else {
                // macro expansion reached the eoi
                    return act_token = token_t();
                }

            // return the next preprocessed token
                return expand_tokensequence_worker(pending, first, last, 
                    expand_undefined);
            }
//            else if (expand_undefined) {
//            // in preprocessing conditionals undefined identifiers and keywords 
//            // are to be replaced with '0' (see. C++ standard 16.1.4, [cpp.cond])
//                return act_token = 
//                    token_t(T_INTLIT, "0", (*first++).get_position());
//            }
            else {
                act_token = name_token;
                ++first;
                return act_token;
            }
        }
        else if (expand_undefined && IS_CATEGORY(*first, BoolLiteralTokenType)) {
        // expanding a constant expression inside #if/#elif, special handling
        // of 'true' and 'false'        

        // all remaining identifiers and keywords, except for true and false, 
        // are replaced with the pp-number 0 (C++ standard 16.1.4, [cpp.cond])
            return act_token = token_t(T_INTLIT, T_TRUE != id ? "0" : "1", 
                (*first++).get_position());
        }
        else {
            act_token = *first;
            ++first;
            return act_token;
        }
    }
    return act_token = token_t();     // eoi
}

///////////////////////////////////////////////////////////////////////////////
//  version to be used while in C++0x mode
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0

namespace {

    ///////////////////////////////////////////////////////////////////////////
    //  skip all tokens back, which relate with the current qualified name
    //  (the iterators are reverse_iterators)
    template <typename IteratorT>
    inline void
    rewind_identifier(IteratorT &first, IteratorT const &last)
    {
        using namespace cpplexer;
        
    IteratorT save(first);

        if (T_COLON_COLON == token_id(*save)) {
            do {
            // skip whitespace
            token_id id = impl::skip_whitespace(save, last);

                if (T_IDENTIFIER != id && 
                    !IS_CATEGORY(id, KeywordTokenType)) 
                {
                    break;
                }
            
            // save this iterator position as the new starting point
                first = save;

            // skip whitespace again   
                id = impl::skip_whitespace(save, last);
                if (T_COLON_COLON != id)
                    break;

            // save this iterator position as the new starting point
                first = save;
            
            } while (true);
        }
        else {
            do {
            // skip whitespace
            token_id id = impl::skip_whitespace(save, last);

                if (T_COLON_COLON != id) 
                    break;
                
            // save this iterator position as the new starting point
                first = save;

            // skip whitespace again   
                id = impl::skip_whitespace(save, last);
                if (T_IDENTIFIER != id && 
                    !IS_CATEGORY(id, KeywordTokenType))
                {
                    break;
                }

            // save this iterator position as the new starting point
                first = save;

            } while (true);
        }
    }
}

template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline typename ContextT::token_t const &
macromap<ContextT>::expand_tokensequence_worker_cpp0x(
    ContainerT &pending, 
    unput_queue_iterator<IteratorT, token_t, ContainerT> &first, 
    unput_queue_iterator<IteratorT, token_t, ContainerT> const &last, 
    bool expand_undefined, bool *seen_qualified_name)
{
    using namespace boost::wave;
    typedef unput_queue_iterator<IteratorT, token_t, ContainerT> iterator_t;
    
//  analyze the next element(s) of the given sequence, if it is an 
//  valid macro name, try to replace this as a macro etc.
defined_macros_t *scope = 0;
ContainerT queue_symbol;    // queue up tokens to backup the not found case
bool was_colon_colon = false;
bool found_scope = false;

    while (first != last) {
    token_id id = token_id(*first);

        if (T_COLON_COLON == id) {
            if (0 == scope) 
                scope = defined_macros.get();   // start at global scope

            if (seen_qualified_name)
                *seen_qualified_name = true;

        // continue with the next non-whitespace token
            impl::skip_whitespace(first, last, queue_symbol);
            was_colon_colon = true;
            continue;
        }

    // ignore placeholder tokens
        if (T_PLACEHOLDER == id) {
        token_t placeholder = *first;
        
            ++first;
            if (first == last)
                return act_token = placeholder;
            id = token_id(*first);
            queue_symbol.push_back(placeholder);
        }

        if (T_IDENTIFIER == id || IS_CATEGORY(id, KeywordTokenType)) {
        // try to replace this identifier as a macro
            if (expand_undefined && (*first).get_value() == "defined") {
            // resolve operator defined()
                if (0 != scope) {
                // misplaced operator defined()
                    BOOST_WAVE_THROW(preprocess_exception, misplaced_operator, 
                        "defined", main_pos);
                }
                if (seen_qualified_name)
                    *seen_qualified_name = false;
                return resolve_defined(first, last, pending);
            }
            else if (boost::wave::need_variadics(ctx.get_language()) && 
                (*first).get_value() == "_Pragma") 
            {
                if (0 != scope) {
                // misplaced operator defined()
                    BOOST_WAVE_THROW(preprocess_exception, misplaced_operator, 
                        "_Pragma", main_pos);
                }

                if (seen_qualified_name)
                    *seen_qualified_name = false;

            // in C99 mode only: resolve the operator _Pragma
            token_t curr_token = *first;
            
                if (!resolve_operator_pragma(first, last, pending) ||
                    pending.size() > 0) 
                {
                // unknown to us pragma or supplied replacement, return the 
                // next token
                on_exit::pop_front<definition_container_t> pop_token(pending);

                    return act_token = pending.front();
                }
                
            // the operator _Pragma() was eaten completely, continue
                return act_token = token_t(T_PLACEHOLDER, "_", 
                    curr_token.get_position());
            }

        // initialize scope
            if (0 == scope) {
                scope = current_macros;
            }
            else if (!was_colon_colon) {
            // not resolvable as a macro
            on_exit::splice_pop_front<definition_container_t> 
                pop_front_queue (pending, queue_symbol);
            
                return act_token = pending.front();
            }

        // search the identifier first as a scope, and if not found, 
        // subsequently as a macro
        token_t name_token(*first);
        boost::shared_ptr<defined_macros_t> next_scope;
        
            if (scope -> get_scope(name_token.get_value(), next_scope)) {
            // continue with the next non-whitespace token
            token_id idnext = impl::skip_whitespace(first, last, queue_symbol);
            
                if (T_COLON_COLON != idnext) {
                // the name under inspection may not be a qualified macro name
                    if (T_EOI != idnext && seen_qualified_name) {
                    // if eoi is reached, the expanded sequence may be 
                    // continued at the call site of this macro and still may 
                    // form a qualified name
                        *seen_qualified_name = false;
                    }

                // return the first token from the pending queue and restart 
                // with the next token
                on_exit::splice_pop_front<definition_container_t> 
                    pop_front_queue (pending, queue_symbol);
                
                    return act_token = pending.front();
                }

            // this identifier is known here as a subscope
                scope = next_scope.get();
                was_colon_colon = false;
                found_scope = true;
                if (seen_qualified_name)
                    *seen_qualified_name = true;
                
                continue;
            }

        typename defined_macros_t::iterator it;
        
            if (is_defined(name_token.get_value(), it, scope)) {
            // the current token contains an identifier, which is currently 
            // defined as a macro
            
                if (expand_macro(pending, name_token, it, first, last, 
                    expand_undefined, scope, &queue_symbol, seen_qualified_name)) 
                {
                // the tokens returned by expand_macro should be rescanned
                // beginning at the last token of the returned replacement list
                    if (first != last) {
                    // splice the last token back into the input queue
                    typename ContainerT::reverse_iterator rit = pending.rbegin();

                    // skip all remaining tokens _back_ if the last token is 
                    // the end of a qualified name
                        if (seen_qualified_name && *seen_qualified_name &&
                            (T_COLON_COLON == token_id(*rit) ||
                             IS_CATEGORY(*rit, IdentifierTokenType)) 
                           )
                        {
                            rewind_identifier(rit, pending.rend());
                        }
                        first.get_unput_queue().splice(
                            first.get_unput_queue().begin(), pending,
                            (++rit).base(), pending.end());
                    }
                                            
                // fall through ...
                }
                else if (!pending.empty()) {
                // there was some replacement, but no rescanning is required
                // (predefined macros, not-expandable macros etc.)

                // return the first token from the pending queue
                on_exit::splice_pop_front<definition_container_t> 
                    pop_front_queue (pending, queue_symbol);
                
                    return act_token = pending.front();
                }
                else {
                // macro expansion reached the eoi
                    return act_token = token_t();
                }

            // return the next preprocessed token
                return expand_tokensequence_worker(pending, first, last, 
                    expand_undefined, seen_qualified_name);
            }
//            else if (expand_undefined) {
//            // in preprocessing conditionals undefined identifiers and keywords 
//            // are to be replaced with '0' (see. C++ standard 16.1.4, [cpp.cond])
//                return act_token = 
//                    token_t(T_INTLIT, "0", (*first++).get_position());
//            }
            else {
            // the token sequence is not known as a (qualified) macro name, so
            // make this sequence pending and return the first token
                if (seen_qualified_name)
                    *seen_qualified_name = false;

                if (found_scope) {
                // If a qualified name does not find a nested name, it is 
                // not a qualified name to the preprocessor.
                    BOOST_ASSERT(queue_symbol.size() >= 2);
                
                // i.e. we have to re-evaluate the last (not found nested) 
                // name in the global context, for this reason the  
                // preceeding '::' is pushed back into the input stream
                typename ContainerT::reverse_iterator rit = queue_symbol.rbegin();

                    if (T_COLON_COLON != token_id(*rit))
                        impl::skip_whitespace(rit, queue_symbol.rend());
                    first.get_unput_queue().splice(
                        first.get_unput_queue().begin(), queue_symbol,
                        (++rit).base(), queue_symbol.end());

                // leave all up to the last '::'' in the symbol queue, which
                // is treated as preprocessed
                on_exit::splice_pop_front<definition_container_t> 
                    pop_front_queue (pending, queue_symbol);

                    return act_token = pending.front();
                }
                    
                if (was_colon_colon) {
                // the symbol queue may only be not empty if there was a '::' 
                // followed eventually by some whitespace
                    BOOST_ASSERT(!queue_symbol.empty());

                on_exit::splice_pop_front<definition_container_t> 
                    pop_front_queue (pending, queue_symbol);

                    return act_token = pending.front();
                }

                act_token = name_token;
                ++first;
                return act_token;
            }
        }
        else if (expand_undefined && IS_CATEGORY(*first, BoolLiteralTokenType)) {
        // expanding a constant expression inside #if/#elif, special handling
        // of 'true' and 'false'        

        // all remaining identifiers and keywords, except for true and false, 
        // are replaced with the pp-number 0 (C++ standard 16.1.4, [cpp.cond])
            return act_token = token_t(T_INTLIT, T_TRUE != id ? "0" : "1", 
                (*first++).get_position());
        }
        else {
        // something else (not a T_IDENTIFIER token)
            if (seen_qualified_name && !IS_CATEGORY(*first, WhiteSpaceTokenType))
                *seen_qualified_name = false;
                
            queue_symbol.push_back(*first);
            
        on_exit::splice_pop_front<definition_container_t> 
            pop_front_queue (pending, queue_symbol);

            ++first;
            return act_token = pending.front();
        }
    }

// end of input reached
    if (!queue_symbol.empty()) {
    on_exit::splice_pop_front<definition_container_t> 
        pop_queue (pending, queue_symbol);

        return act_token = pending.front();
    }
    return act_token = token_t();     // eoi
}
#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0

///////////////////////////////////////////////////////////////////////////////
// 
//  collect_arguments(): collect the actual arguments of a macro invocation
//
//      return the number of successfully detected non-empty arguments
//
///////////////////////////////////////////////////////////////////////////////

namespace {

    template <typename ContainerT>
    inline void
    trim_argument_left (ContainerT &argument)
    {
        using namespace boost::wave;
        
    // strip leading whitespace (should be only one token)
        if (argument.size() > 0 &&
            IS_CATEGORY(argument.front(), WhiteSpaceTokenType))
        {
            argument.pop_front();
        }
    }
    
    template <typename ContainerT>
    inline void
    trim_argument_right (ContainerT &argument)
    {
        using namespace boost::wave;
        
    // strip trailing whitespace (should be only one token)
        if (argument.size() > 0 &&
            IS_CATEGORY(argument.back(), WhiteSpaceTokenType))
        {
            argument.pop_back();
        }
    }

    template <typename ContainerT>
    inline void
    trim_argument (ContainerT &argument)
    {
        trim_argument_left(argument);
        trim_argument_right(argument);
    }
    
    template <typename ContainerT>
    inline bool
    is_whitespace_only (ContainerT const &argument)
    {
        using namespace cpplexer;
        
        typename ContainerT::const_iterator end = argument.end();
        for (typename ContainerT::const_iterator it = argument.begin();
             it != end; ++it)
        {
            if (!IS_CATEGORY(*it, WhiteSpaceTokenType))
                return false;
        }
        return true;
    }
}

template <typename ContextT>
template <typename IteratorT, typename ContainerT, typename SizeT>
inline typename std::vector<ContainerT>::size_type 
macromap<ContextT>::collect_arguments (token_t const curr_token, 
    std::vector<ContainerT> &arguments, IteratorT &next, IteratorT const &end, 
    SizeT const &parameter_count)
{
    using namespace boost::wave;
    
    arguments.push_back(ContainerT());
    
// collect the actual arguments
typename std::vector<ContainerT>::size_type count_arguments = 0;
int nested_parenthesis_level = 1;
ContainerT *argument = &arguments[0];
bool was_whitespace = false;
token_t startof_argument_list = *next;

    while (++next != end && nested_parenthesis_level) {
    token_id id = token_id(*next);

        if (0 == parameter_count && 
            !IS_CATEGORY((*next), WhiteSpaceTokenType) && id != T_NEWLINE &&
            id != T_RIGHTPAREN && id != T_LEFTPAREN) 
        {
        // there shouldn't be any arguments
            BOOST_WAVE_THROW(preprocess_exception, too_many_macroarguments, 
                curr_token.get_value(), main_pos);
        }
        
        switch (id) {
        case T_LEFTPAREN:
            ++nested_parenthesis_level;
            argument->push_back(*next);
            was_whitespace = false;
            break;
            
        case T_RIGHTPAREN:
            {
                if (--nested_parenthesis_level >= 1)
                    argument->push_back(*next);
                else {
                // found closing parenthesis
//                    trim_argument(argument);
                    if (parameter_count > 0) {
                        if (0 == argument->size() || 
                            is_whitespace_only(*argument)) 
                        {
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                            if (boost::wave::need_variadics(ctx.get_language())) {
                            // store a placemarker as the argument
                                argument->push_back(token_t(T_PLACEMARKER, "§", 
                                    (*next).get_position()));
                                ++count_arguments;
                            }
#endif
                        }
                        else {
                            ++count_arguments;
                        }
                    }
                }
                was_whitespace = false;
            }
            break;
        
        case T_COMMA:
            if (1 == nested_parenthesis_level) {
            // next parameter
//                trim_argument(argument);
                if (0 == argument->size() || 
                    is_whitespace_only(*argument)) 
                {
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                    if (boost::wave::need_variadics(ctx.get_language())) {
                    // store a placemarker as the argument
                        argument->push_back(token_t(T_PLACEMARKER, "§", 
                            (*next).get_position()));
                        ++count_arguments;
                    }
#endif
                }
                else {
                    ++count_arguments;
                }
                arguments.push_back(ContainerT()); // add new arg
                argument = &arguments[arguments.size()-1];
            }
            else {
            // surrounded by parenthesises, so store to current argument
                argument->push_back(*next);
            }
            was_whitespace = false;
            break;

        case T_SPACE:
        case T_SPACE2:
        case T_CCOMMENT:
        case T_NEWLINE:
            if (!was_whitespace) 
                argument->push_back(token_t(T_SPACE, " ", (*next).get_position()));
            was_whitespace = true;
            break;      // skip whitespace

        case T_PLACEHOLDER:
            break;      // ignore placeholder
                        
        default:
            argument->push_back(*next);
            was_whitespace = false;
            break;
        }
    }

    if (nested_parenthesis_level >= 1) {
    // missing ')': improperly terminated macro invocation
        BOOST_WAVE_THROW(preprocess_exception, improperly_terminated_macro, 
            "missing ')'", main_pos);
    }

// if there isn't expected any argument and there really wasn't found any, 
// than remove the empty element
    if (0 == parameter_count && 0 == count_arguments) {
        BOOST_ASSERT(1 == arguments.size());
        arguments.clear();
    }
    return count_arguments;
}        

///////////////////////////////////////////////////////////////////////////////
namespace {

    using namespace boost::wave;
    
    template <typename IteratorT>
    bool skip_to_token(IteratorT &it, IteratorT const &end, token_id id)
    {
        using namespace boost::wave;
        if (token_id(*it) == id) 
            return true;
        if (++it == end) 
            return false;
        while (IS_CATEGORY(*it, WhiteSpaceTokenType) || 
               T_NEWLINE == token_id(*it)) 
        {
            if (++it == end)
                return false;
        }
        BOOST_ASSERT(token_id(*it) == id);
        return true;
    }

}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_whole_tokensequence
//
//      fully expands a given token sequence 
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline void
macromap<ContextT>::expand_whole_tokensequence(ContainerT &expanded, 
    IteratorT const &first, IteratorT const &last, bool expand_undefined,
    bool *seen_qualified_name)
{
    typedef impl::gen_unput_queue_iterator<IteratorT, token_t, ContainerT> gen_t;
    typedef typename gen_t::return_t iterator_t;

    ContainerT unput_queue;
    iterator_t first_it = gen_t::generate(unput_queue, first);
    ContainerT eof_queue;
    iterator_t last_it = gen_t::generate(eof_queue, last);

    bool was_whitespace = false;
    ContainerT pending_queue;
    while (!pending_queue.empty() || first_it != last_it) {
    token_t t = expand_tokensequence_worker(pending_queue, first_it, 
                    last_it, expand_undefined, seen_qualified_name);
    bool is_whitespace = IS_CATEGORY(t, WhiteSpaceTokenType) &&
        T_PLACEHOLDER != token_id(t);

        if (!was_whitespace || !is_whitespace) {
            if (is_whitespace && T_SPACE != token_id(t)) {
                t.set_token_id(T_SPACE);
                t.set_value(" ");
            }
            expanded.push_back(t);
        }
        was_whitespace = is_whitespace;
    }

// should have returned all expanded tokens
    BOOST_ASSERT(unput_queue.empty() && pending_queue.empty());
}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_argument
//
//      fully expands the given argument of a macro call 
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT>
template <typename ContainerT>
inline void 
macromap<ContextT>::expand_argument (
    typename std::vector<ContainerT>::size_type arg, 
    std::vector<ContainerT> &arguments, std::vector<ContainerT> &expanded_args, 
    bool expand_undefined, std::vector<bool> &has_expanded_args)
{
    if (!has_expanded_args[arg]) {
    // expand the argument only once
    bool seen_qualified_name = false;

        expand_whole_tokensequence(expanded_args[arg], arguments[arg].begin(), 
            arguments[arg].end(), expand_undefined, &seen_qualified_name);
        impl::remove_placeholders(expanded_args[arg]);
        has_expanded_args[arg] = true;
    }
}
                    
///////////////////////////////////////////////////////////////////////////////
// 
//  expand_replacement_list
//
//      fully expands the replacement list of a given macro with the 
//      actual arguments/expanded arguments
//      handles the '#' [cpp.stringize] and the '##' [cpp.concat] operator
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT>
template <typename ContainerT>
inline void
macromap<ContextT>::expand_replacement_list(
    macro_definition_t const &macrodef,
    std::vector<ContainerT> &arguments, bool expand_undefined, 
    ContainerT &expanded)
{
    using namespace boost::wave;
    typedef typename macro_definition_t::const_definition_iterator_t 
        macro_definition_iter_t;

std::vector<ContainerT> expanded_args(arguments.size());
std::vector<bool> has_expanded_args(arguments.size());
bool seen_concat = false;
bool adjacent_concat = false;
bool adjacent_stringize = false;

    macro_definition_iter_t cend = macrodef.macrodefinition.end();
    for (macro_definition_iter_t cit = macrodef.macrodefinition.begin();
        cit != cend; ++cit)
    {
    bool use_replaced_arg = true;
    token_id base_id = BASE_TOKEN(token_id(*cit));
    
        if (T_POUND_POUND == base_id) {
        // concatenation operator
            adjacent_concat = true;
            seen_concat = true;
        }
        else if (T_POUND == base_id) {
        // stringize operator
            adjacent_stringize = true;
//            seen_concat = true;
        }
        else {
            if (adjacent_stringize || adjacent_concat || 
                T_POUND_POUND == impl::next_token<macro_definition_iter_t>
                    ::peek(cit, cend))
            {
                use_replaced_arg = false;
            }
            if (adjacent_concat)    // spaces after '##' ?
                adjacent_concat = IS_CATEGORY(*cit, WhiteSpaceTokenType);
        }

        if (IS_CATEGORY((*cit), ParameterTokenType)) {
        // copy argument 'i' instead of the parameter token i
        typename ContainerT::size_type i;
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
        bool is_ellipsis = false;
        
            if (IS_EXTCATEGORY((*cit), ExtParameterTokenType)) {
                BOOST_ASSERT(boost::wave::need_variadics(ctx.get_language()));
                i = token_id(*cit) - T_EXTPARAMETERBASE;
                is_ellipsis = true;
            } 
            else 
#endif
            {
                i = token_id(*cit) - T_PARAMETERBASE;
            }
            
            BOOST_ASSERT(i < arguments.size());
            if (use_replaced_arg) {
                
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                if (is_ellipsis) {
                position_t const &pos = (*cit).get_position();

                    BOOST_ASSERT(boost::wave::need_variadics(ctx.get_language()));
                    
                // ensure all variadic arguments to be expanded
                    for (typename vector<ContainerT>::size_type arg = i; 
                         arg < expanded_args.size(); ++arg)
                    {
                        expand_argument(arg, arguments, expanded_args, 
                            expand_undefined, has_expanded_args);
                    }
                    impl::replace_ellipsis(expanded_args, i, expanded, pos);
                }
                else 
#endif
                {
                // ensure argument i to be expanded
                    expand_argument(i, arguments, expanded_args, 
                        expand_undefined, has_expanded_args);
                    
                // replace argument
                ContainerT const &arg = expanded_args[i];
                
                    std::copy(arg.begin(), arg.end(), 
                        std::inserter(expanded, expanded.end()));
                }
            }
            else if (adjacent_stringize && 
                    !IS_CATEGORY(*cit, WhiteSpaceTokenType)) 
            {
            // stringize the current argument
                BOOST_ASSERT(!arguments[i].empty());
                
            position_t const &pos = (*arguments[i].begin()).get_position();

#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                if (is_ellipsis && boost::wave::need_variadics(ctx.get_language())) {
                    trim_argument_left(arguments[i]);
                    trim_argument_right(arguments.back());
                    expanded.push_back(token_t(T_STRINGLIT, 
                        impl::as_stringlit(arguments, i, pos), pos));
                }
                else 
#endif 
                {
                    trim_argument(arguments[i]);
                    expanded.push_back(token_t(T_STRINGLIT, 
                        impl::as_stringlit(arguments[i], pos), pos));
                }
                adjacent_stringize = false;
            }
            else {
            // simply copy the original argument (adjacent '##' or '#')
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                if (is_ellipsis) {
                position_t const &pos = (*cit).get_position();

                    trim_argument_left(arguments[i]);
                    trim_argument_right(arguments.back());
                    BOOST_ASSERT(boost::wave::need_variadics(ctx.get_language()));
                    impl::replace_ellipsis(arguments, i, expanded, pos);
                }
                else
#endif
                {
                ContainerT &arg = arguments[i];
                
                    trim_argument(arg);
                    std::copy(arg.begin(), arg.end(), 
                        std::inserter(expanded, expanded.end()));
                }
            }
        }
        else if (!adjacent_stringize || T_POUND != base_id) {
        // insert the actual replacement token (if it is not the '#' operator)
            expanded.push_back(*cit);
        }
    }

    if (adjacent_stringize) {
    // error, '#' should not be the last token
        BOOST_WAVE_THROW(preprocess_exception, ill_formed_operator,
            "stringize ('#')", main_pos);
    }
        
// handle the cpp.concat operator
    if (seen_concat)
        concat_tokensequence(expanded);
}

template <typename ContextT>
template <typename ContainerT>
inline void 
macromap<ContextT>::rescan_replacement_list(token_t const &curr_token, 
    macro_definition_t &macro_def, ContainerT &replacement_list, 
    ContainerT &expanded, bool expand_undefined, bool *seen_qualified_name)
{
    using namespace boost::wave;

    if (!replacement_list.empty()) {
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
    // remove the placemarkers
        if (boost::wave::need_variadics(ctx.get_language())) {
        typename ContainerT::iterator end = replacement_list.end();
        typename ContainerT::iterator it = replacement_list.begin();
        
            while (it != end) {
                if (T_PLACEMARKER == token_id(*it)) {
                typename ContainerT::iterator placemarker = it;
                
                    ++it;
                    replacement_list.erase(placemarker);
                }
                else {
                    ++it;
                }
            }
        }
#endif

    // rescan the replacement list, during this rescan the current macro under
    // expansion isn't available as an expandable macro
    on_exit::reset<bool> on_exit(macro_def.is_available_for_replacement, false);

        expand_whole_tokensequence(expanded, replacement_list.begin(),
            replacement_list.end(), expand_undefined, seen_qualified_name);

    // trim replacement list, leave placeholder tokens untouched
        trim_replacement_list(expanded);
    }
    
    if (expanded.empty()) {
    // the resulting replacement list should contain at least a placeholder
    // token
        BOOST_ASSERT(expanded.empty());
        expanded.push_back(token_t(T_PLACEHOLDER, "_", curr_token.get_position()));
    }
}

namespace impl {

    //  The following predicate is used in conjunction with the remove_copy_if
    //  algorithm to allow the detection of an eventually copied operator ##.
    //  No removal is performed in any case.
    class find_concat_operator 
    {
    public:
        find_concat_operator(bool &found_) : found_concat(found_) {}
        
        template <typename TokenT>
        bool operator()(TokenT const &tok)
        {
            using namespace cpplexer;
            if (T_POUND_POUND == BASE_TOKEN(token_id(tok)))
                found_concat = true;
            return false;
        }

    private:
        bool &found_concat;
    };
}

///////////////////////////////////////////////////////////////////////////////
// 
//  expand_macro(): expands a defined macro
//
//      This functions tries to expand the macro, to which points the 'first'
//      iterator. The functions eats up more tokens, if the macro to expand is
//      a function-like macro.
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline bool 
macromap<ContextT>::expand_macro(ContainerT &expanded, 
    token_t const &curr_token, typename defined_macros_t::iterator it, 
    IteratorT &first, IteratorT const &last, 
    bool expand_undefined, defined_macros_t *scope, 
    ContainerT *queue_symbol, bool *seen_qualified_name) 
{
    using namespace boost::wave;
    
    if (0 == scope) scope = current_macros;
    
    BOOST_ASSERT(T_IDENTIFIER == token_id(curr_token) ||
        IS_CATEGORY(token_id(curr_token), KeywordTokenType));
        
    if (it == scope->end()) {
        ++first;    // advance

    // try to expand a predefined macro (__FILE__, __LINE__ or __INCLUDE_LEVEL__)
        if (expand_predefined_macro(curr_token, expanded))
            return false;
        
    // not defined as a macro
        if (0 != queue_symbol) {
            expanded.splice(expanded.end(), *queue_symbol);
        }
        else {
            expanded.push_back(curr_token);
        }
        return false;
    }

// ensure the parameters to be replaced with special parameter tokens
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
macro_definition_t &macro_def = *(*it).second.get();
#else
macro_definition_t &macro_def = *(*it).data().get();
#endif 

    macro_def.replace_parameters();

// test if this macro is currently available for replacement
    if (!macro_def.is_available_for_replacement) {
    // this macro is marked as non-replaceable
    // copy the macro name itself
        if (0 != queue_symbol) {
            queue_symbol->push_back(token_t(T_NONREPLACABLE_IDENTIFIER,
                curr_token.get_value(), curr_token.get_position()));
            expanded.splice(expanded.end(), *queue_symbol);
        }
        else {
            expanded.push_back(token_t(T_NONREPLACABLE_IDENTIFIER,
                curr_token.get_value(), curr_token.get_position()));
        }
        ++first;
        return false;
    }

// try to replace the current identifier as a function-like macro
ContainerT replacement_list;

    if (T_LEFTPAREN == impl::next_token<IteratorT>::peek(first, last)) {
    // called as a function-like macro 
        skip_to_token(first, last, T_LEFTPAREN);
        
        if (macro_def.is_functionlike) {
        // defined as a function-like macro
        
        // collect the arguments
        std::vector<ContainerT> arguments;
        typename std::vector<ContainerT>::size_type count_args = 
            collect_arguments (curr_token, arguments, first, last, 
                macro_def.macroparameters.size());

        // verify the parameter count
            if (count_args < macro_def.macroparameters.size() ||
                arguments.size() < macro_def.macroparameters.size()) 
            {
            // too few macro arguments
                BOOST_WAVE_THROW(preprocess_exception, too_few_macroarguments, 
                    curr_token.get_value(), main_pos);
            }
            
            if (count_args > macro_def.macroparameters.size() ||
                arguments.size() > macro_def.macroparameters.size()) 
            {
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                if (!macro_def.has_ellipsis) 
#endif 
                {
                // too many macro arguments
                    BOOST_WAVE_THROW(preprocess_exception, too_many_macroarguments, 
                        curr_token.get_value(), main_pos);
                }
            }
                
        // inject tracing support
            ctx.get_trace_policy().expanding_function_like_macro(
                macro_def.macroname, macro_def.macroparameters, 
                macro_def.macrodefinition, curr_token, arguments);
        
        // expand the replacement list of this macro
            expand_replacement_list(macro_def, arguments, expand_undefined,
                replacement_list);
        }
        else {
        // defined as an object-like macro
            ctx.get_trace_policy().expanding_object_like_macro(
                macro_def.macroname, macro_def.macrodefinition, curr_token);

        bool found = false;
        impl::find_concat_operator concat_tag(found);
        
            std::remove_copy_if(macro_def.macrodefinition.begin(), 
                macro_def.macrodefinition.end(), 
                std::inserter(replacement_list, replacement_list.end()),
                concat_tag);
                
        // handle concatenation operators
            if (found)
                concat_tokensequence(replacement_list);
        }
    }
    else {
    // called as an object like macro
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
        if ((*it).second->is_functionlike) {
#else
        if ((*it).data()->is_functionlike) {
#endif 
        // defined as a function-like macro
            if (0 != queue_symbol) {
                queue_symbol->push_back(curr_token);
                expanded.splice(expanded.end(), *queue_symbol);
            }
            else {
                expanded.push_back(curr_token);
            }
            ++first;                // skip macro name
            return false;           // no further preprocessing required
        }
        else {
        // defined as an object-like macro (expand it)
            ctx.get_trace_policy().expanding_object_like_macro(
                macro_def.macroname, macro_def.macrodefinition, curr_token);

        bool found = false;
        impl::find_concat_operator concat_tag(found);
        
            std::remove_copy_if(macro_def.macrodefinition.begin(), 
                macro_def.macrodefinition.end(), 
                std::inserter(replacement_list, replacement_list.end()),
                concat_tag);

        // handle concatenation operators
            if (found)
                concat_tokensequence(replacement_list);

            ++first;                // skip macro name
        }
    }

// rescan the replacement list
ContainerT expanded_list;

    ctx.get_trace_policy().expanded_macro(replacement_list);
    
    rescan_replacement_list(curr_token, macro_def, replacement_list, 
        expanded_list, expand_undefined, seen_qualified_name);
    
    ctx.get_trace_policy().rescanned_macro(expanded_list);  
    expanded.splice(expanded.end(), expanded_list);
    return true;        // rescan is required
}

template <typename ContextT>
template <typename ContainerT>
inline bool 
macromap<ContextT>::expand_predefined_macro(token_t const &curr_token, 
    ContainerT &expanded)
{
    using namespace boost::wave;
    
string_t const &value = curr_token.get_value();

    if (value.size() < 8 || '_' != value[0] || '_' != value[1])
        return false;       // quick check failed
        
    if (value == "__LINE__") { 
    // expand the __LINE__ macro
    char buffer[22];    // 21 bytes holds all NUL-terminated unsigned 64-bit numbers

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "%d", main_pos.get_line());
        expanded.push_back(token_t(T_INTLIT, buffer, curr_token.get_position()));
        return true;
    }
    else if (value == "__FILE__") {
    // expand the __FILE__ macro
        namespace fs = boost::filesystem;
        
    std::string file("\"");
    fs::path filename(main_pos.get_file().c_str(), fs::native);
    
        using boost::wave::util::impl::escape_lit;
        file += escape_lit(filename.native_file_string()) + "\"";
        expanded.push_back(token_t(T_STRINGLIT, file.c_str(), 
            curr_token.get_position()));
        return true;
    }
    else if (value == "__INCLUDE_LEVEL__") {
    // expand the __INCLUDE_LEVEL__ macro
    char buffer[22];    // 21 bytes holds all NUL-terminated unsigned 64-bit numbers

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "%d", ctx.get_iteration_depth());
        expanded.push_back(token_t(T_INTLIT, buffer, curr_token.get_position()));
        return true;
    }
    return false;   // no predefined token
}

///////////////////////////////////////////////////////////////////////////////
//
//  resolve_defined(): resolve the operator defined() and replace it with the 
//                     correct T_INTLIT token
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline typename ContextT::token_t const &
macromap<ContextT>::resolve_defined(IteratorT &first, 
    IteratorT const &last, ContainerT &pending) 
{
    using namespace boost::wave;
    using namespace boost::wave::grammars;

ContainerT result;
IteratorT start = first;
boost::spirit::parse_info<IteratorT> hit = 
    defined_grammar_gen<typename ContextT::lex_t>::
        parse_operator_defined(start, last, result);
    
    if (!hit.hit) {
        BOOST_WAVE_THROW(preprocess_exception, ill_formed_expression, 
            "defined()", main_pos);
    }
    impl::assign_iterator<IteratorT>::do_(first, hit.stop);

// insert a token, which reflects the outcome
    pending.push_back(token_t(T_INTLIT, 
        is_defined(result.begin(), result.end()) ? "1" : "0", 
        main_pos));

on_exit::pop_front<definition_container_t> pop_front_token(pending);

    return act_token = pending.front();
}

///////////////////////////////////////////////////////////////////////////////
//
//  resolve_operator_pragma(): resolve the operator _Pragma() and dispatch to
//                             the associated action
//
//      This function returns true, if the pragma was correctly interpreted. 
//      The iterator 'first' is positioned behind the closing ')'.
//      This function returnes false, if the _Pragma was not known, the 
//      preprocessed token sequence is pushed back to the 'pending' sequence.
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
template <typename IteratorT, typename ContainerT>
inline bool
macromap<ContextT>::resolve_operator_pragma(IteratorT &first, 
    IteratorT const &last, ContainerT &pending) 
{
// isolate the parameter of the operator _Pragma
    token_t pragma_token = *first;
    
    skip_to_token(first, last, T_LEFTPAREN);
    std::vector<ContainerT> arguments;
    typename std::vector<ContainerT>::size_type count_args = 
        collect_arguments (pragma_token, arguments, first, last, 1);

// verify the parameter count
    if (0 == pragma_token.get_position().get_file().size())
        pragma_token.set_position(act_token.get_position());
        
    if (1 > count_args || 1 > arguments.size()) {
    // too few macro arguments
        BOOST_WAVE_THROW(preprocess_exception, too_few_macroarguments, 
            pragma_token.get_value(), pragma_token.get_position());
    }
    if (1 < count_args || 1 < arguments.size()) {
    // too many macro arguments
        BOOST_WAVE_THROW(preprocess_exception, too_many_macroarguments, 
            pragma_token.get_value(), pragma_token.get_position());
    }

// preprocess the pragma token body
    ContainerT expanded;
    bool seen_qualified_name = false;
    expand_whole_tokensequence(expanded, arguments[0].begin(), 
        arguments[0].end(), false, &seen_qualified_name);

// unescape the parameter of the operator _Pragma
    typedef typename token_t::string_t string_t;
    
    string_t pragma_cmd;
    typename ContainerT::const_iterator end_exp = expanded.end();
    for (typename ContainerT::const_iterator it_exp = expanded.begin();
         it_exp != end_exp; ++it_exp)
    {
        if (T_EOF == token_id(*it_exp))
            break;
        if (IS_CATEGORY(*it_exp, WhiteSpaceTokenType))
            continue;
            
        if (T_STRINGLIT != token_id(*it_exp)) {
        // ill formed operator _Pragma
            BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option, 
                "_Pragma", pragma_token.get_position());
        }

        if (pragma_cmd.size() > 0) {
        // there should be exactly one string literal (string literals are to 
        // be concatenated at translation phase 6, but _Pragma operators are 
        // to be executed at translation phase 4)
            BOOST_WAVE_THROW(preprocess_exception, ill_formed_pragma_option, 
                "_Pragma", pragma_token.get_position());
        }
        
    // remove the '\"' and concat all given string literal-values
        string_t token_str = (*it_exp).get_value();
        pragma_cmd += token_str.substr(1, token_str.size() - 2);
    }
    pragma_cmd = impl::unescape_lit(pragma_cmd);

// tokenize the pragma body
    typedef typename ContextT::lex_t lex_t;
    
    ContainerT pragma;
    std::string pragma_cmd_str(pragma_cmd.c_str());
    lex_t it = lex_t(pragma_cmd_str.begin(), pragma_cmd_str.end(), 
        pragma_token.get_position(), ctx.get_language());
    lex_t end = lex_t();
    for (/**/; it != end; ++it) 
        pragma.push_back(*it);

// analyze the preprocessed tokensequence and eventually dispatch to the 
// associated action
    if (interpret_pragma(ctx, pragma_token, pragma.begin(), pragma.end(), 
        pending, ctx.get_language()))
    {
        return true;    // successfully recognized a wave specific pragma
    }
    
// unknown pragma token sequence, push it back and return to the caller
    pending.push_front(token_t(T_SPACE, " ", pragma_token.get_position()));
    pending.push_front(token_t(T_RIGHTPAREN, ")", pragma_token.get_position()));
    pending.push_front(token_t(T_STRINGLIT, string_t("\"") + pragma_cmd + "\"",
        pragma_token.get_position()));
    pending.push_front(token_t(T_LEFTPAREN, "(", pragma_token.get_position()));
    pending.push_front(pragma_token);
    return false;
}

// Test if the result of a concat operator is well formed. 
template <typename ContextT>
template <typename ContainerT>
inline bool
macromap<ContextT>::is_invalid_concat(string_t new_value, 
    position_t const &pos, ContainerT &rescanned)
{
// retokenize the newly generated string
    typedef typename ContextT::lex_t lex_t;
    
    std::string value_to_test(new_value.c_str());
    lex_t it = lex_t(value_to_test.begin(), value_to_test.end(), pos, 
        ctx.get_language());
    lex_t end = lex_t();
    for (/**/; it != end; ++it) 
        rescanned.push_back(*it);
        
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    if (boost::wave::need_cpp0x(ctx.get_language()))
        return false;       // in C++0x mode token pasting is well defined
#endif 
        
// test if the newly generated token sequence contains more than 1 token
// the second one is the T_EOF token
    BOOST_ASSERT(T_EOF == token_id(rescanned.back()));
    return rescanned.size() != 2;
}

template <typename ContextT>
template <typename ContainerT>
inline void 
macromap<ContextT>::concat_tokensequence(ContainerT &expanded)
{
    using namespace boost::wave;
    typedef typename ContainerT::iterator iterator_t;
    
    iterator_t end = expanded.end();
    iterator_t prev = end;
    for (iterator_t it = expanded.begin(); it != end; /**/) 
    {
        if (T_POUND_POUND == BASE_TOKEN(token_id(*it))) {
        iterator_t next = it;
    
            ++next;
            if (prev == end || next == end) {
            // error, '##' should be in between two tokens
                BOOST_WAVE_THROW(preprocess_exception, ill_formed_operator,
                    "concat ('##')", main_pos);
            }

        // replace prev##next with the concatenated value, skip whitespace
        // before and after the '##' operator
            while (IS_CATEGORY(*next, WhiteSpaceTokenType)) {
                ++next;
                if (next == end) {
                // error, '##' should be in between two tokens
                    BOOST_WAVE_THROW(preprocess_exception, ill_formed_operator,
                        "concat ('##')", main_pos);
                }
            }
            
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
            if (boost::wave::need_variadics(ctx.get_language())) {
                if (T_PLACEMARKER == token_id(*next)) {
                // remove the '##' and the next tokens from the sequence
                iterator_t first_to_delete = prev;

                    expanded.erase(++first_to_delete, ++next);
                    it = next;
                    continue;
                }
                else if (T_PLACEMARKER == token_id(*prev)) {
                // remove the '##' and the next tokens from the sequence
                iterator_t first_to_delete = prev;

                    *prev = *next;
                    expanded.erase(++first_to_delete, ++next); 
                    it = next;
                    continue;
                }
            }
#endif // BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0

        // test if the concat operator has to concatenate two unrelated 
        // tokens i.e. the result yields more then one token
        string_t concat_result;
        ContainerT rescanned;

            concat_result = ((*prev).get_value() + (*next).get_value());

        // Here we have to work around a conflict between the Standards 
        // requirement, that the preprocessor has to act upon so called 
        // pp-tokens and the fact, that Wave acts upon C++ tokens. So we have 
        // eventually to combine the current token with the next tokens
        // if these are of type T_IDENTIFIER or T_INTLIT following without any 
        // interventing whitespace.
        iterator_t save = next;
        
            if (IS_CATEGORY(*prev, IdentifierTokenType) && 
                T_INTLIT == token_id(*save)) 
            {
            token_id id = impl::next_token<iterator_t>::peek(next, end, false);
            
                if (IS_CATEGORY(id, IdentifierTokenType) || 
                    IS_CATEGORY(id, KeywordTokenType))
                {
                    concat_result += (*++next).get_value();
                }
            }

        // analyze the validity of the concatenation result
            if (is_invalid_concat(concat_result, (*prev).get_position(), 
                    rescanned) &&
                !IS_CATEGORY(*prev, WhiteSpaceTokenType) && 
                !IS_CATEGORY(*next, WhiteSpaceTokenType)) 
            {
            string_t error_string("\"");
            
                error_string += (*prev).get_value();
                error_string += "\" and \"";
                error_string += (*save).get_value();
                error_string += "\"";
                BOOST_WAVE_THROW(preprocess_exception, invalid_concat,
                    error_string, main_pos);
            }

#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
            if (boost::wave::need_cpp0x(ctx.get_language())) {
            // remove the prev, '##' and the next tokens from the sequence
                expanded.erase(prev, ++next);       // remove not needed tokens   

            // replace the old token (pointed to by *prev) with the retokenized
            // sequence
            typename ContainerT::reverse_iterator rit = rescanned.rbegin();

                BOOST_ASSERT(rit != rescanned.rend());
                rescanned.erase((++rit).base());
                expanded.splice(next, rescanned);

            // the last token of the inserted sequence is the new previous
                prev = next;
                --prev;
            }
            else
#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
            {
            // we leave the token_id unchanged, but unmark the token as 
            // disabled, if appropriate
                (*prev).set_value(concat_result);
                if (T_NONREPLACABLE_IDENTIFIER == token_id(*prev))
                    (*prev).set_token_id(T_IDENTIFIER);
                
            // remove the '##' and the next tokens from the sequence
            iterator_t first_to_delete = prev;

                expanded.erase(++first_to_delete, ++next); 
            }
            it = next;
            continue;
        }

    // save last non-whitespace token position
        if (!IS_CATEGORY(*it, WhiteSpaceTokenType))        
            prev = it;

        ++it;           // next token, please
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  experimental: macro scoping support
//
///////////////////////////////////////////////////////////////////////////////

namespace {

    ///////////////////////////////////////////////////////////////////////////
    //
    //  get the full name of a given macro name or scope name (concatenate the
    //  string representations of the single tokens).
    //
    template <typename IteratorT>
    inline std::string
    get_full_name(IteratorT const &begin, IteratorT const &end)
    {
    std::string full_name;
    
        for (IteratorT err_it = begin; err_it != end; ++err_it) 
            full_name += (*err_it).get_value().c_str();

        return full_name;
    }

    template <typename IteratorT>
    inline bool
    is_qualified_name(IteratorT const &begin, IteratorT const &end)
    {
//#if !defined(WAVE_USE_RE2C_IDL_LEXER)
    bool was_colon_colon = false;

        for (IteratorT it = begin; it != end; ++it) {
            if (token_id(*it) == T_COLON_COLON) {
                if (was_colon_colon) {
                // error: two consecutive '::'
                    return false;
                }
                was_colon_colon = true;
            }
            else if (T_IDENTIFIER == token_id(*it) || 
                IS_CATEGORY(*it, KeywordTokenType)) 
            {
                if (!was_colon_colon) {
                // error: missing '::' between two parts of the macro name
                    return false;
                }
                was_colon_colon = false;
            }
            else if (!IS_CATEGORY(*it, WhiteSpaceTokenType)) {
            // invalid token type inside a qualified name
                return false;
            }
        }
//#else
//        return false;   // always false for IDL mode
//#endif // !defined(WAVE_USE_RE2C_IDL_LEXER)
        return true;
    }
}   // namespace impl

#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0

///////////////////////////////////////////////////////////////////////////////
//
//  #region directive 
//
//    - syntax
//
//      #region [<identifier>]
//
//      Where <identifier> is a possibly qualified name defining the 
//      name of the region to open. This name is optional. If the name is 
//      omitted a nameless region is opened. Macros defined inside a nameless
//      region may not be accessed from outside this region.
//
//    - The #region directive is opaque for all macro definitions made outside
//      this region. A #region may be re-opened (i.e. a #region directive with
//      the same name is found at the same scope), and macros from the first
//      occurrence of this region will be visible.
//
//    - Region names and macro names of the same scope are stored into the same 
//      symbol table. I.e. at one scope there shall not be defined a region and 
//      a macro with the same name.
//
//    - #region's may be nested.
//
//    - The arguments of the #region directive are _not_ subject to macro 
//      expansion.
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
inline void 
macromap<ContextT>::open_scope(string_t const &name,
    defined_macros_t *current_scope, 
    boost::shared_ptr<defined_macros_t> &new_scope)
{
    bool is_new_scope = false;
    boost::shared_ptr<defined_macros_t> next_scope;
    if (!current_scope -> get_scope(name, next_scope))
        is_new_scope = true;

    new_scope = current_scope -> begin_scope(name, ++macro_uid);
    if (is_new_scope) {
    // make predefined macros available
        init_predefined_macros(new_scope.get(), false);
        
    // predefine the __CURRENT_SCOPE__ and __FULL_SCOPE__ names
        position_t pos;
        predefine_macro(new_scope.get(), 
            "__STDC_CURRENT_" BOOST_WAVE_PP_REGION_UC "__", 
            token_t(T_STRINGLIT, name, pos));
        predefine_macro(new_scope.get(),
            "__STDC_FULL_" BOOST_WAVE_PP_REGION_UC "__", 
            token_t(T_STRINGLIT, new_scope->get_full_name(), pos));
    }
}

template <typename ContextT>
inline void 
macromap<ContextT>::begin_unnamed_scope()
{
// create/open a new unnamed sub-region
boost::shared_ptr<defined_macros_t> new_scope;

    open_scope("", current_macros, new_scope);
    scopes.push(new_scope);
    current_macros = new_scope.get();
}

template <typename ContextT>
template <typename IteratorT>
inline void 
macromap<ContextT>::begin_scope(IteratorT const &begin, 
    IteratorT const &end)
{
    using namespace cpplexer;
    
defined_macros_t *current_scope = 0;
bool was_colon_colon = false;
bool found_qualified_name = false;
    
    for (IteratorT it = begin; it != end; /**/) {
        if (token_id(*it) == T_COLON_COLON) {
            if (was_colon_colon) {
            // error: two consecutive '::'
                BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                    get_full_name(begin, end), main_pos);
            }
            if (0 == current_scope) {
            // initialize starting scope (global scope)
                if (current_macros -> get_contains_unnamed_part()) {
                    BOOST_WAVE_THROW(preprocess_exception, illegal_global_region,
                        get_full_name(begin, end), main_pos);
                }
                if (++it == end) {
                    scopes.push(defined_macros);    // store opened region
                    current_macros = defined_macros.get();
                    found_qualified_name = true;
                    break;
                }
                else {
                    current_scope = defined_macros.get();
                }
            } 
            else {
                ++it;
            }
            was_colon_colon = true;
        }
        else {
            if (0 == current_scope) {
            // initialize starting scope
                current_scope = current_macros;     // start at current scope
            }
            else if (!was_colon_colon) {
            // error: missing '::' between two parts of the macro name
                BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                    get_full_name(begin, end), main_pos);
            }
            was_colon_colon = false;
            
        // open the given sub-region (create it if necessary)
        string_t name ((*it).get_value());
        boost::shared_ptr<defined_macros_t> next_scope;

            open_scope(name, current_scope, next_scope);
            if (++it == end) {
                scopes.push(next_scope);            // store opened region
                current_macros = next_scope.get();
                found_qualified_name = true;
                break;
            }
            else {
            // analyze the next part of the qualified name
                current_scope = next_scope.get();
            }
        }
    }
    
    if (!found_qualified_name) {
    // missing last part of the name (scope end with '::')
        BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
            get_full_name(begin, end), main_pos);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  #endregion
//
//    - syntax
//
//      #endregion
//
//    - The #endregion ends the last opened macro region. It is directive 
//      opaque for all macros defined inside the closed region. Macros from 
//      this region may be accessed outside this region only if imported 
//      (see #import) or if used as qualified names specifying the region and 
//      the macro name.
//
///////////////////////////////////////////////////////////////////////////////
template <typename ContextT>
inline void 
macromap<ContextT>::end_scope()
{
    if (0 == scopes.size()) {
    // should not be used at global scope
        BOOST_WAVE_THROW(preprocess_exception, unexpected_endregion, "", main_pos);
    }
    
// inform the symbol table
    current_macros->end_scope();

// restore the previous scope
    scopes.pop();
    if (scopes.size() > 0)
        current_macros = scopes.top().get();
    else
        current_macros = defined_macros.get();
}

///////////////////////////////////////////////////////////////////////////////
//
//  #import directive
//
//    - syntax
//
//      #import <qualified-name> [, <qualified-name>]
//
//      Where <qualified-name> is a macro name or region name consisting out 
//      of a possibly empty sequence of region names interspersed with '::' 
//      optionally tokens followed by a macro name defined inside the 
//      specified region.
//      
//      If <qualified-name> refers to a macro, then the referenced macro 
//      definition is _copied_ into the current region, just if it were defined 
//      here initially.
//
//      If <qualified-name> refers to a region, then
//      - all macro definitions of the referenced region are _copied_ into the
//        current region, just if these were defined here initially.
//      - all sub-regions of the referenced region are made available from the
//        current region, just if these were defined as direct sub-regions of 
//        the current region.
//
//      Imported macros may be undefined with #undef as usual. This removes
//      the referenced macro from the current region, but leaves it unchanged 
//      in the original region.
//
//    - The arguments of the #import directive are _not_ subject to macro 
//      expansion.
//
///////////////////////////////////////////////////////////////////////////////

template <typename ContextT>
template <typename IteratorT>
inline void 
macromap<ContextT>::import_name(IteratorT const &begin, 
    IteratorT const &end)
{
    using namespace cpplexer;
    defined_macros_t *current_scope = 0;
    bool was_colon_colon = false;
    
    for (IteratorT it = begin; it != end; /**/) {
        if (token_id(*it) == T_COLON_COLON) {
            if (was_colon_colon) {
            // error: two consecutive '::'
                BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                    get_full_name(begin, end), main_pos);
            }
            if (0 == current_scope) {
            // initialize starting scope (global scope)
                if (++it == end) {
                // '#import ::' specified
                    current_macros -> import_scope(defined_macros.get(), 
                        main_pos); 
                    return;     // imported scope
                }
                current_scope = defined_macros.get();
            }
            else {
                ++it;
            }
            was_colon_colon = true;
        }
        else {
            if (0 == current_scope) {
            // initialize starting scope
                current_scope = current_macros;     // start at current scope
            }
            else if (!was_colon_colon) {
            // error: missing '::' between two parts of the macro name
                BOOST_WAVE_THROW(preprocess_exception, invalid_macroname, 
                    get_full_name(begin, end), main_pos);
            }
            was_colon_colon = false;
            
        string_t name ((*it).get_value());
        boost::shared_ptr<defined_macros_t> next_scope;
        typename defined_macros_t::iterator cit( 
            current_scope -> find(name));
        
            if (cit != current_scope -> end()) {
            // defined as a macro name
            
            // should be the last given name part 
                if (++it != end) {
                    BOOST_WAVE_THROW(preprocess_exception, undefined_macroname, 
                        get_full_name(begin, end), main_pos);
                }

            // import this macro name
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
                if (!current_macros -> import_macroname(name, (*cit).second)) {
#else
                if (!current_macros -> import_macroname(name, (*cit).data())) {
#endif 
                // this macro name is already used as a scope name here
                    BOOST_WAVE_THROW(preprocess_exception, alreadydefined_name, 
                        name, main_pos);
                }
                return;     // imported macro
            }
            else if (current_scope -> get_scope(name, next_scope)) {
            // if this part was the last one, then import the whole scope
                if (++it == end) {
                    current_macros -> import_scope(next_scope.get(), main_pos); 
                    return;     // imported scope
                }
                else {
                // defined as a scope name
                    current_scope = next_scope.get();
                }                
            }
            else {
            // name is not defined
                BOOST_WAVE_THROW(preprocess_exception, undefined_macroname, 
                    get_full_name(begin, end), main_pos);
            }
        }
    }
}
#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0

///////////////////////////////////////////////////////////////////////////////
//
//  init_predefined_macros(): init the predefined macros
//
///////////////////////////////////////////////////////////////////////////////

namespace predefined_macros {

// list of static predefined macros
    struct static_macros {
        char const *name;
        boost::wave::token_id token_id;
        char const *value;
    };

// C++ mode 
    inline static_macros const & 
    static_data_cpp(std::size_t i)
    {
    static static_macros data[] = {
            { "__STDC__", T_INTLIT, "1" },
            { "__cplusplus", T_INTLIT, "199711L" },
            { 0, T_EOF, 0 }
        }; 
        BOOST_ASSERT(i < sizeof(data)/sizeof(data[0]));
        return data[i];
    }

#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
// C99 mode
    inline static_macros const &
    static_data_c99(std::size_t i)
    {
    static static_macros data[] = {
            { "__STDC__", T_INTLIT, "1" },
            { "__STDC_VERSION__", T_INTLIT, "199901L" },
            { "__STDC_HOSTED__", T_INTLIT, "0" },
            { "__WAVE_HAS_VARIADICS__", T_INTLIT, "1" },
            { 0, T_EOF, 0 }
        }; 
        BOOST_ASSERT(i < sizeof(data)/sizeof(data[0]));
        return data[i];
    }
    
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
// C++0x mode 
    inline static_macros const &
    static_data_cpp0x(std::size_t i)
    {
    static static_macros data[] = {
            { "__STDC__", T_INTLIT, "1" },
            { "__cplusplus", T_INTLIT, "200304L" },
            { "__WAVE_HAS_VARIADICS__", T_INTLIT, "1" },
            { 0, T_EOF, 0 }
        }; 
        BOOST_ASSERT(i < sizeof(data)/sizeof(data[0]));
        return data[i];
    }
#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
#endif // BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
    
// list of dynamic predefined macros
    typedef char const * (* get_dynamic_value)(bool);

// __DATE__ 
    inline 
    char const *get_date(bool reset)
    {
    static std::string datestr;
    static const char *const monthnames[] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };
    
        if (reset) {
            datestr.erase();
        }
        else {
        // for some systems sprintf, time_t etc. is in namespace std
            using namespace std;    

        time_t tt = time(0);
        struct tm *tb = 0;

            if (tt != (time_t)-1) {
            char buffer[sizeof("\"Oct 11 1347\"")+1];

                tb = localtime (&tt);
                sprintf (buffer, "\"%s %2d %4d\"",
                    monthnames[tb->tm_mon], tb->tm_mday, tb->tm_year + 1900);
                datestr = buffer;
            }
            else {
                datestr = "\"??? ?? ????\"";
            }
        }
        return datestr.c_str();
    }

// __TIME__
    inline 
    char const *get_time(bool reset)
    {
    static std::string timestr;
    
        if (reset) {
            timestr.erase();
        }
        else {
        // for some systems sprintf, time_t etc. is in namespace std
            using namespace std;    

        time_t tt = time(0);
        struct tm *tb = 0;

            if (tt != (time_t)-1) {
            char buffer[sizeof("\"12:34:56\"")+1];

                tb = localtime (&tt);
                sprintf (buffer, "\"%02d:%02d:%02d\"", tb->tm_hour, 
                    tb->tm_min, tb->tm_sec);
                timestr = buffer;
            }
            else {
                timestr = "\"??:??:??\"";
            }
        }
        return timestr.c_str();
    }

// __SPIRIT_PP__/__WAVE__
    inline 
    char const *get_version(bool /*reset*/)
    {
    static std::string versionstr;
    char buffer[sizeof("0x0000")+1];

        using namespace std;    // for some systems sprintf is in namespace std
        sprintf(buffer, "0x%02d%1d%1d", BOOST_WAVE_VERSION_MAJOR, 
            BOOST_WAVE_VERSION_MINOR, BOOST_WAVE_VERSION_SUBMINOR);
        versionstr = buffer;
        return versionstr.c_str();
    }
    
// __SPIRIT_PP_VERSION__/__WAVE_VERSION__
    boost::wave::util::time_conversion_helper const 
        compilation_time(__DATE__ " " __TIME__);
        
    inline 
    char const *get_fullversion(bool /*reset*/)
    {
    static std::string versionstr;
    char buffer[sizeof("0x00000000")+1];

    // for some systems sprintf, time_t etc. is in namespace std
        using namespace std;    

    // calculate the number of days since Dec 13 2001 
    // (the day the Wave project was started)
    tm first_day;

        using namespace std;    // for some systems memset is in namespace std
        memset (&first_day, 0, sizeof(tm));
        first_day.tm_mon = 11;           // Dec
        first_day.tm_mday = 13;          // 13
        first_day.tm_year = 101;         // 2001

    long seconds = long(difftime(compilation_time.get_time(), 
        mktime(&first_day)));

        sprintf(buffer, "0x%02d%1d%1d%04ld", BOOST_WAVE_VERSION_MAJOR,
             BOOST_WAVE_VERSION_MINOR, BOOST_WAVE_VERSION_SUBMINOR, 
             seconds/(3600*24));
        versionstr = buffer;
        return versionstr.c_str();
    }
    
// __SPIRIT_PP_VERSION_STR__/__WAVE_VERSION_STR__
    inline 
    char const *get_versionstr(bool /*reset*/)
    {
    static std::string versionstr;
    char buffer[sizeof("\"00.00.00.0000\"")+1];

    // for some systems sprintf, time_t etc. is in namespace std
        using namespace std;    

    // calculate the number of days since Dec 13 2001 
    // (the day the Wave project was started)
    tm first_day;

        using namespace std;    // for some systems memset is in namespace std
        memset (&first_day, 0, sizeof(tm));
        first_day.tm_mon = 11;           // Dec
        first_day.tm_mday = 13;          // 13
        first_day.tm_year = 101;         // 2001

    long seconds = long(difftime(compilation_time.get_time(), 
        mktime(&first_day)));

        sprintf(buffer, "\"%d.%d.%d.%ld\"", BOOST_WAVE_VERSION_MAJOR,
             BOOST_WAVE_VERSION_MINOR, BOOST_WAVE_VERSION_SUBMINOR, 
             seconds/(3600*24));
        versionstr = buffer;
        return versionstr.c_str();
    }
    
    struct dynamic_macros {
        char const *name;
        boost::wave::token_id token_id;
        get_dynamic_value generator;
    };
    
    inline dynamic_macros const &
    dynamic_data(std::size_t i)
    {
    static dynamic_macros data[] = {
            { "__DATE__", T_STRINGLIT, get_date },
            { "__TIME__", T_STRINGLIT, get_time },
            { "__SPIRIT_PP__", T_INTLIT, get_version },
            { "__SPIRIT_PP_VERSION__", T_INTLIT, get_fullversion },
            { "__SPIRIT_PP_VERSION_STR__", T_STRINGLIT, get_versionstr },
            { "__WAVE__", T_INTLIT, get_version },
            { "__WAVE_VERSION__", T_INTLIT, get_fullversion },
            { "__WAVE_VERSION_STR__", T_STRINGLIT, get_versionstr },
            { 0, T_EOF, 0 }
        };
        BOOST_ASSERT(i < sizeof(data)/sizeof(data[0]));
        return data[i];
    }
    
}   // namespace predefined_macros

template <typename ContextT>
inline void
macromap<ContextT>::predefine_macro(defined_macros_t *scope, 
    string_t const &name, token_t const &t)
{
definition_container_t macrodefinition;
std::vector<token_t> param;

    macrodefinition.push_back(t);
    add_macro(token_t(T_IDENTIFIER, name, t.get_position()), 
        false, param, macrodefinition, true, scope);
}

template <typename ContextT>
inline void 
macromap<ContextT>::init_predefined_macros(defined_macros_t *scope,
    bool at_global_scope)
{
    using namespace predefined_macros;

// if no scope is given, use the current one
defined_macros_t *current_scope = scope ? scope : current_macros;

// first, add the static macros
position_t pos;

#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
    if (boost::wave::need_c99(ctx.get_language())) {
    // define C99 specifics
        for (int i = 0; 0 != static_data_c99(i).name; ++i) {
            predefine_macro(current_scope, static_data_c99(i).name,
                token_t(static_data_c99(i).token_id, 
                    static_data_c99(i).value, pos));
        }
    }
    else 
#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
    if (boost::wave::need_cpp0x(ctx.get_language())) {
    // define C++0x specifics
        for (int i = 0; 0 != static_data_cpp0x(i).name; ++i) {
            predefine_macro(current_scope, static_data_cpp0x(i).name, 
                token_t(static_data_cpp0x(i).token_id, 
                    static_data_cpp0x(i).value, pos));
        }

    // add the global scope definitions
        if (at_global_scope) {
        position_t pos;

            predefine_macro(current_scope, "__STDC_GLOBAL__", 
                token_t(T_INTLIT, "1", pos));
            predefine_macro(current_scope, 
                "__STDC_CURRENT_" BOOST_WAVE_PP_REGION_UC "__",  
                token_t(T_STRINGLIT, "", pos));
            predefine_macro(current_scope, 
                "__STDC_FULL_" BOOST_WAVE_PP_REGION_UC "__", 
                token_t(T_STRINGLIT, "::", pos));
        }
    }
    else
#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS != 0
#endif // BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
    {
    // define C++ specifics
        for (int i = 0; 0 != static_data_cpp(i).name; ++i) {
            predefine_macro(current_scope, static_data_cpp(i).name, 
                token_t(static_data_cpp(i).token_id, 
                    static_data_cpp(i).value, pos));
        }
        
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
    // define __WAVE_HAS_VARIADICS__, if appropriate
        if (boost::wave::need_variadics(ctx.get_language())) {
            predefine_macro(current_scope, "__WAVE_HAS_VARIADICS__",
                token_t(T_INTLIT, "1", pos));
        }
#endif 
    }
    
// now add the dynamic macros
    for (int j = 0; 0 != dynamic_data(j).name; ++j) {
        predefine_macro(current_scope, dynamic_data(j).name,
            token_t(dynamic_data(j).token_id, 
                dynamic_data(j).generator(false), pos));
    }
}

template <typename ContextT>
inline void 
macromap<ContextT>::reset_macromap()
{
    current_macros->clear();
    predefined_macros::get_time(true);
    predefined_macros::get_date(true);
    act_token = token_t();
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace wave
}   // namespace boost

#endif // !defined(CPP_MACROMAP_HPP_CB8F51B0_A3F0_411C_AEF4_6FF631B8B414_INCLUDED)
