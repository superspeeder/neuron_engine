#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include "win32.hpp"
#include <stdexcept>
#ifdef WIN32
#include <windowsx.h>


#ifdef SYNAPSE_SHARED
HINSTANCE hInst_;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH)
        hInst_ = hInstDLL;
    return TRUE;
}
#endif

namespace neuron::synapse {
    namespace detail {
        static auto CLASSNAME = "neuron_class_name";

        std::unique_ptr<Platform> create_win32_platform() {
            return std::make_unique<Win32Platform>();
        }

        struct keyinfo {
            WORD vkc;
            WORD scancode;
            bool is_extended;
            bool was_down;
            WORD repeat_count;
            bool is_released;
        };

        static KeyCode kcmap[256] = {
            KeyCode::Invalid,        // 0x00
            KeyCode::Invalid,        // 0x01
            KeyCode::Invalid,        // 0x02
            KeyCode::Invalid,        // 0x03
            KeyCode::Invalid,        // 0x04
            KeyCode::Invalid,        // 0x05
            KeyCode::Invalid,        // 0x06
            KeyCode::Invalid,        // 0x07
            KeyCode::Backspace,      // 0x08
            KeyCode::Tab,            // 0x09
            KeyCode::Invalid,        // 0x0A
            KeyCode::Invalid,        // 0x0B
            KeyCode::Invalid,        // 0x0C
            KeyCode::Enter,          // 0x0D
            KeyCode::Invalid,        // 0x0E
            KeyCode::Invalid,        // 0x0F
            KeyCode::Shift,          // 0x10
            KeyCode::Control,        // 0x11
            KeyCode::Alt,            // 0x12
            KeyCode::Pause,          // 0x13
            KeyCode::CapsLock,       // 0x14
            KeyCode::Invalid,        // 0x15
            KeyCode::Invalid,        // 0x16
            KeyCode::Invalid,        // 0x17
            KeyCode::Invalid,        // 0x18
            KeyCode::Invalid,        // 0x19
            KeyCode::Invalid,        // 0x1A
            KeyCode::Escape,         // 0x1B
            KeyCode::Invalid,        // 0x1C
            KeyCode::Invalid,        // 0x1D
            KeyCode::Invalid,        // 0x1E
            KeyCode::Invalid,        // 0x1F
            KeyCode::Space,          // 0x20
            KeyCode::PageUp,         // 0x21
            KeyCode::PageDown,       // 0x22
            KeyCode::End,            // 0x23
            KeyCode::Home,           // 0x24
            KeyCode::Left,           // 0x25
            KeyCode::Up,             // 0x26
            KeyCode::Right,          // 0x27
            KeyCode::Down,           // 0x28
            KeyCode::Invalid,        // 0x29
            KeyCode::Invalid,        // 0x2A
            KeyCode::Invalid,        // 0x2B
            KeyCode::PrintScreen,    // 0x2C
            KeyCode::Insert,         // 0x2D
            KeyCode::Delete,         // 0x2E
            KeyCode::Invalid,        // 0x2F
            KeyCode::Num0,           // 0x30
            KeyCode::Num1,           // 0x31
            KeyCode::Num2,           // 0x32
            KeyCode::Num3,           // 0x33
            KeyCode::Num4,           // 0x34
            KeyCode::Num5,           // 0x35
            KeyCode::Num6,           // 0x36
            KeyCode::Num7,           // 0x37
            KeyCode::Num8,           // 0x38
            KeyCode::Num9,           // 0x39
            KeyCode::Invalid,        // 0x3A
            KeyCode::Invalid,        // 0x3B
            KeyCode::Invalid,        // 0x3C
            KeyCode::Invalid,        // 0x3D
            KeyCode::Invalid,        // 0x3E
            KeyCode::Invalid,        // 0x3F
            KeyCode::Invalid,        // 0x40
            KeyCode::A,              // 0x41
            KeyCode::B,              // 0x42
            KeyCode::C,              // 0x43
            KeyCode::D,              // 0x44
            KeyCode::E,              // 0x45
            KeyCode::F,              // 0x46
            KeyCode::G,              // 0x47
            KeyCode::H,              // 0x48
            KeyCode::I,              // 0x49
            KeyCode::J,              // 0x4A
            KeyCode::K,              // 0x4B
            KeyCode::L,              // 0x4C
            KeyCode::M,              // 0x4D
            KeyCode::N,              // 0x4E
            KeyCode::O,              // 0x4F
            KeyCode::P,              // 0x50
            KeyCode::Q,              // 0x51
            KeyCode::R,              // 0x52
            KeyCode::S,              // 0x53
            KeyCode::T,              // 0x54
            KeyCode::U,              // 0x55
            KeyCode::V,              // 0x56
            KeyCode::W,              // 0x57
            KeyCode::X,              // 0x58
            KeyCode::Y,              // 0x59
            KeyCode::Z,              // 0x5A
            KeyCode::LeftSuper,      // 0x5B
            KeyCode::RightSuper,     // 0x5C
            KeyCode::Invalid,        // 0x5D
            KeyCode::Invalid,        // 0x5E
            KeyCode::Invalid,        // 0x5F
            KeyCode::Numpad0,        // 0x60
            KeyCode::Numpad1,        // 0x61
            KeyCode::Numpad2,        // 0x62
            KeyCode::Numpad3,        // 0x63
            KeyCode::Numpad4,        // 0x64
            KeyCode::Numpad5,        // 0x65
            KeyCode::Numpad6,        // 0x66
            KeyCode::Numpad7,        // 0x67
            KeyCode::Numpad8,        // 0x68
            KeyCode::Numpad9,        // 0x69
            KeyCode::NumpadMultiply, // 0x6A
            KeyCode::NumpadAdd,      // 0x6B
            KeyCode::Invalid,        // 0x6C
            KeyCode::NumpadSubtract, // 0x6D
            KeyCode::NumpadPeriod,   // 0x6E
            KeyCode::NumpadDivide,   // 0x6F
            KeyCode::F1,             // 0x70
            KeyCode::F2,             // 0x71
            KeyCode::F3,             // 0x72
            KeyCode::F4,             // 0x73
            KeyCode::F5,             // 0x74
            KeyCode::F6,             // 0x75
            KeyCode::F7,             // 0x76
            KeyCode::F8,             // 0x77
            KeyCode::F9,             // 0x78
            KeyCode::F10,            // 0x79
            KeyCode::F11,            // 0x7A
            KeyCode::F12,            // 0x7B
            KeyCode::F13,            // 0x7C
            KeyCode::F14,            // 0x7D
            KeyCode::F15,            // 0x7E
            KeyCode::F16,            // 0x7F
            KeyCode::F17,            // 0x80
            KeyCode::F18,            // 0x81
            KeyCode::F19,            // 0x82
            KeyCode::F20,            // 0x83
            KeyCode::F21,            // 0x84
            KeyCode::F22,            // 0x85
            KeyCode::F23,            // 0x86
            KeyCode::F24,            // 0x87
            KeyCode::Invalid,        // 0x88
            KeyCode::Invalid,        // 0x89
            KeyCode::Invalid,        // 0x8A
            KeyCode::Invalid,        // 0x8B
            KeyCode::Invalid,        // 0x8C
            KeyCode::Invalid,        // 0x8D
            KeyCode::Invalid,        // 0x8E
            KeyCode::Invalid,        // 0x8F
            KeyCode::NumLock,        // 0x90
            KeyCode::ScrollLock,     // 0x91
            KeyCode::Invalid,        // 0x92
            KeyCode::Invalid,        // 0x93
            KeyCode::Invalid,        // 0x94
            KeyCode::Invalid,        // 0x95
            KeyCode::Invalid,        // 0x96
            KeyCode::Invalid,        // 0x97
            KeyCode::Invalid,        // 0x98
            KeyCode::Invalid,        // 0x99
            KeyCode::Invalid,        // 0x9A
            KeyCode::Invalid,        // 0x9B
            KeyCode::Invalid,        // 0x9C
            KeyCode::Invalid,        // 0x9D
            KeyCode::Invalid,        // 0x9E
            KeyCode::Invalid,        // 0x9F
            KeyCode::LeftShift,      // 0xA0
            KeyCode::RightShift,     // 0xA1
            KeyCode::LeftControl,    // 0xA2
            KeyCode::RightControl,   // 0xA3
            KeyCode::LeftAlt,        // 0xA4
            KeyCode::RightAlt,       // 0xA5
            KeyCode::Invalid,        // 0xA6
            KeyCode::Invalid,        // 0xA7
            KeyCode::Invalid,        // 0xA8
            KeyCode::Invalid,        // 0xA9
            KeyCode::Invalid,        // 0xAA
            KeyCode::Invalid,        // 0xAB
            KeyCode::Invalid,        // 0xAC
            KeyCode::Invalid,        // 0xAD
            KeyCode::Invalid,        // 0xAE
            KeyCode::Invalid,        // 0xAF
            KeyCode::Invalid,        // 0xB0
            KeyCode::Invalid,        // 0xB1
            KeyCode::Invalid,        // 0xB2
            KeyCode::Invalid,        // 0xB3
            KeyCode::Invalid,        // 0xB4
            KeyCode::Invalid,        // 0xB5
            KeyCode::Invalid,        // 0xB6
            KeyCode::Invalid,        // 0xB7
            KeyCode::Invalid,        // 0xB8
            KeyCode::Invalid,        // 0xB9
            KeyCode::Semicolon,      // 0xBA
            KeyCode::NumpadAdd,      // 0xBB
            KeyCode::Comma,          // 0xBC
            KeyCode::Hyphen,         // 0xBD
            KeyCode::Period,         // 0xBE
            KeyCode::Slash,          // 0xBF
            KeyCode::Grave,          // 0xC0
            KeyCode::Invalid,        // 0xC1
            KeyCode::Invalid,        // 0xC2
            KeyCode::Invalid,        // 0xC3
            KeyCode::Invalid,        // 0xC4
            KeyCode::Invalid,        // 0xC5
            KeyCode::Invalid,        // 0xC6
            KeyCode::Invalid,        // 0xC7
            KeyCode::Invalid,        // 0xC8
            KeyCode::Invalid,        // 0xC9
            KeyCode::Invalid,        // 0xCA
            KeyCode::Invalid,        // 0xCB
            KeyCode::Invalid,        // 0xCC
            KeyCode::Invalid,        // 0xCD
            KeyCode::Invalid,        // 0xCE
            KeyCode::Invalid,        // 0xCF
            KeyCode::Invalid,        // 0xD0
            KeyCode::Invalid,        // 0xD1
            KeyCode::Invalid,        // 0xD2
            KeyCode::Invalid,        // 0xD3
            KeyCode::Invalid,        // 0xD4
            KeyCode::Invalid,        // 0xD5
            KeyCode::Invalid,        // 0xD6
            KeyCode::Invalid,        // 0xD7
            KeyCode::Invalid,        // 0xD8
            KeyCode::Invalid,        // 0xD9
            KeyCode::Invalid,        // 0xDA
            KeyCode::LeftBracket,    // 0xDB
            KeyCode::Backslash,      // 0xDC
            KeyCode::RightBracket,   // 0xDD
            KeyCode::Apostrophe,     // 0xDE
            KeyCode::Invalid,        // 0xDF
            KeyCode::Invalid,        // 0xE0
            KeyCode::Invalid,        // 0xE1
            KeyCode::Invalid,        // 0xE2
            KeyCode::Invalid,        // 0xE3
            KeyCode::Invalid,        // 0xE4
            KeyCode::Invalid,        // 0xE5
            KeyCode::Invalid,        // 0xE6
            KeyCode::Invalid,        // 0xE7
            KeyCode::Invalid,        // 0xE8
            KeyCode::Invalid,        // 0xE9
            KeyCode::Invalid,        // 0xEA
            KeyCode::Invalid,        // 0xEB
            KeyCode::Invalid,        // 0xEC
            KeyCode::Invalid,        // 0xED
            KeyCode::Invalid,        // 0xEE
            KeyCode::Invalid,        // 0xEF
            KeyCode::Invalid,        // 0xF0
            KeyCode::Invalid,        // 0xF1
            KeyCode::Invalid,        // 0xF2
            KeyCode::Invalid,        // 0xF3
            KeyCode::Invalid,        // 0xF4
            KeyCode::Invalid,        // 0xF5
            KeyCode::Invalid,        // 0xF6
            KeyCode::Invalid,        // 0xF7
            KeyCode::Invalid,        // 0xF8
            KeyCode::Invalid,        // 0xF9
            KeyCode::Invalid,        // 0xFA
            KeyCode::Invalid,        // 0xFB
            KeyCode::Invalid,        // 0xFC
            KeyCode::Invalid,        // 0xFD
            KeyCode::Invalid,        // 0xFE
            KeyCode::Invalid,        // 0xFF
        };

