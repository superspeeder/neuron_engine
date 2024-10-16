#pragma once

#include "neuron/stem/pre.hpp"
#include <vector>

namespace neuron::stem {
    NEURON_STEM_API const char* get_version();

    class NEURON_STEM_API PlatformGenerics {
    public:
        virtual ~PlatformGenerics() = default;

        virtual std::vector<const char*> vulkan_needed_instance_extensions() = 0;
    };

    NEURON_STEM_API void set_active_platform(PlatformGenerics* platform);
    NEURON_STEM_API void unset_active_platform();
    NEURON_STEM_API PlatformGenerics* active_platform();
}