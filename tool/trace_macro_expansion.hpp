/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library
    http://spirit.sourceforge.net/

    Copyright (c) 2001-2004 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(TRACE_MACRO_EXPANSION_HPP_D8469318_8407_4B9D_A19F_13CA60C1661F_INCLUDED)
#define TRACE_MACRO_EXPANSION_HPP_D8469318_8407_4B9D_A19F_13CA60C1661F_INCLUDED

#include <ostream>
#include <string>

#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/timer.hpp>

#include <boost/wave/token_ids.hpp>
#include <boost/wave/util/macro_helpers.hpp>
#include <boost/wave/trace_policies.hpp>
#include <boost/wave/language_support.hpp>

#ifdef BOOST_NO_STRINGSTREAM
#include <strstream>
#define BOOST_WAVE_OSSTREAM std::ostrstream
std::string BOOST_WAVE_GETSTRING(std::ostrstream& ss)
{
    ss << ends;
    std::string rval = ss.str();
    ss.freeze(false);
    return rval;
}
#else
#include <sstream>
#define BOOST_WAVE_GETSTRING(ss) ss.str()
#define BOOST_WAVE_OSSTREAM std::ostringstream
#endif

///////////////////////////////////////////////////////////////////////////////
//  
class stop_watch : public boost::timer {

    typedef boost::timer base_t;
    
public:
    stop_watch() : is_suspended_since(0), suspended_overall(0) {}

    void suspend()
    {
        if (0 == is_suspended_since) {
        // if not already suspended
            is_suspended_since = this->base_t::elapsed();
        }
    }
    void resume()
    {
        if (0 != is_suspended_since) {
        // if really suspended
            suspended_overall += this->base_t::elapsed() - is_suspended_since;
            is_suspended_since = 0;
        }
    }
    double elapsed() const
    {
        if (0 == is_suspended_since) {
        // currently running
            return this->base_t::elapsed() - suspended_overall;
        }

    // currently suspended
        BOOST_ASSERT(is_suspended_since >= suspended_overall);
        return is_suspended_since - suspended_overall;
    }
    
    std::string format_elapsed_time() const
    {
    double current = elapsed();
    char time_buffer[sizeof("1234:56:78.90 abcd.")+1];

        using namespace std;
        if (current >= 3600) {
        // show hours
            sprintf (time_buffer, "%d:%02d:%02d.%03d hrs.",
                (int)(current) / 3600, ((int)(current) % 3600) / 60,
                ((int)(current) % 3600) % 60, 
                (int)(current * 1000) % 1000);
        }
        else if (current >= 60) {
        // show minutes
            sprintf (time_buffer, "%d:%02d.%03d min.", 
                (int)(current) / 60, (int)(current) % 60, 
                (int)(current * 1000) % 1000);
        }
        else {
        // show seconds
            sprintf(time_buffer, "%d.%03d sec.", (int)current, 
                (int)(current * 1000) % 1000);
        }
        return time_buffer;
    }
    
private:
    double is_suspended_since;
    double suspended_overall; 
};