        keyinfo _parsekeyinfo(WPARAM wparam, LPARAM lparam) {
            keyinfo ki{};

            ki.vkc         = LOWORD(wparam);
            WORD key_flags = HIWORD(lparam);
            ki.scancode    = LOBYTE(key_flags);
            ki.is_extended = (key_flags & KF_EXTENDED) == KF_EXTENDED;

            if (ki.is_extended) {
                ki.scancode = MAKEWORD(ki.scancode, 0xE0);
            }

            ki.was_down     = (key_flags & KF_REPEAT) == KF_REPEAT;
            ki.repeat_count = LOWORD(lparam);
            ki.is_released  = (key_flags & KF_UP) == KF_UP;

            switch (ki.vkc) {
            case VK_SHIFT:
            case VK_CONTROL:
            case VK_MENU:
                ki.vkc = LOWORD(MapVirtualKeyA(ki.scancode, MAPVK_VSC_TO_VK_EX));
                break;
            }

            return ki;
        }

        KeyCode _trvkc(WORD vkc) {
            if (vkc <= 0xFF) {
                return kcmap[vkc];
            }
            return KeyCode::Invalid;
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


#ifdef SYNAPSE_VULKAN_SUPPORT
    const std::vector<const char *> &Win32Platform::required_instance_extensions() {
        static std::vector<const char*> REQUIRED_EXTS = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME};
        return REQUIRED_EXTS;
    }
#endif

