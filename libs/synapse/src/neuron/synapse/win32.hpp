#pragma once
#ifdef WIN32
#ifdef UNICODE
#undef UNICODE
#endif

#include "generic.hpp"
#include "neuron/synapse/pre.hpp"

#include <neuron/stem/interfaces.hpp>

#include <Windows.h>
#include <unordered_set>

namespace neuron::synapse {
    class Win32Window;

    class SYNAPSE_API Win32Platform final : public Platform, public stem::Provider<HINSTANCE> {
      public:
         Win32Platform();
        ~Win32Platform() override;

        std::weak_ptr<Window> create_window(const WindowDescription &description) override;
        [[nodiscard]] size_t  get_window_count() const override;
        void                  run_event_loop() override;
        void                  step_event_loop() override;

        [[nodiscard]] inline HINSTANCE hinstance() const { return m_hinstance; };

        inline const HINSTANCE &provide(stem::blank_typed<HINSTANCE>) override { return m_hinstance; };

        void remove_window(HWND hwnd);

#ifdef SYNAPSE_VULKAN_SUPPORT
        const std::vector<const char *> &required_instance_extensions() override;
#endif

        inline std::vector<const char *> vulkan_needed_instance_extensions() override {
#ifdef SYNAPSE_VULKAN_SUPPORT
            return required_instance_extensions();
#else
            return {};
#endif
        };

      private:
        friend class Win32Window;
        HINSTANCE m_hinstance;

        std::unordered_set<HWND>                               m_open_windows;
        std::unordered_map<HWND, std::shared_ptr<Win32Window>> m_window_map;

        static LRESULT CALLBACK _window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };

    class SYNAPSE_API Win32Window final : public Window, public stem::Provider<HWND> {
        friend class Win32Platform;
        explicit Win32Window(const WindowDescription &description);

      public:
        ~Win32Window() override;

        glm::uvec2     get_inner_size() const override;
        void           trigger_close() override;
        glm::ivec2     get_cursor_pos() const override;
        vk::SurfaceKHR create_vulkan_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator) override;

        [[nodiscard]] inline HWND win32_handle() const { return m_hwnd; };

        LRESULT handle_event(UINT msg, WPARAM w_param, LPARAM l_param);

        inline const HWND &provide(stem::blank_typed<HWND>) override { return m_hwnd; };

      private:
        HWND       m_hwnd;
        glm::ivec2 m_last_cursor_pos;
    };
} // namespace neuron::synapse

#endif
