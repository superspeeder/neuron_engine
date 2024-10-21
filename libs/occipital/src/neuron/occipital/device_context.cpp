#include "device_context.hpp"

namespace neuron::occipital {
    DeviceContext::DeviceContext(const std::shared_ptr<Occipital> &occipital) {
        m_physical_device = occipital->instance().enumeratePhysicalDevices()[0];

        auto extensions = m_physical_device.enumerateDeviceExtensionProperties();

        bool is_nonconformant = false;
        bool supports_dynamic_rendering = false;

    }

    DeviceContext::~DeviceContext() {
        if (m_device) {
            m_device.destroy();
        }
    }
} // namespace neuron::occipital
