#include "stem.hpp"

namespace neuron::stem {
    struct StemAccessor {
        PlatformGenerics* platform;
    };

    StemAccessor& get_stem_accessor() {
        static StemAccessor accessor{};
        return accessor;
    }


    const char *get_version() {
        return NEURON_STEM_VERSION;
    }

    void set_active_platform(PlatformGenerics *platform) {
        get_stem_accessor().platform = platform;
    }

    void unset_active_platform() {
        get_stem_accessor().platform = nullptr;
    }

    PlatformGenerics *active_platform() {
        return get_stem_accessor().platform;
    }
} // namespace neuron::stem
