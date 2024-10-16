#include "occipital.hpp"

#include <iostream>
#include <neuron/stem/stem.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace neuron::occipital {
    namespace detail {
        std::shared_ptr<Occipital> occipital;
    }

    const char *get_version() {
        return OCCIPITAL_VERSION;
    }

    Occipital::Occipital(const OccipitalSettings &settings) {
        VULKAN_HPP_DEFAULT_DISPATCHER.init();
        auto extensions = stem::active_platform()->vulkan_needed_instance_extensions();

        auto inst_ver = vk::enumerateInstanceVersion();
        if (VK_API_VERSION_MAJOR(inst_ver) != 1) throw std::runtime_error("Vulkan instance version is invalid");
        if (VK_API_VERSION_MINOR(inst_ver) != 2 && VK_API_VERSION_MINOR(inst_ver) != 3) throw std::runtime_error("Vulkan instance version is invalid");

        bool is_edge = VK_API_VERSION_MINOR(inst_ver) == 3;

        vk::ApplicationInfo app_info{};
        if (is_edge) {
            app_info.setApiVersion(vk::ApiVersion13);
        } else {
            app_info.setApiVersion(vk::ApiVersion12);
        }

        m_instance = vk::createInstance(vk::InstanceCreateInfo({}, &app_info, {}, extensions));
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);
    }

    std::shared_ptr<Occipital> Occipital::init(const OccipitalSettings &settings) {
        detail::occipital = std::shared_ptr<Occipital>(new Occipital(settings));
        return detail::occipital;
    }

    std::shared_ptr<Occipital> Occipital::get() {
        return detail::occipital;
    }

    void Occipital::cleanup() {
        detail::occipital.reset();
    }

    Occipital::~Occipital() {
        m_instance.destroy();
    }

    const vk::Instance &Occipital::provide(stem::blank_typed<vk::Instance>) {
        return m_instance;
    }
} // namespace neuron::occipital
