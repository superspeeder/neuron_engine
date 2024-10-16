#pragma once

#include "neuron/occipital/pre.hpp"
#include <neuron/stem/interfaces.hpp>

#include <vulkan/vulkan.hpp>

namespace neuron::occipital {
    OCCIPITAL_API const char *get_version();

    struct OccipitalSettings {
        bool force_compatibility_mode = false;
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
    };
} // namespace neuron::occipital