    LRESULT Win32Platform::_window_proc(HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
        if (hwnd) {
            LONG_PTR lp = GetWindowLongPtrA(hwnd, GWLP_USERDATA);
            if (lp) {
                auto *w = reinterpret_cast<Win32Window *>(lp);
                return w->handle_event(uMsg, wParam, lParam);
            }
        }

        if (uMsg == WM_CREATE) {
            auto *cs = reinterpret_cast<CREATESTRUCTA *>(lParam);
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        }

        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }

    struct _wpdata {
        unsigned int target_index;
        unsigned int current_index = 0;
        HMONITOR     monitor;
        HMONITOR     first_monitor = nullptr;
        bool         at            = false;
    };

    void _getmcenter(HMONITOR hmon, int *x, int *y, const glm::uvec2 &wsz) {
        MONITORINFOEXA mi{};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoA(hmon, &mi);

        int w  = mi.rcMonitor.right - mi.rcMonitor.left;
        int h  = mi.rcMonitor.bottom - mi.rcMonitor.top;
        int mx = mi.rcMonitor.left;
        int my = mi.rcMonitor.top;

        *x = (w - wsz.x) / 2 + mx;
        *y = (h - wsz.y) / 2 + my;
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
        case WindowPlacementMethod::CENTER_MONITOR: {
            // TODO monitor gather better way
            HDC     hdc = GetDC(nullptr);
            _wpdata wpdata{.target_index = static_cast<unsigned int>(std::get<WindowCenterMonitor>(description.placement).monitor_index)};
            EnumDisplayMonitors(
                hdc, nullptr,
                +[](HMONITOR hmonitor, HDC hdc, LPRECT lprect, LPARAM lparam) {
                    auto *wpdata = reinterpret_cast<_wpdata *>(lparam);
                    if (wpdata->first_monitor == nullptr) {
                        wpdata->first_monitor = hmonitor;
                    }
                    if (wpdata->current_index == wpdata->target_index) {
                        wpdata->at      = true;
                        wpdata->monitor = hmonitor;
                        return FALSE;
                    }

                    wpdata->current_index++;
                    return TRUE;
                },
                reinterpret_cast<LPARAM>(&wpdata));
            if (!wpdata.at) {
                if (wpdata.first_monitor == nullptr) {
                    x = y = CW_USEDEFAULT;
                } else {
                    _getmcenter(wpdata.first_monitor, &x, &y, description.size);
                }
            } else {
                _getmcenter(wpdata.monitor, &x, &y, description.size);
            }
        } break;
        case WindowPlacementMethod::CENTER_HOVERED_MONITOR: {
            POINT mp;
            GetCursorPos(&mp);
            HMONITOR m = MonitorFromPoint(mp, MONITOR_DEFAULTTOPRIMARY);
            _getmcenter(m, &x, &y, description.size);
        } break;
        }

