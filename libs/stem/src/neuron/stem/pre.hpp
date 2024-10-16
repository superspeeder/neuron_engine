#pragma once

#ifdef WIN32
# ifdef NEURON_STEM_SHARED
#  ifdef NEURON_STEM_SHARED_EXPORTS
#   define NEURON_STEM_API __declspec(dllexport)
#  else
#   define NEURON_STEM_API __declspec(dllimport)
#  endif
# else
#  define NEURON_STEM_API
# endif
#else
# define NEURON_STEM_API
#endif