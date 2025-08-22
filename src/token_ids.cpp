/*=============================================================================
    Boost.Wave: A Standard compliant C++ preprocessor library
    The definition of a default set of token identifiers and related
    functions.

    http://www.boost.org/

    Copyright (c) 2001-2012 Hartmut Kaiser. Distributed under the Boost
    Software License, Version 1.0. (See accompanying file
    LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#define BOOST_WAVE_SOURCE 1

// disable stupid compiler warnings
#include <boost/config/warning_disable.hpp>

#include <string>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

#include <boost/wave/wave_config.hpp>
#include <boost/wave/token_ids.hpp>

// this must occur after all of the includes and before any code appears
#ifdef BOOST_HAS_ABI_HEADERS
#include BOOST_ABI_PREFIX
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {

///////////////////////////////////////////////////////////////////////////////
//  return a token name
BOOST_WAVE_STRINGTYPE
get_token_name(token_id tokid)
{
//  Table of token names
//
//      Please note that the sequence of token names must match the sequence of
//      token id's defined in then enum token_id above.
static char const *tok_names[] = {
    /* 256 */   "AND",
    /* 257 */   "ANDAND",
    /* 258 */   "ASSIGN",
    /* 259 */   "ANDASSIGN",
    /* 260 */   "OR",
    /* 261 */   "ORASSIGN",
    /* 262 */   "XOR",
    /* 263 */   "XORASSIGN",
    /* 264 */   "COMMA",
    /* 265 */   "COLON",
    /* 266 */   "DIVIDE",
    /* 267 */   "DIVIDEASSIGN",
    /* 268 */   "DOT",
    /* 269 */   "DOTSTAR",
    /* 270 */   "ELLIPSIS",
    /* 271 */   "EQUAL",
    /* 272 */   "GREATER",
    /* 273 */   "GREATEREQUAL",
    /* 274 */   "LEFTBRACE",
    /* 275 */   "LESS",
    /* 276 */   "LESSEQUAL",
    /* 277 */   "LEFTPAREN",
    /* 278 */   "LEFTBRACKET",
    /* 279 */   "MINUS",
    /* 280 */   "MINUSASSIGN",
    /* 281 */   "MINUSMINUS",
    /* 282 */   "PERCENT",
    /* 283 */   "PERCENTASSIGN",
    /* 284 */   "NOT",
    /* 285 */   "NOTEQUAL",
    /* 286 */   "OROR",
    /* 287 */   "PLUS",
    /* 288 */   "PLUSASSIGN",
    /* 289 */   "PLUSPLUS",
    /* 290 */   "ARROW",
    /* 291 */   "ARROWSTAR",
    /* 292 */   "QUESTION_MARK",
    /* 293 */   "RIGHTBRACE",
    /* 294 */   "RIGHTPAREN",
    /* 295 */   "RIGHTBRACKET",
    /* 296 */   "COLON_COLON",
    /* 297 */   "SEMICOLON",
    /* 298 */   "SHIFTLEFT",
    /* 299 */   "SHIFTLEFTASSIGN",
    /* 300 */   "SHIFTRIGHT",
    /* 301 */   "SHIFTRIGHTASSIGN",
    /* 302 */   "STAR",
    /* 303 */   "COMPL",
    /* 304 */   "STARASSIGN",
    /* 305 */   "ASM",
    /* 306 */   "AUTO",
    /* 307 */   "BOOL",
    /* 308 */   "FALSE",
    /* 309 */   "TRUE",
    /* 310 */   "BREAK",
    /* 311 */   "CASE",
    /* 312 */   "CATCH",
    /* 313 */   "CHAR",
    /* 314 */   "CLASS",
    /* 315 */   "CONST",
    /* 316 */   "CONSTCAST",
    /* 317 */   "CONTINUE",
    /* 318 */   "DEFAULT",
    /* 319 */   "DELETE",
    /* 320 */   "DO",
    /* 321 */   "DOUBLE",
    /* 322 */   "DYNAMICCAST",
    /* 323 */   "ELSE",
    /* 324 */   "ENUM",
    /* 325 */   "EXPLICIT",
    /* 326 */   "EXPORT",
    /* 327 */   "EXTERN",
    /* 328 */   "FLOAT",
    /* 329 */   "FOR",
    /* 330 */   "FRIEND",
    /* 331 */   "GOTO",
    /* 332 */   "IF",
    /* 333 */   "INLINE",
    /* 334 */   "INT",
    /* 335 */   "LONG",
    /* 336 */   "MUTABLE",
    /* 337 */   "NAMESPACE",
    /* 338 */   "NEW",
    /* 339 */   "OPERATOR",
    /* 340 */   "PRIVATE",
    /* 341 */   "PROTECTED",
    /* 342 */   "PUBLIC",
    /* 343 */   "REGISTER",
    /* 344 */   "REINTERPRETCAST",
    /* 345 */   "RETURN",
    /* 346 */   "SHORT",
    /* 347 */   "SIGNED",
    /* 348 */   "SIZEOF",
    /* 349 */   "STATIC",
    /* 350 */   "STATICCAST",
    /* 351 */   "STRUCT",
    /* 352 */   "SWITCH",
    /* 353 */   "TEMPLATE",
    /* 354 */   "THIS",
    /* 355 */   "THROW",
    /* 356 */   "TRY",
    /* 357 */   "TYPEDEF",
    /* 358 */   "TYPEID",
    /* 359 */   "TYPENAME",
    /* 360 */   "UNION",
    /* 361 */   "UNSIGNED",
    /* 362 */   "USING",
    /* 363 */   "VIRTUAL",
    /* 364 */   "VOID",
    /* 365 */   "VOLATILE",
    /* 366 */   "WCHART",
    /* 367 */   "WHILE",
    /* 368 */   "PP_DEFINE",
    /* 369 */   "PP_IF",
    /* 370 */   "PP_IFDEF",
    /* 371 */   "PP_IFNDEF",
    /* 372 */   "PP_ELSE",
    /* 373 */   "PP_ELIF",
    /* 374 */   "PP_ENDIF",
    /* 375 */   "PP_ERROR",
    /* 376 */   "PP_LINE",
    /* 377 */   "PP_PRAGMA",
    /* 378 */   "PP_UNDEF",
    /* 379 */   "PP_WARNING",
    /* 380 */   "IDENTIFIER",
    /* 381 */   "OCTALINT",
    /* 382 */   "DECIMALINT",
    /* 383 */   "HEXAINT",
    /* 384 */   "INTLIT",
    /* 385 */   "LONGINTLIT",
    /* 386 */   "SIZETLIT",
    /* 387 */   "FLOATLIT",
    /* 388 */   "CCOMMENT",
    /* 389 */   "CPPCOMMENT",
    /* 390 */   "CHARLIT",
    /* 391 */   "STRINGLIT",
    /* 392 */   "CONTLINE",
    /* 393 */   "SPACE",
    /* 394 */   "SPACE2",
    /* 395 */   "NEWLINE",
    /* 396 */   "POUND_POUND",
    /* 397 */   "POUND",
    /* 398 */   "ANY",
    /* 399 */   "PP_INCLUDE",
    /* 400 */   "PP_QHEADER",
    /* 401 */   "PP_HHEADER",
    /* 402 */   "EOF",
    /* 403 */   "EOI",
    /* 404 */   "PP_NUMBER",

              // MS extensions
    /* 405 */   "MSEXT_INT8",
    /* 406 */   "MSEXT_INT16",
    /* 407 */   "MSEXT_INT32",
    /* 408 */   "MSEXT_INT64",
    /* 409 */   "MSEXT_BASED",
    /* 410 */   "MSEXT_DECLSPEC",
    /* 411 */   "MSEXT_CDECL",
    /* 412 */   "MSEXT_FASTCALL",
    /* 413 */   "MSEXT_STDCALL",
    /* 414 */   "MSEXT_TRY",
    /* 415 */   "MSEXT_EXCEPT",
    /* 416 */   "MSEXT_FINALLY",
    /* 417 */   "MSEXT_LEAVE",
    /* 418 */   "MSEXT_INLINE",
    /* 419 */   "MSEXT_ASM",
    /* 420 */   "MSEXT_REGION",
    /* 421 */   "MSEXT_ENDREGION",

    /* 422 */   "IMPORT",

    /* 423 */   "ALIGNAS",
    /* 424 */   "ALIGNOF",
    /* 425 */   "CHAR16_T",
    /* 426 */   "CHAR32_T",
    /* 427 */   "CONSTEXPR",
    /* 428 */   "DECLTYPE",
    /* 429 */   "NOEXCEPT",
    /* 430 */   "NULLPTR",
    /* 431 */   "STATIC_ASSERT",
    /* 432 */   "THREADLOCAL",
    /* 433 */   "RAWSTRINGLIT",

    /* 434 */   "T_CHAR8_T",
    /* 435 */   "T_CONCEPT",
    /* 436 */   "T_CONSTEVAL",
    /* 437 */   "T_CONSTINIT",
    /* 438 */   "T_CO_AWAIT",
    /* 439 */   "T_CO_RETURN",
    /* 440 */   "T_CO_YIELD",
    /* 441 */   "T_REQUIRES",
    /* 442 */   "T_MODULE",
    /* 443 */   "T_SPACESHIP",
    };

    // make sure, I have not forgotten any commas (as I did more than once)
    BOOST_STATIC_ASSERT(
        sizeof(tok_names)/sizeof(tok_names[0]) == T_LAST_TOKEN-T_FIRST_TOKEN
    );

    unsigned int id = BASEID_FROM_TOKEN(tokid)-T_FIRST_TOKEN;
    return (id < T_LAST_TOKEN-T_FIRST_TOKEN) ? tok_names[id] : "<UnknownToken>";
}

