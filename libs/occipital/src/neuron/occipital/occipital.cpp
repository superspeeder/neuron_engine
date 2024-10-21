#include "occipital.hpp"

#include <iostream>
#include <neuron/stem/stem.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

std::string colored_bool_text(const bool b) {
    return b ? "\033[1;32mtrue\033[0m " : "\033[1;31mfalse\033[0m";
}

namespace neuron::occipital {
    namespace detail {
        std::shared_ptr<Occipital> occipital;
    }

    const char *get_version() {
        return OCCIPITAL_VERSION;
    }

    Occipital::Occipital(const OccipitalSettings &settings) {
        assert(stem::active_platform() != nullptr, "No active platform.");
        assert(stem::active_platform()->get_vulkan_instance_loader() != nullptr, "Active platform does not support Vulkan.");

        VULKAN_HPP_DEFAULT_DISPATCHER.init(reinterpret_cast<PFN_vkGetInstanceProcAddr>(stem::active_platform()->get_vulkan_instance_loader()));
        auto                      extensions = stem::active_platform()->required_instance_extensions();
        std::vector<const char *> layers;

        const auto inst_ver = vk::enumerateInstanceVersion();
        if (VK_API_VERSION_MAJOR(inst_ver) != 1)
            throw std::runtime_error("Vulkan instance version is invalid");
        if (VK_API_VERSION_MINOR(inst_ver) != 2 && VK_API_VERSION_MINOR(inst_ver) != 3)
            throw std::runtime_error("Vulkan instance version is invalid");

        const bool supports_v3 = VK_API_VERSION_MINOR(inst_ver) == 3;

        const auto supported_layers     = vk::enumerateInstanceLayerProperties();
        const auto supported_extensions = vk::enumerateInstanceExtensionProperties();

        bool supports_validation_layer       = false;
        bool supports_synchronization2_layer = false;
        bool supports_shader_object_layer    = false;

        bool supports_debug_utils             = false;
        bool supports_display                 = false;
        bool supports_display_props_2         = false;
        bool supports_surface_caps_2          = false;
        bool supports_portability_enum        = false;
        bool supports_surface_protected_caps  = false;
        bool supports_direct_mode_display     = false;
        bool supports_display_surface_counter = false;
        bool supports_headless_surface        = false;
        bool supports_layer_settings          = false;
        bool supports_surface_maintenance1    = false;
        bool supports_swapchain_colorspace    = false;
        bool supports_surfaceless_query       = false;


        for (const auto &layer : supported_layers) {
            if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
                supports_validation_layer = true;
            } else if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_synchronization2") == 0) {
                supports_synchronization2_layer = true;
            } else if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_shader_object") == 0) {
                supports_shader_object_layer = true;
            }
        }

        for (const auto &ext : supported_extensions) {
            if (strcmp(ext.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
                supports_debug_utils = true;
            } else if (strcmp(ext.extensionName, VK_KHR_DISPLAY_EXTENSION_NAME) == 0) {
                supports_display = true;
            } else if (strcmp(ext.extensionName, VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME) == 0) {
                supports_display_props_2 = true;
            } else if (strcmp(ext.extensionName, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME) == 0) {
                supports_surface_caps_2 = true;
            } else if (strcmp(ext.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0) {
                supports_portability_enum = true;
            } else if (strcmp(ext.extensionName, VK_KHR_SURFACE_PROTECTED_CAPABILITIES_EXTENSION_NAME) == 0) {
                supports_surface_protected_caps = true;
            } else if (strcmp(ext.extensionName, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME) == 0) {
                supports_direct_mode_display = true;
            } else if (strcmp(ext.extensionName, VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME) == 0) {
                supports_display_surface_counter = true;
            } else if (strcmp(ext.extensionName, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) == 0) {
                supports_headless_surface = true;
            } else if (strcmp(ext.extensionName, VK_EXT_LAYER_SETTINGS_EXTENSION_NAME) == 0) {
                supports_layer_settings = true;
            } else if (strcmp(ext.extensionName, VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME) == 0) {
                supports_surface_maintenance1 = true;
            } else if (strcmp(ext.extensionName, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME) == 0) {
                supports_swapchain_colorspace = true;
            } else if (strcmp(ext.extensionName, VK_GOOGLE_SURFACELESS_QUERY_EXTENSION_NAME) == 0) {
                supports_surfaceless_query = true;
            }
        }

        for (const auto &ext : extensions) {
            bool supported = false;
            for (const auto &ext2 : supported_extensions) {
                if (strcmp(ext, ext2.extensionName) == 0) {
                    supported = true;
                    break;
                }
            }

            if (!supported) {
                throw std::runtime_error("Vulkan required surface extensions are not supported.");
            }
        }

        std::cout << "Host Support:\n";
        std::cout << "  Layers:\n";
        std::cout << "    Validation                    : " << colored_bool_text(supports_validation_layer) << " \033[2m(VK_LAYER_KHRONOS_validation)\033[0m\n";
        std::cout << "    Synchronization 2             : " << colored_bool_text(supports_synchronization2_layer) << " \033[2m(VK_LAYER_KHRONOS_synchronization2)\033[0m\n";
        std::cout << "    Shader Object                 : " << colored_bool_text(supports_shader_object_layer) << " \033[2m(VK_LAYER_KHRONOS_shader_object)\033[0m\n";
        std::cout << "  Extensions:\n";
        std::cout << "    Debug Utils                   : " << colored_bool_text(supports_debug_utils) << " \033[2m(" << VK_EXT_DEBUG_UTILS_EXTENSION_NAME << ")\033[0m\n";
        std::cout << "    Display                       : " << colored_bool_text(supports_display) << " \033[2m(" << VK_KHR_DISPLAY_EXTENSION_NAME << ")\033[0m\n";
        std::cout << "    Display Properties 2          : " << colored_bool_text(supports_display_props_2) << " \033[2m(" << VK_KHR_GET_DISPLAY_PROPERTIES_2_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Surface Capabilities 2        : " << colored_bool_text(supports_surface_caps_2) << " \033[2m(" << VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Portability Enumeration       : " << colored_bool_text(supports_portability_enum) << " \033[2m(" << VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Surface Protected Capabilities: " << colored_bool_text(supports_surface_protected_caps) << " \033[2m("
                  << VK_KHR_SURFACE_PROTECTED_CAPABILITIES_EXTENSION_NAME << ")\033[0m\n";
        std::cout << "    Direct Mode Display           : " << colored_bool_text(supports_direct_mode_display) << " \033[2m(" << VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Display Surface Counter       : " << colored_bool_text(supports_display_surface_counter) << " \033[2m(" << VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Headless Surface              : " << colored_bool_text(supports_headless_surface) << " \033[2m(" << VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Layer Settings                : " << colored_bool_text(supports_layer_settings) << " \033[2m(" << VK_EXT_LAYER_SETTINGS_EXTENSION_NAME << ")\033[0m\n";
        std::cout << "    Surface Maintenance 1         : " << colored_bool_text(supports_surface_maintenance1) << " \033[2m(" << VK_EXT_SURFACE_MAINTENANCE_1_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Swapchain Colorspace          : " << colored_bool_text(supports_swapchain_colorspace) << " \033[2m(" << VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
                  << ")\033[0m\n";
        std::cout << "    Surfaceless Query             : " << colored_bool_text(supports_surfaceless_query) << " \033[2m(" << VK_GOOGLE_SURFACELESS_QUERY_EXTENSION_NAME
                  << ")\033[0m\n";

        vk::ApplicationInfo app_info{};
        if (supports_v3 && !settings.force_v2) {
            app_info.setApiVersion(vk::ApiVersion13);
        } else {
            app_info.setApiVersion(vk::ApiVersion12);
        }

        std::cout << "Enabled Features:\n";

        bool feature_error                   = false;
        bool enable_direct_display_rendering = false;
        bool enable_validation               = false;
        bool enable_headless_surface         = false;
        bool enable_surfaceless_query        = false;

        switch (settings.direct_display_rendering) {
        case FeatureDependencyMode::None:
            std::cout << "  Direct Display Rendering        : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        case FeatureDependencyMode::IfAvailable:
            if (supports_display) {
                std::cout << "  Direct Display Rendering        : \033[1;32menabled     \033[0m\033[2m(optional)\033[0m\n";
                enable_direct_display_rendering = true;
            } else {
                std::cout << "  Direct Display Rendering        : \033[1;31munavailable \033[0m\033[2m(optional)\033[0m\n";
            }
            break;
        case FeatureDependencyMode::Required:
            if (supports_display) {
                std::cout << "  Direct Display Rendering        : \033[1;32menabled     \033[0m\033[2m(required)\033[0m\n";
                enable_direct_display_rendering = true;
            } else {
                std::cout << "  Direct Display Rendering        : \033[1;101munavailable\033[0m\033[2m (required)\033[0m\n";
            }
            break;
        default:
            std::cout << "  Direct Display Rendering        : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        }

        switch (settings.validation) {
        case FeatureDependencyMode::None:
            std::cout << "  Validation                      : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        case FeatureDependencyMode::IfAvailable:
            if (supports_debug_utils && supports_validation_layer) {
                std::cout << "  Validation                      : \033[1;32menabled     \033[0m\033[2m(optional)\033[0m\n";
                enable_validation = true;
            } else {
                std::cout << "  Validation                      : \033[1;31munavailable \033[0m\033[2m(optional)\033[0m\n";
            }
            break;
        case FeatureDependencyMode::Required:
            if (supports_debug_utils && supports_validation_layer) {
                std::cout << "  Validation                      : \033[1;32menabled     \033[0m\033[2m(required)\033[0m\n";
                enable_validation = true;
            } else {
                std::cout << "  Validation                      : \033[1;97;101munavailable\033[0m\033[2m (required)\033[0m\n";
                feature_error = true;
            }
            break;
        default:
            std::cout << "  Validation                      : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        }

        switch (settings.headless_surface) {
        case FeatureDependencyMode::None:
            std::cout << "  Headless Surface                : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        case FeatureDependencyMode::IfAvailable:
            if (supports_headless_surface) {
                std::cout << "  Headless Surface                : \033[1;32menabled     \033[0m\033[2m(optional)\033[0m\n";
                enable_headless_surface = true;
            } else {
                std::cout << "  Headless Surface                : \033[1;31munavailable \033[0m\033[2m(optional)\033[0m\n";
            }
            break;
        case FeatureDependencyMode::Required:
            if (supports_headless_surface) {
                std::cout << "  Headless Surface                : \033[1;32menabled     \033[0m\033[2m(required)\033[0m\n";
                enable_headless_surface = true;
            } else {
                std::cout << "  Headless Surface                : \033[1;97;101munavailable\033[0m\033[2m (required)\033[0m\n";
                feature_error = true;
            }
            break;
        default:
            std::cout << "  Headless Surface                : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        }

        switch (settings.surfaceless_query) {
        case FeatureDependencyMode::None:
            std::cout << "  Surfaceless Query               : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        case FeatureDependencyMode::IfAvailable:
            if (supports_surfaceless_query) {
                std::cout << "  Surfaceless Query               : \033[1;32menabled     \033[0m\033[2m(optional)\033[0m\n";
                enable_surfaceless_query = true;
            } else {
                std::cout << "  Surfaceless Query               : \033[1;31munavailable \033[0m\033[2m(optional)\033[0m\n";
            }
            break;
        case FeatureDependencyMode::Required:
            if (supports_surfaceless_query) {
                std::cout << "  Surfaceless Query               : \033[1;32menabled     \033[0m\033[2m(required)\033[0m\n";
                enable_surfaceless_query = true;
            } else {
                std::cout << "  Surfaceless Query               : \033[1;97;101munavailable\033[0m\033[2m (required)\033[0m\n";
                feature_error = true;
            }
            break;
        default:
            std::cout << "  Surfaceless Query               : \033[1;31mdisabled    \033[0m\033[2m(not requested)\033[0m\n";
            break;
        }

        if (feature_error) {
            throw std::runtime_error("Not all required features are available");
        }

        m_feature_availability{.v3 = app_info.apiVersion == vk::ApiVersion13};

        if (enable_direct_display_rendering) {
            extensions.push_back(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
            m_feature_availability.direct_display_rendering = true;
        }

        vk::DebugUtilsMessengerCreateInfoEXT messenger_create_info;

        if (enable_validation) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            layers.push_back("VK_LAYER_KHRONOS_validation");
            m_feature_availability.validation  = true;
            messenger_create_info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
            messenger_create_info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding;
            messenger_create_info.pfnUserCallback = +[](VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
                std::cout << "[\033[1;36mvalidation\033[0m] "
                          << "(" << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(severity)) << ") "
                          << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(types)) << " " << pCallbackData->pMessage << std::endl;
                return VK_FALSE;
            };
        }

        if (enable_headless_surface) {
            extensions.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
            m_feature_availability.headless_surface = true;
        }

        if (enable_surfaceless_query) {
            extensions.push_back(VK_GOOGLE_SURFACELESS_QUERY_EXTENSION_NAME);
            m_feature_availability.surfaceless_query = true;
        }

        if (supports_synchronization2_layer) {
            layers.push_back("VK_LAYER_KHRONOS_synchronization2");
            m_feature_availability.sync2_emulation = true;
        }

        if (supports_shader_object_layer) {
            layers.push_back("VK_LAYER_KHRONOS_shader_object");
            m_feature_availability.shader_obj_emulation = true;
        }

        vk::InstanceCreateFlags instance_create_flags{};

        if (supports_portability_enum && settings.allow_non_conformant_devices) {
            extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            instance_create_flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
            m_feature_availability.non_conformant_devices = true;
            std::cout << "Non-conformant device enumeration : " << colored_bool_text(true) << "\n";
        } else {
            std::cout << "Non-conformant device enumeration : " << colored_bool_text(false) << "\n";
        }

        void *instance_pnext = nullptr;
        if (enable_validation)
            instance_pnext = &messenger_create_info;

        m_instance = vk::createInstance(vk::InstanceCreateInfo(instance_create_flags, &app_info, layers, extensions, instance_pnext));
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_instance);

        if (enable_validation)
            m_debug_messenger = m_instance.createDebugUtilsMessengerEXT(messenger_create_info);
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
        if (m_instance) {
            if (m_debug_messenger)
                m_instance.destroy(m_debug_messenger);

            m_instance.destroy();
        }
    }

    const vk::Instance &Occipital::provide(stem::blank_typed<vk::Instance>) {
        return m_instance;
    }
} // namespace neuron::occipital
