#pragma once

#include "neuron/stem/pre.hpp"
#include <vector>

#include <vulkan/vk_platform.h>

#ifndef APIENTRY
#ifdef WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#endif

namespace vk {
    class SurfaceKHR;
    class Instance;
    class AllocationCallbacks;
}

namespace neuron::stem {
    NEURON_STEM_API const char *get_version();

    class NEURON_STEM_API PlatformGenerics {
      public:
        virtual ~PlatformGenerics() = default;

        virtual std::vector<const char *> required_instance_extensions() = 0;
        
        typedef void (*pfn_voidFunction)();
        typedef pfn_voidFunction (*pfn_vkGetInstanceProcAddr)(void*,const char*);

        virtual pfn_vkGetInstanceProcAddr get_vulkan_instance_loader() = 0;
    };

    class NEURON_STEM_API VulkanSurfaceCreator {
      public:
        virtual ~              VulkanSurfaceCreator()                                                                 = default;
        virtual vk::SurfaceKHR create_vulkan_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator) = 0;
    };

    NEURON_STEM_API void              set_active_platform(PlatformGenerics *platform);
    NEURON_STEM_API void              unset_active_platform();
    NEURON_STEM_API PlatformGenerics *active_platform();
} // namespace neuron::stem

#ifdef DEFINED_VKNS
#undef VULKAN_HPP_NAMESPACE
#endif