///////////////////////////////////////////////////////////////////////////////
//  
//  The trace_macro_expansion policy is used to trace the macro expansion of
//  macros whenever it is requested from inside the input stream to preprocess
//  through the '#pragma wave_option(trace: enable)' directive. The macro 
//  tracing is disabled with the help of a '#pragma wave_option(trace: disable)'
//  directive.
//
//  This policy type is used as a template parameter to the boost::wave::context<>
//  object.
//
///////////////////////////////////////////////////////////////////////////////
class trace_macro_expansion
:   public boost::wave::trace_policies::default_tracing
{
public:
    trace_macro_expansion(std::ostream &outstrm_, 
            boost::wave::trace_policies::trace_flags flags_)
    :   outstrm(outstrm_), level(0), 
        flags(flags_), logging_flags(boost::wave::trace_policies::trace_nothing)
    {
    }
    ~trace_macro_expansion()
    {
    }
    
    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function enable_tracing is called, whenever the status of the 
    //  tracing was changed.
    //
    //  The parameter 'enable' is to be used as the new tracing status.
    //  
    ///////////////////////////////////////////////////////////////////////////
    void enable_tracing(boost::wave::trace_policies::trace_flags flags) 
    { logging_flags = flags; }

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function tracing_enabled should return the current tracing status.
    //  
    ///////////////////////////////////////////////////////////////////////////
    boost::wave::trace_policies::trace_flags tracing_enabled() 
    { return logging_flags; }

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanding_function_like_macro' is called, whenever a 
    //  function-like macro is to be expanded.
    //
    //  The 'macrodef' parameter marks the position, where the macro to expand 
    //  is defined.
    //  The 'formal_args' parameter holds the formal arguments used during the
    //  definition of the macro.
    //  The 'definition' parameter holds the macro definition for the macro to 
    //  trace.
    //
    //  The 'macrocall' parameter marks the position, where this macro invoked.
    //  The 'arguments' parameter holds the macro arguments used during the 
    //  invocation of the macro
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename TokenT, typename ContainerT>
    void expanding_function_like_macro(
        TokenT const &macrodef, std::vector<TokenT> const &formal_args, 
        ContainerT const &definition,
        TokenT const &macrocall, std::vector<ContainerT> const &arguments) 
    {
        if (!enabled_macro_tracing()) return;
        
        if (0 == get_level()) {
        // output header line
        BOOST_WAVE_OSSTREAM stream;

            stream 
                << macrocall.get_position() << ": "
                << macrocall.get_value() << "(";

        // argument list
            for (typename ContainerT::size_type i = 0; i < arguments.size(); ++i) {
                stream << boost::wave::util::impl::as_string(arguments[i]);
                if (i < arguments.size()-1)
                    stream << ", ";
            }
            stream << ")" << std::endl; 
            output(BOOST_WAVE_GETSTRING(stream));
            increment_level();
        }        
        
    // output definition reference
        {
        BOOST_WAVE_OSSTREAM stream;

            stream 
                << macrodef.get_position() << ": see macro definition: "
                << macrodef.get_value() << "(";

        // formal argument list
            for (typename std::vector<TokenT>::size_type i = 0; 
                i < formal_args.size(); ++i) 
            {
                stream << formal_args[i].get_value();
                if (i < formal_args.size()-1)
                    stream << ", ";
            }
            stream << ")" << std::endl; 
            output(BOOST_WAVE_GETSTRING(stream));
        }

        if (formal_args.size() > 0) {
        // map formal and real arguments
            open_trace_body("invoked with\n");
            for (typename std::vector<TokenT>::size_type j = 0; 
                j < formal_args.size(); ++j) 
            {
                using namespace boost::wave;

                BOOST_WAVE_OSSTREAM stream;
                stream << formal_args[j].get_value() << " = ";
#if BOOST_WAVE_SUPPORT_VARIADICS_PLACEMARKERS != 0
                if (T_ELLIPSIS == token_id(formal_args[j])) {
                // ellipsis
                    for (typename ContainerT::size_type k = j; 
                        k < arguments.size(); ++k) 
                    {
                        stream << boost::wave::util::impl::as_string(arguments[k]);
                        if (k < arguments.size()-1)
                            stream << ", ";
                    }
                } 
                else 
#endif
                {
                    stream << boost::wave::util::impl::as_string(arguments[j]);
                }
                stream << std::endl;
                output(BOOST_WAVE_GETSTRING(stream));
            }
            close_trace_body();
        }
        open_trace_body();
    }

    ///////////////////////////////////////////////////////////////////////////
    //  
    //  The function 'expanding_object_like_macro' is called, whenever a 
    //  object-like macro is to be expanded .
    //
    //  The 'macrodef' parameter marks the position, where the macro to expand 
    //  is defined.
    //  The 'definition' parameter holds the macro definition for the macro to 
    //  trace.
    //
    //  The 'macrocall' parameter marks the position, where this macro invoked.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename TokenT, typename ContainerT>
    void expanding_object_like_macro(TokenT const &macrodef, 
        ContainerT const &definition, TokenT const &macrocall)
    {
        if (!enabled_macro_tracing()) return;
        
        if (0 == get_level()) {
        // output header line
        BOOST_WAVE_OSSTREAM stream;

            stream 
                << macrocall.get_position() << ": "
                << macrocall.get_value() << std::endl;
            output(BOOST_WAVE_GETSTRING(stream));
            increment_level();
        }
        
    // output definition reference
        {
        BOOST_WAVE_OSSTREAM stream;

            stream 
                << macrodef.get_position() << ": see macro definition: "
                << macrodef.get_value() << std::endl;
            output(BOOST_WAVE_GETSTRING(stream));
        }
        open_trace_body();
    }
    
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
    {
        if (!enabled_macro_tracing()) return;
        
        BOOST_WAVE_OSSTREAM stream;
        stream << boost::wave::util::impl::as_string(result) << std::endl;
        output(BOOST_WAVE_GETSTRING(stream));

        open_trace_body("rescanning\n");
    }

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
    {
        if (!enabled_macro_tracing() || get_level() == 0) 
            return;

        BOOST_WAVE_OSSTREAM stream;
        stream << boost::wave::util::impl::as_string(result) << std::endl;
        output(BOOST_WAVE_GETSTRING(stream));
        close_trace_body();
        close_trace_body();
        
        if (1 == get_level())
            decrement_level();
    }

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
        if (option.get_value() == "timer") {
        // #pragma wave timer(value)
            if (0 == values.size()) {
            // no value means '1'
                using namespace boost::wave;
                timer(TokenT(T_INTLIT, "1", act_token.get_position()));
            }
            else {
                timer(values.front());
            }
            return true;
        }
        return false;
    }
        
