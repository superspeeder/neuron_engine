#include "win32.hpp"
#include <stdexcept>
#ifdef WIN32

#ifdef SYNAPSE_SHARED
HINSTANCE hInst_;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) hInst_ = hInstDLL;
    return TRUE;
}
#endif

namespace neuron::synapse {
    namespace detail {
        static auto CLASSNAME = "neuron_class_name";

        std::unique_ptr<Platform> create_win32_platform() {
            return std::make_unique<Win32Platform>();
        }
    } // namespace detail

    inline Win32Platform *_get_platform() {
        return dynamic_cast<Win32Platform *>(Platform::get().get());
    }

    Win32Platform::Win32Platform() {
#ifdef SYNAPSE_SHARED
        m_hinstance = hInst_;
#else
        m_hinstance = GetModuleHandleA(NULL);
#endif
        // printf("hinst: %p\n", m_hinstance);


        WNDCLASSEXA wc{};
        wc.cbSize        = sizeof(wc);
        wc.style         = CS_VREDRAW | CS_HREDRAW;
        wc.lpfnWndProc   = Win32Platform::_window_proc;
        wc.hInstance     = m_hinstance;
        wc.lpszClassName = detail::CLASSNAME;
        RegisterClassExA(&wc);
    }

    Win32Platform::~Win32Platform() {
        UnregisterClassA(detail::CLASSNAME, m_hinstance);
    }

    std::weak_ptr<Window> Win32Platform::create_window(const WindowDescription &description) {
        auto window = std::shared_ptr<Win32Window>(new Win32Window(description));
        m_open_windows.insert(window->m_hwnd);
        m_window_map.insert({window->m_hwnd, window});
        return window;
    }

    size_t Win32Platform::get_window_count() const {
        return m_open_windows.size();
    }

    void Win32Platform::run_event_loop() {
        MSG msg{};
        while (!m_open_windows.empty() && GetMessageA(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    void Win32Platform::step_event_loop() {
        MSG msg{};
        while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                // bye bye ocean
                m_open_windows.clear();
                m_window_map.clear();
                return;
            }

            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    void Win32Platform::remove_window(HWND hwnd) {
        m_open_windows.erase(hwnd);
        m_window_map.erase(hwnd);
    }

    LRESULT Win32Platform::_window_proc(HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
        if (hwnd) {
            LONG_PTR lp = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
            if (lp) {
                auto *w = reinterpret_cast<Win32Window *>(lp);
                return w->handle_event(uMsg, wParam, lParam);
            }
        }

        if (uMsg == WM_CREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        }

        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }

    Win32Window::Win32Window(const WindowDescription &description) {
        DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;
        DWORD style    = WS_OVERLAPPEDWINDOW;

        int x, y;

        switch (static_cast<WindowPlacementMethod>(description.placement.index())) {
        case WindowPlacementMethod::USE_DEFAULT:
            x = y = CW_USEDEFAULT;
            break;
        case WindowPlacementMethod::FIXED_POSITION: {
            const auto &p = std::get<WindowFixedPosition>(description.placement).position;
            x             = p.x;
            y             = p.y;
        }; break;
        case WindowPlacementMethod::CENTER_MONITOR:
            // TODO
            break;
        case WindowPlacementMethod::CENTER_HOVERED_MONITOR:
            break;
        }

        m_hwnd = CreateWindowExA(ex_style, detail::CLASSNAME, description.title.c_str(), style, x, y, description.size.x, description.size.y, nullptr, nullptr,
                                 _get_platform()->hinstance(), this);
        ShowWindow(m_hwnd, SW_NORMAL);
    }

    Win32Window::~Win32Window() {
        DestroyWindow(m_hwnd);
    }

    glm::uvec2 Win32Window::get_inner_size() const {
        RECT r;
        GetClientRect(m_hwnd, &r);
        return {r.right - r.left, r.bottom - r.top};
    }

    void Win32Window::trigger_close() {
        SendMessageA(m_hwnd, WM_CLOSE, 0, 0);
    }

    LRESULT Win32Window::handle_event(const UINT msg, const WPARAM w_param, const LPARAM l_param) {
        switch (msg) {
        case WM_CLOSE:
            DestroyWindow(m_hwnd);
            return 0;
        case WM_DESTROY:
            _get_platform()->remove_window(m_hwnd);
            return 0;
        }
        return DefWindowProcA(m_hwnd, msg, w_param, l_param);
    }
} // namespace neuron::synapse

#else
// define neuron::detail::create_win32_platform as a runtime error

#include "neuron/synapse/generic.hpp"
#include <memory>
#include <stdexcept>

namespace neuron::synapse::detail {
    std::unique_ptr<Platform> create_win32_platform() {
        throw std::runtime_error("Win32 platform can only be created in Windows.");
    }
} // namespace neuron::synapse::detail


#endif
