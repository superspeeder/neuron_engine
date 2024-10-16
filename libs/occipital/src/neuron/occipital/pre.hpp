#pragma once

#ifdef WIN32
# ifdef OCCIPITAL_SHARED
#  ifdef OCCIPITAL_SHARED_EXPORTS
#   define OCCIPITAL_API __declspec(dllexport)
#  else
#   define OCCIPITAL_API __declspec(dllimport)
#  endif
# else
#  define OCCIPITAL_API
# endif
#else
# define OCCIPITAL_API
#endif