        m_hwnd = CreateWindowExA(ex_style, detail::CLASSNAME, description.title.c_str(), style, x, y, description.size.x, description.size.y, nullptr, nullptr,
                                 _get_platform()->hinstance(), this);
        ShowWindow(m_hwnd, SW_NORMAL);
        m_last_cursor_pos = get_cursor_pos();
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

    glm::ivec2 Win32Window::get_cursor_pos() const {
        POINT p;
        GetCursorPos(&p);
        return {p.x, p.y};
    }


    vk::SurfaceKHR Win32Window::create_vulkan_surface(const vk::Instance instance, const vk::AllocationCallbacks *allocator) {
        const auto vkCreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(instance.getProcAddr("vkCreateWin32SurfaceKHR", vk::getDispatchLoaderStatic()));
        VkSurfaceKHR surface;
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.pNext = nullptr;
        createInfo.hinstance = _get_platform()->hinstance();
        createInfo.hwnd = m_hwnd;
        createInfo.flags = 0;
        vkCreateWin32SurfaceKHR(instance, &createInfo, &static_cast<const VkAllocationCallbacks&>(*allocator), &surface);
        return surface;
    }

    KeyMods _getmods() {
        KeyMods mods{};
        if (GetKeyState(VK_SHIFT) & 0x8000) {
            mods.shift = true;
        }

        if (GetKeyState(VK_CONTROL) & 0x8000) {
            mods.control = true;
        }

        if (GetKeyState(VK_MENU) & 0x8000) {
            mods.alt = true;
        }

        if (GetKeyState(VK_LWIN) & 0x8000 || GetKeyState(VK_RWIN) & 0x8000) {
            mods.super = true;
        }

        if (GetKeyState(VK_CAPITAL) & 0x0001) {
            mods.caps_lock = true;
        }

        if (GetKeyState(VK_NUMLOCK) & 0x0001) {
            mods.num_lock = true;
        }
        return mods;
    }