///////////////////////////////////////////////////////////////////////////////
//  return a token name
char const *
get_token_value(token_id tokid)
{
//  Table of token values
//
//      Please note that the sequence of token names must match the sequence of
//      token id's defined in then enum token_id above.
static char const *tok_values[] = {
    /* 256 */   "&",
    /* 257 */   "&&",
    /* 258 */   "=",
    /* 259 */   "&=",
    /* 260 */   "|",
    /* 261 */   "|=",
    /* 262 */   "^",
    /* 263 */   "^=",
    /* 264 */   ",",
    /* 265 */   ":",
    /* 266 */   "/",
    /* 267 */   "/=",
    /* 268 */   ".",
    /* 269 */   ".*",
    /* 270 */   "...",
    /* 271 */   "==",
    /* 272 */   ">",
    /* 273 */   ">=",
    /* 274 */   "{",
    /* 275 */   "<",
    /* 276 */   "<=",
    /* 277 */   "(",
    /* 278 */   "[",
    /* 279 */   "-",
    /* 280 */   "-=",
    /* 281 */   "--",
    /* 282 */   "%",
    /* 283 */   "%=",
    /* 284 */   "!",
    /* 285 */   "!=",
    /* 286 */   "||",
    /* 287 */   "+",
    /* 288 */   "+=",
    /* 289 */   "++",
    /* 290 */   "->",
    /* 291 */   "->*",
    /* 292 */   "?",
    /* 293 */   "}",
    /* 294 */   ")",
    /* 295 */   "]",
    /* 296 */   "::",
    /* 297 */   ";",
    /* 298 */   "<<",
    /* 299 */   "<<=",
    /* 300 */   ">>",
    /* 301 */   ">>=",
    /* 302 */   "*",
    /* 303 */   "~",
    /* 304 */   "*=",
    /* 305 */   "asm",
    /* 306 */   "auto",
    /* 307 */   "bool",
    /* 308 */   "false",
    /* 309 */   "true",
    /* 310 */   "break",
    /* 311 */   "case",
    /* 312 */   "catch",
    /* 313 */   "char",
    /* 314 */   "class",
    /* 315 */   "const",
    /* 316 */   "const_cast",
    /* 317 */   "continue",
    /* 318 */   "default",
    /* 319 */   "delete",
    /* 320 */   "do",
    /* 321 */   "double",
    /* 322 */   "dynamic_cast",
    /* 323 */   "else",
    /* 324 */   "enum",
    /* 325 */   "explicit",
    /* 326 */   "export",
    /* 327 */   "extern",
    /* 328 */   "float",
    /* 329 */   "for",
    /* 330 */   "friend",
    /* 331 */   "goto",
    /* 332 */   "if",
    /* 333 */   "inline",
    /* 334 */   "int",
    /* 335 */   "long",
    /* 336 */   "mutable",
    /* 337 */   "namespace",
    /* 338 */   "new",
    /* 339 */   "operator",
    /* 340 */   "private",
    /* 341 */   "protected",
    /* 342 */   "public",
    /* 343 */   "register",
    /* 344 */   "reinterpret_cast",
    /* 345 */   "return",
    /* 346 */   "short",
    /* 347 */   "signed",
    /* 348 */   "sizeof",
    /* 349 */   "static",
    /* 350 */   "static_cast",
    /* 351 */   "struct",
    /* 352 */   "switch",
    /* 353 */   "template",
    /* 354 */   "this",
    /* 355 */   "throw",
    /* 356 */   "try",
    /* 357 */   "typedef",
    /* 358 */   "typeid",
    /* 359 */   "typename",
    /* 360 */   "union",
    /* 361 */   "unsigned",
    /* 362 */   "using",
    /* 363 */   "virtual",
    /* 364 */   "void",
    /* 365 */   "volatile",
    /* 366 */   "wchar_t",
    /* 367 */   "while",
    /* 368 */   "#define",
    /* 369 */   "#if",
    /* 370 */   "#ifdef",
    /* 371 */   "#ifndef",
    /* 372 */   "#else",
    /* 373 */   "#elif",
    /* 374 */   "#endif",
    /* 375 */   "#error",
    /* 376 */   "#line",
    /* 377 */   "#pragma",
    /* 378 */   "#undef",
    /* 379 */   "#warning",
    /* 380 */   "",   // identifier
    /* 381 */   "",   // octalint
    /* 382 */   "",   // decimalint
    /* 383 */   "",   // hexlit
    /* 384 */   "",   // intlit
    /* 385 */   "",   // longintlit
    /* 386 */   "",   // sizetintlit
    /* 387 */   "",   // floatlit
    /* 388 */   "",   // ccomment
    /* 389 */   "",   // cppcomment
    /* 390 */   "",   // charlit
    /* 391 */   "",   // stringlit
    /* 392 */   "",   // contline
    /* 393 */   "",   // space
    /* 394 */   "",   // space2
    /* 395 */   "\n",
    /* 396 */   "##",
    /* 397 */   "#",
    /* 398 */   "",   // any
    /* 399 */   "#include",
    /* 400 */   "#include",
    /* 401 */   "#include",
    /* 402 */   "",   // eof
    /* 403 */   "",   // eoi
    /* 404 */   "",   // pp-number

              // MS extensions
    /* 405 */   "__int8",
    /* 406 */   "__int16",
    /* 407 */   "__int32",
    /* 408 */   "__int64",
    /* 409 */   "__based",
    /* 410 */   "__declspec",
    /* 411 */   "__cdecl",
    /* 412 */   "__fastcall",
    /* 413 */   "__stdcall",
    /* 414 */   "__try",
    /* 415 */   "__except",
    /* 416 */   "__finally",
    /* 417 */   "__leave",
    /* 418 */   "__inline",
    /* 419 */   "__asm",
    /* 420 */   "#region",
    /* 421 */   "#endregion",

    /* 422 */   "import",

    /* 423 */   "alignas",
    /* 424 */   "alignof",
    /* 425 */   "char16_t",
    /* 426 */   "char32_t",
    /* 427 */   "constexpr",
    /* 428 */   "decltype",
    /* 429 */   "noexcept",
    /* 430 */   "nullptr",
    /* 431 */   "static_assert",
    /* 432 */   "thread_local",
    /* 433 */   "",   // extrawstringlit

    /* 434 */   "char8_t",
    /* 435 */   "concept",
    /* 436 */   "consteval",
    /* 437 */   "constinit",
    /* 438 */   "co_await",
    /* 439 */   "co_return",
    /* 440 */   "co_yield",
    /* 441 */   "requires",
    /* 442 */   "module",
    /* 443 */   "<=>",
    };

    // make sure, I have not forgotten any commas (as I did more than once)
    BOOST_STATIC_ASSERT(
        sizeof(tok_values)/sizeof(tok_values[0]) == T_LAST_TOKEN-T_FIRST_TOKEN
    );

    unsigned int id = BASEID_FROM_TOKEN(tokid)-T_FIRST_TOKEN;
    return (id < T_LAST_TOKEN-T_FIRST_TOKEN) ? tok_values[id] : "<UnknownToken>";
}

///////////////////////////////////////////////////////////////////////////////
}   // namespace wave
}   // namespace boost

// the suffix header occurs after all of the code
#ifdef BOOST_HAS_ABI_HEADERS
#include BOOST_ABI_SUFFIX
#endif
