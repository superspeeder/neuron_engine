#pragma once

#include <vulkan/vulkan.hpp>
#include "neuron/occipital/occipital.hpp"

namespace neuron::occipital {

    struct DeviceFeatureSupport {

    };

    class DeviceContext : public std::enable_shared_from_this<DeviceContext> {
        explicit DeviceContext(const std::shared_ptr<Occipital>& occipital); // TODO: device selection.
      public:

        virtual ~DeviceContext();

      private:
        vk::PhysicalDevice m_physical_device;
        vk::Device m_device;
    };

} // namespace neuron::occipital
