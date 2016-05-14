/*************************************************************************

USAGE:
~~~~~~

Before including this header you must define one or more of define the following macros:

TWP_LIB_NAME:           Required: A string containing the basename of the library,
                          for example net.
TWP_LIB_DIAGNOSTIC:     Optional: when set the header will print out the name
                          of the library selected (useful for debugging).

These macros will be undef'ed at the end of the header, further this header
has no include guards - so be sure to include it only once from your library!

Algorithm:
~~~~~~~~~~

Libraries for Microsoft compilers are automatically
selected here, the name of the lib is selected according to the following
formula:

TWP_LIB_NAME
   + "_"
   + TWP_LIB_RTL_OPT
   + "_"
   + TWP_LIB_DEBUG_OPT
   + ".lib"

These are defined as:

TWP_LIB_NAME:       The base name of the lib ( for example TWP_regex).

TWP_LIB_DEBUG_OPT: "debug" for debug version,otherwise "release".

TWP_LIB_RTL_OPT:   for c/cpp run-time library

***************************************************************************/

#ifndef TWP_LIB_NAME
#  error "Macro TWP_LIB_NAME not set (internal error)"
#endif

// check this supported compilers

// set TWP_LIB_RTL_OPT for c/cpp run-time library
#if defined(_MT) && defined(_DLL)
#  define TWP_LIB_RTL_OPT "dynamic-rtl"
#else
#	if defined(_MT)
#		define TWP_LIB_RTL_OPT "static-rtl"
#	else
#		define TWP_LIB_RTL_OPT "static-rtl"
#	endif
#endif

// set TWP_LIB_DEBUG_OPT
#ifdef _DEBUG
#  define TWP_LIB_DEBUG_OPT "debug"
#else
#  define TWP_LIB_DEBUG_OPT "release"
#endif

//
// now include the lib:
//
#if defined(TWP_LIB_NAME) \
    && defined(TWP_LIB_RTL_OPT) \
    && defined(TWP_LIB_DEBUG_OPT)
#else
#  error "some required macros where not defined (internal logic error)."
#endif

#if defined _MSC_VER	
#	 pragma comment(lib, TWP_LIB_NAME "_" TWP_LIB_RTL_OPT "_" TWP_LIB_DEBUG_OPT ".lib")
#elif defined __GNUC__

#else
#	 error "Unknown compiler, only support for msvc and gcc."
#endif

#  ifdef TWP_LIB_DIAGNOSTIC
#	pragma message ("Linking to lib file: " TWP_LIB_NAME "_" TWP_LIB_RTL_OPT "_" TWP_LIB_DEBUG_OPT ".lib")
#endif

//
// finally undef any macros we may have set:
//
#if defined(TWP_LIB_NAME)
#  undef TWP_LIB_NAME
#endif
#if defined(TWP_LIB_RTL_OPT)
#  undef TWP_LIB_RTL_OPT
#endif
#if defined(TWP_LIB_DEBUG_OPT)
#  undef TWP_LIB_DEBUG_OPT
#endif