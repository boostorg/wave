/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    http://spirit.sourceforge.net/

    Copyright (c) 2001-2004 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(TRACE_POLICIES_HPP_338DE478_A13C_4B63_9BA9_041C917793B8_INCLUDED)
#define TRACE_POLICIES_HPP_338DE478_A13C_4B63_9BA9_041C917793B8_INCLUDED

#include <vector>

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace trace_policies {

///////////////////////////////////////////////////////////////////////////////
//  
//  trace_flags:  enable single tracing functionality
//  
///////////////////////////////////////////////////////////////////////////////
enum trace_flags {
    trace_nothing = 0,      // disable tracing
    trace_macros = 1,       // enable macro tracing
    trace_includes = 2      // enable include file tracing
};

///////////////////////////////////////////////////////////////////////////////
//  
//  The default_tracing class is a placeholder for all macro 
//  expansion trace hooks contained inside the macro expansion engine
//
///////////////////////////////////////////////////////////////////////////////
struct default_tracing {

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function enable_tracing is called, whenever the status of the 
    //  tracing was changed.
    //
    //  The parameter 'enable' is to be used as the new tracing status.
    //  
    ///////////////////////////////////////////////////////////////////////////
    void enable_tracing(trace_flags flags)
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function tracing_enabled should return the current tracing status.
    //  
    ///////////////////////////////////////////////////////////////////////////
    trace_flags tracing_enabled() 
    { return trace_nothing; }
    
    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanding_function_like_macro' is called, whenever a 
    //  function-like macro is to be expanded.
    //
    //  The macroname parameter marks the position, where the macro to expand 
    //  is defined.
    //  The formal_args parameter holds the formal arguments used during the
    //  definition of the macro.
    //  The definition parameter holds the macro definition for the macro to 
    //  trace.
    //
    //  The macro call parameter marks the position, where this macro invoked.
    //  The arguments parameter holds the macro arguments used during the 
    //  invocation of the macro
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename TokenT, typename ContainerT>
    void expanding_function_like_macro(
        TokenT const &macrodef, std::vector<TokenT> const &formal_args, 
        ContainerT const &definition,
        TokenT const &macrocall, std::vector<ContainerT> const &arguments) 
    {}

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanding_object_like_macro' is called, whenever a 
    //  object-like macro is to be expanded .
    //
    //  The macroname parameter marks the position, where the macro to expand 
    //  is defined.
    //  The definition parameter holds the macro definition for the macro to 
    //  trace.
    //
    //  The macro call parameter marks the position, where this macro invoked.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename TokenT, typename ContainerT>
    void expanding_object_like_macro(TokenT const &macro, 
        ContainerT const &definition, TokenT const &macrocall)
    {}

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanded_macro' is called, whenever the expansion of a 
    //  macro is finished but before the rescanning process starts.
    //
    //  The parameter 'result' contains the token sequence generated as the 
    //  result of the macro expansion.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContainerT>
    void expanded_macro(ContainerT const &result)
    {}

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'rescanned_macro' is called, whenever the rescanning of a 
    //  macro is finished.
    //
    //  The parameter 'result' contains the token sequence generated as the 
    //  result of the rescanning.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename ContainerT>
    void rescanned_macro(ContainerT const &result)
    {}

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'opened_include_file' is called, whenever a file referred 
    //  by an #include directive was successfully located and opened.
    //
    //  The parameter 'filename' contains the full file system path of the 
    //  opened file.
    //
    //  The include_depth parameter contains the current include file depth.
    //
    //  The is_system_include parameter denotes, if the given file was found
    //  as a result of a #include <...> directive.
    //  
    ///////////////////////////////////////////////////////////////////////////
    void 
    opened_include_file(std::string const &filename, std::size_t include_depth,
        bool is_system_include) 
    {}
    
    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'returning_from_include_file' is called, whenever an
    //  included file is about to be closed after it's processing is complete.
    //
    ///////////////////////////////////////////////////////////////////////////
    void
    returning_from_include_file() 
    {}

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'interpret_pragma' is called, whenever a #pragma wave 
    //  directive is found, which isn't known to the core Wave library. 
    //
    //  The parameter 'pending' may be used to push tokens back into the input 
    //  stream, which are to be used as the replacement text for the whole 
    //  #pragma wave() directive.
    //
    //  The parameter 'option' contains the name of the interpreted pragma.
    //
    //  The parameter 'values' holds the values of the parameter provided to 
    //  the pragma operator.
    //
    //  The parameter 'act_token' contains the actual #pragma token, which may 
    //  be used for error output.
    //
    //  The parameter 'language' contains the current language mode, in which 
    //  the Wave library operates.
    //
    //  If the return value is 'false', the whole #pragma directive is 
    //  interpreted as unknown and a corresponding error message is issued. A
    //  return value of 'true' signs a successful interpretation of the given 
    //  #pragma.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename TokenT, typename ContainerT>
    bool 
    interpret_pragma(ContainerT &pending, TokenT const &option, 
        ContainerT const &values, TokenT const &act_token, 
        boost::wave::language_support language)
    {
        return false;
    }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace trace_policies
}   // namespace wave
}   // namespace boost

#endif // !defined(TRACE_POLICIES_HPP_338DE478_A13C_4B63_9BA9_041C917793B8_INCLUDED)