    LRESULT Win32Window::handle_event(const UINT msg, const WPARAM w_param, const LPARAM l_param) {
        switch (msg) {
        case WM_CLOSE: {
            bool should_close = true;
            call_close_request_callback(&should_close);
            if (should_close) {
                call_close_callback();
                DestroyWindow(m_hwnd);
            }
        }
            return 0;
        case WM_DESTROY:
            _get_platform()->remove_window(m_hwnd);
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP: {
            detail::keyinfo ki   = detail::_parsekeyinfo(w_param, l_param);
            KeyCode         kc   = detail::_trvkc(ki.vkc);
            KeyMods         mods = _getmods();
            mods.alt             = msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP;

            if (ki.is_released) {
                call_key_released_callback(kc, mods, ki.scancode);
            } else {
                call_key_pressed_callback(kc, mods, ki.scancode, ki.was_down);
            }
        } break;
        case WM_SIZE: {
            const WORD w = LOWORD(l_param);
            const WORD h = HIWORD(l_param);
            call_resize_callback(glm::uvec2(w, h));
        } break;
        case WM_MOUSEMOVE: {
            const auto p = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            call_mouse_moved_callback(m_last_cursor_pos, p, p - m_last_cursor_pos);
            m_last_cursor_pos = p;
        } break;
        case WM_LBUTTONDOWN: {
            const auto    p    = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            const KeyMods mods = _getmods();
            call_mouse_button_down_callback(MouseButton::Left, p, mods);
        } break;
        case WM_MBUTTONDOWN: {
            const auto    p    = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            const KeyMods mods = _getmods();
            call_mouse_button_down_callback(MouseButton::Middle, p, mods);
        } break;
        case WM_RBUTTONDOWN: {
            const auto    p    = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            const KeyMods mods = _getmods();
            call_mouse_button_down_callback(MouseButton::Right, p, mods);
        } break;
        case WM_XBUTTONDOWN: {
            const auto    p    = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            const KeyMods mods = _getmods();
            if (HIWORD(w_param) == XBUTTON1) {
                call_mouse_button_down_callback(MouseButton::Button4, p, mods);
            } else {
                call_mouse_button_down_callback(MouseButton::Button5, p, mods);
            }
        } break;
        case WM_LBUTTONUP: {
            const auto p = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            call_mouse_button_up_callback(MouseButton::Left, p);
        } break;
        case WM_MBUTTONUP: {
            const auto p = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            call_mouse_button_up_callback(MouseButton::Middle, p);
        } break;
        case WM_RBUTTONUP: {
            const auto p = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};
            call_mouse_button_up_callback(MouseButton::Right, p);
        } break;
        case WM_XBUTTONUP: {
            const auto p = glm::ivec2{GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)};

            if (HIWORD(w_param) == XBUTTON1) {
                call_mouse_button_up_callback(MouseButton::Button4, p);
            } else {
                call_mouse_button_up_callback(MouseButton::Button5, p);
            }
        } break;
        case WM_MOUSELEAVE: {
            call_mouse_leave_callback();
        } break;
        // case WM_MOUSEHOVER: {} // TODO: understand how this works
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
