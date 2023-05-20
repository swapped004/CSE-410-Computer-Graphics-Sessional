#ifndef INCLUDED_LINE_AA_API
#define INCLUDED_LINE_AA_API

// linkage
#define LINE_AA_LINKAGE extern

// calling convention
#ifdef _MSC_VER
# define LINE_AA_CALL __cdecl
#else
# define LINE_AA_CALL
#endif

// DLL interface
#ifdef LINE_AA_BUILD
# define LINE_AA_API LINE_AA_LINKAGE __declspec(dllexport)
# else
# define LINE_AA_API LINE_AA_LINKAGE __declspec(dllimport)
#endif

// automatically link against the import library
#ifndef LINE_AA_BUILD
# ifdef _MSC_VER
#  ifdef NDEBUG
#   pragma comment(lib, "LineAA.lib")
#  else
#   pragma comment(lib, "LineAA_d.lib")
#  endif
# endif
#endif

#endif	// #ifndef INCLUDED_LINE_AA_API
