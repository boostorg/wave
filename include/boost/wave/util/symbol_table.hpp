/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(SYMBOL_TABLE_HPP_32B0F7C6_3DD6_4113_95A5_E16516C6F45A_INCLUDED)
#define SYMBOL_TABLE_HPP_32B0F7C6_3DD6_4113_95A5_E16516C6F45A_INCLUDED

#include <map>
#include <algorithm>

#include <boost/shared_ptr.hpp>
#include <boost/wave/wave_config.hpp>

#if BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS == 0

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace util {

///////////////////////////////////////////////////////////////////////////////
//
//  The symbol_table class is used for the storage of defined macros. 
//
///////////////////////////////////////////////////////////////////////////////

template <typename StringT, typename MacroDefT>
struct symbol_table 
:   public std::map<StringT, boost::shared_ptr<MacroDefT> > 
{
    symbol_table(long uid_) 
    {}
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace wave
}   // namespace boost

#else  // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS == 0

#include <utility>
#include <algorithm>

#include <boost/concept_check.hpp>

//#define BOOST_WAVE_USE_TST_SYMBOLTABLE

#if defined(BOOST_WAVE_USE_TST_SYMBOLTABLE)
#include <boost/wave/util/tst.hpp>
#endif // defined(WAVE_USE_TST_SYMBOLTABLE)

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace util {

namespace {

    template <typename StringT>
    inline StringT 
    make_name(StringT const &name)
    {
        return !name.empty() ? name : "<unnamed>";
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  The symbol_table class is used for structured storage of defined macros. It
//  supports macro scoping.
//
///////////////////////////////////////////////////////////////////////////////

template <typename StringT, typename MacroDefT>
class symbol_table {

    typedef symbol_table<StringT, MacroDefT>            self_t;
    typedef boost::shared_ptr<self_t>                   self_ref_t;
    typedef boost::shared_ptr<MacroDefT>                macro_ref_t;
    typedef std::map<StringT, self_ref_t>               enclosed_scopes_t;

#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
    typedef std::map<StringT, macro_ref_t>              defined_macros_t;
#else
    typedef boost::spirit::tst<macro_ref_t, StringT>    defined_macros_t;
#endif
    
public:
    typedef typename defined_macros_t::value_type       value_type;
    typedef typename defined_macros_t::iterator         iterator;
    typedef typename defined_macros_t::const_iterator   const_iterator;

    typedef typename enclosed_scopes_t::value_type      scope_type;
    typedef typename enclosed_scopes_t::iterator        scope_iterator;
    typedef typename enclosed_scopes_t::const_iterator  const_scope_iterator;

public:
// constructs global scope symbol table
    symbol_table(long uid_) 
    :   contains_unnamed_part(false), uid(uid_)
    {}
    
// constructs any non-global scope symbol table
    symbol_table(StringT const &scope_name_, StringT const &full_outer_name_,
        bool contains_unnamed_part_, long uid_) 
    :   scope_name(scope_name_), 
        full_scope_name(full_outer_name_ + "::" + make_name(scope_name_)),
        contains_unnamed_part(contains_unnamed_part_),
        uid(uid_)
    {}
    // generated copy constructor
    // generated destructor
    // generated assignment operator

// symbol table operations
    iterator find(StringT const &macro_name)
        { return macro_names.find(macro_name); }
    const_iterator find(StringT const &macro_name) const
        { return macro_names.find(macro_name); }

    iterator begin() { return macro_names.begin(); }
    const_iterator begin() const { return macro_names.begin(); }
    iterator end() { return macro_names.end(); }
    const_iterator end() const { return macro_names.end(); }

#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
    std::pair<iterator, bool> insert(value_type const &value)
        { return macro_names.insert(value); }
    void erase(iterator where) { macro_names.erase(where); }
#else
    std::pair<iterator, bool> insert(StringT const &value)
        { return macro_names.insert(value); }
    void erase(iterator where) 
        { macro_names.erase((*where).data()->macroname.get_value()); }
#endif

    void clear() { macro_names.clear(); }

// scope manipulation
    self_ref_t begin_scope(StringT const &name, long uid);
    self_t *end_scope() const;
    bool get_scope(StringT const &name, self_ref_t &scope) const;

    bool get_contains_unnamed_part() const { return contains_unnamed_part; }
    
    scope_iterator scope_begin() { return scopes.begin(); }
    const_scope_iterator scope_begin() const { return scopes.begin(); }
    scope_iterator scope_end() { return scopes.end(); }
    const_scope_iterator scope_end() const { return scopes.end(); }
    
// import a given macro name into this scope
    bool import_macroname (StringT const &name, macro_ref_t const &value);
    
// import a given scope into this scope
    template <typename PositionT>
    void import_scope (self_t *value, PositionT const &act_pos);
    
    StringT const &get_full_name() const { return full_scope_name; }
    
private:
    defined_macros_t macro_names;   // macros defined inside this scope
    StringT scope_name;             // name of this scope (empty for global)
    StringT full_scope_name;        // full name of this scope ('::' for global)
    enclosed_scopes_t scopes;       // scopes defined inside this scope
    bool contains_unnamed_part;     // name contains unnamed part(s)
    long uid;
};

///////////////////////////////////////////////////////////////////////////////
//  
//  begin_scope
//
//      This function opens a new scope with the given name inside the current 
//      scope. If this scope already exists, the function does nothing.
//      The newly created or existing scopes are returned.
//
///////////////////////////////////////////////////////////////////////////////
template <typename StringT, typename MacroDefT>
inline boost::shared_ptr<symbol_table<StringT, MacroDefT> >
symbol_table<StringT, MacroDefT>::begin_scope(StringT const &name, long uid)
{
// if this scope does not exist, create it, otherwise reuse the existing 
scope_iterator it = scopes.find(name);

    if (it == scopes.end()) {
    // create the new scope
    self_ref_t new_symbol_table(new self_t(name, full_scope_name, 
        contains_unnamed_part || name.empty(), uid));
    std::pair<scope_iterator, bool> p = scopes.insert(
        scope_type(name, new_symbol_table));

        BOOST_SPIRIT_ASSERT(p.second);
        it = p.first;
    }
    
// return a pointer to the new scope
    return (*it).second;
}

///////////////////////////////////////////////////////////////////////////////
//
//  end_scope
//
//      This function closes (ends) the currently active scope
//      The next outer scope is returned
//
///////////////////////////////////////////////////////////////////////////////
template <typename StringT, typename MacroDefT>
inline symbol_table<StringT, MacroDefT> *
symbol_table<StringT, MacroDefT>::end_scope() const
{
    return 0;   // not used anymore
}

///////////////////////////////////////////////////////////////////////////////
//
//  get_scope
//
//      This function returns a scope with a given name, if this scope exists. 
//      The return value of this function indicates whether a scope with the
//      given name was found or not.
//
///////////////////////////////////////////////////////////////////////////////
template <typename StringT, typename MacroDefT>
inline bool
symbol_table<StringT, MacroDefT>::get_scope(StringT const &name, 
    boost::shared_ptr<symbol_table<StringT, MacroDefT> > &scope) const
{
const_scope_iterator cit = scopes.find(name);

    if (cit == scopes.end()) 
        return false;

    scope = (*cit).second;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// 
//   import_macroname
//
//      Import a given macro name into this scope
//
//      The name parameter should be an unqualified macro name.
//
///////////////////////////////////////////////////////////////////////////////
template <typename StringT, typename MacroDefT>
inline bool
symbol_table<StringT, MacroDefT>::import_macroname (StringT const &name, 
    macro_ref_t const &value)
{
// the new name shouldn't exist in this scope
iterator it = find(name);

    if (it != end()) {
    // name already defined in this scope as a macro, may only be imported
    // if it is the same macro
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
        return ((*it).second->uid == value->uid);
#else
        return ((*it).data()->uid == value->uid);
#endif
    }
        
const_scope_iterator its = scopes.find(name);

    if (its != scopes.end()) {
    // name already defined in this scope as a scope, may not be imported
        return false;
    }

#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
std::pair<iterator, bool> p = insert(value_type(name, value));

    BOOST_SPIRIT_ASSERT(p.second);
    boost::ignore_unused_variable_warning(p);
#else
std::pair<iterator, bool> p = insert(name);

    BOOST_SPIRIT_ASSERT(p.second);
    (*p.first).data() = value;
#endif
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//  
//  import_scope
//
//      Import a given scope into this scope. This effectively makes available 
//      the whole scope hierarchy based at the scope to import into this scope.
//
//      The name parameter should be an unqualified scope name.
//
///////////////////////////////////////////////////////////////////////////////
template <typename StringT, typename MacroDefT>
template <typename PositionT>
inline void
symbol_table<StringT, MacroDefT>::import_scope (self_t *value, 
    PositionT const &act_pos)
{
// import all macro names from the given scope
    iterator end = value -> end();
    for (iterator it = value -> begin(); it != end; ++it) {
#if BOOST_WAVE_USE_TST_SYMBOLTABLE == 0
        if ((*it).second->is_predefined)
            continue;   // do not import predefined macros

        if (!import_macroname((*it).first, (*it).second)) {
            BOOST_WAVE_THROW(preprocess_exception, alreadydefined_name, 
                (*it).first, act_pos);
        }
#else
        if ((*it).data()->is_predefined)
            continue;   // do not import predefined macros

        if (!import_macroname((*it).key(), (*it).data())) {
            BOOST_WAVE_THROW(preprocess_exception, alreadydefined_name, 
                (*it).key(), act_pos);
        }
#endif 
    }
    
// import all child scopes of the given one into this scope
    const_scope_iterator scope_end = value -> scope_end();
    for (const_scope_iterator scope_it = value -> scope_begin();
         scope_it != scope_end; ++scope_it)
    {
    StringT name ((*scope_it).first);
    
    // the new name shouldn't exist in this scope
    iterator itm = find(name);

        if (itm != this->end()) {
        // name already defined in this scope as a macro, may not be imported
            BOOST_WAVE_THROW(preprocess_exception, alreadydefined_name, 
                name, act_pos);
        }
        
    scope_iterator its = scopes.find(name);

        if (its != scopes.end()) {
            if ((*its).second->uid != (*scope_it).second->uid) {
            // name already defined in this scope as a scope, may not be imported
                BOOST_WAVE_THROW(preprocess_exception, alreadydefined_name, 
                    name, act_pos);
            }
            continue;   // this scope was imported already
        }

    // if this scope does not exist, import it
    std::pair<scope_iterator, bool> p = scopes.insert(
        scope_type(name, (*scope_it).second));

        BOOST_SPIRIT_ASSERT(p.second);
        boost::ignore_unused_variable_warning(p);
    }
}
    
///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace wave
}   // namespace boost

#endif // BOOST_WAVE_ENABLE_CPP0X_EXTENSIONS == 0

#endif // !defined(SYMBOL_TABLE_HPP_32B0F7C6_3DD6_4113_95A5_E16516C6F45A_INCLUDED)