protected:
    void open_trace_body(char const *label = 0)
    {
        if (label)
            output(label);
        output("[\n");
        increment_level();
    }
    void close_trace_body()
    {
        if (get_level() > 0) {
            decrement_level();
            output("]\n");
            outstrm << std::flush;      // flush the stream buffer
        }
    }

    template <typename StringT>
    void output(StringT const &outstr) const
    {
        indent(get_level());
        outstrm << outstr;          // output the given string
    }

    void indent(int level) const
    {
        for (int i = 0; i < level; ++i)
            outstrm << "  ";        // indent
    }

    int increment_level() { return ++level; }
    int decrement_level() { BOOST_ASSERT(level > 0); return --level; }
    int get_level() const { return level; }
    
    bool enabled_macro_tracing() const 
    { 
        using namespace boost::wave::trace_policies;
        return (flags & trace_macros) && (logging_flags & trace_macros); 
    }
    bool enabled_include_tracing() const 
    { 
        using namespace boost::wave::trace_policies;
        return (flags & trace_includes) && (logging_flags & trace_includes); 
    }
    
    template <typename TokenT>
    void timer(TokenT const &value)
    {
        if (value.get_value() == "0" || value.get_value() == "restart") {
        // restart the timer
            elapsed_time.restart();
        }
        else if (value.get_value() == "1") {
        // print out the current elapsed time
            std::cerr 
                << value.get_position() << ": " 
                << elapsed_time.format_elapsed_time()
                << std::endl;
        }
        else if (value.get_value() == "suspend") {
        // suspend the timer
            elapsed_time.suspend();
        }
        else if (value.get_value() == "resume") {
        // resume the timer
            elapsed_time.resume();
        }
    }

private:
    std::ostream &outstrm;          // output stream
    int level;                      // indentation level
    boost::wave::trace_policies::trace_flags flags;            // enabled globally
    boost::wave::trace_policies::trace_flags logging_flags;    // enabled by a #pragma
    
    stop_watch elapsed_time;        // trace timings
};

#undef BOOST_WAVE_GETSTRING
#undef BOOST_WAVE_OSSTREAM

#endif // !defined(TRACE_MACRO_EXPANSION_HPP_D8469318_8407_4B9D_A19F_13CA60C1661F_INCLUDED)
