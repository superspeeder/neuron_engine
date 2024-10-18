#pragma once

#include "neuron/occipital/pre.hpp"
#include <neuron/stem/interfaces.hpp>

#include <vulkan/vulkan.hpp>

namespace neuron::occipital {
    OCCIPITAL_API const char *get_version();

    enum class FeatureDependencyMode {
        None,
        IfAvailable,
        Required,
    };

    struct OccipitalSettings {
        bool force_v2 = false;
        FeatureDependencyMode direct_display_rendering = FeatureDependencyMode::None;
        FeatureDependencyMode validation = FeatureDependencyMode::None;
        FeatureDependencyMode headless_surface = FeatureDependencyMode::None;
        FeatureDependencyMode surfaceless_query = FeatureDependencyMode::None;
        bool allow_non_conformant_devices = false;
    };

    struct HostFeatureAvailability {
        bool v3 = false;
        bool non_conformant_devices = false;
        bool direct_display_rendering = false;
        bool validation = false;
        bool headless_surface = false;
        bool surfaceless_query = false;
        bool sync2_emulation = false;
        bool shader_obj_emulation = false;
    };

    class OCCIPITAL_API Occipital : public stem::Provider<vk::Instance>, public std::enable_shared_from_this<Occipital> {
        explicit Occipital(const OccipitalSettings &settings);

      public:
        static std::shared_ptr<Occipital> init(const OccipitalSettings &settings);
        static std::shared_ptr<Occipital> get();
        static void cleanup();

        virtual ~Occipital();

        const vk::Instance &provide(stem::blank_typed<vk::Instance>) override;

      private:
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_messenger;
    };
} // namespace neuron::occipital
