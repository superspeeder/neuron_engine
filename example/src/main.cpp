#include "neuron/occipital/occipital.hpp"


#include <neuron/stem/interfaces.hpp>
#include <neuron/synapse/generic.hpp>

#include <iostream>
#include <ostream>

#include <chrono>
#include <glm/gtx/string_cast.hpp>

namespace synapse = neuron::synapse;
namespace occipital = neuron::occipital;

#ifdef WIN32
#include <Windows.h>
void window_test(const std::shared_ptr<synapse::Window>& window) {
    std::cout << "HWND: " << std::hex << reinterpret_cast<size_t>(neuron::stem::provide<HWND>(window)) << std::dec << std::endl;
}
#else
void window_test(const std::shared_ptr<synapse::Window>& window) {
}
#endif

void run() {
    std::shared_ptr<occipital::Occipital> occipital = occipital::Occipital::init(occipital::OccipitalSettings{
        .validation = occipital::FeatureDependencyMode::IfAvailable,
        .surfaceless_query = occipital::FeatureDependencyMode::IfAvailable,
        .allow_non_conformant_devices = true,
    });

    const auto window = synapse::Window::create(synapse::WindowDescription{
        .title     = "Hello!",
        .size      = {800, 600},
        .placement = synapse::WindowCenterMonitor{1},
        // .placement = synapse::WINDOW_PLACEMENT_USE_DEFAULT,
        .resizable = false,
    });

    const auto window2 = synapse::Window::create(synapse::WindowDescription{
        .title     = "Hello!",
        .size      = {800, 600},
        .placement = synapse::WindowCenterMonitor{0},
        // .placement = synapse::WINDOW_PLACEMENT_USE_DEFAULT,
        .resizable = false,
    });

    window_test(window.lock());
    window_test(window2.lock());

    window.lock()->set_on_resize([](const glm::uvec2 &new_size) { std::cout << "Size: " << new_size.x << ", " << new_size.y << std::endl; });

    window2.lock()->set_on_close_request([window](bool *close) {
        if (!window.expired())
            *close = false;
    });

    window.lock()->set_on_close([] { std::cout << "Close" << std::endl; });

    window.lock()->set_on_key_pressed([](const synapse::KeyCode keycode, const synapse::KeyMods &mods, const unsigned int scancode, const bool is_repeat) {
        std::cout << std::hex << static_cast<uint16_t>(keycode) << std::dec << " -- " << is_repeat << std::endl;
    });

    window.lock()->set_on_key_released([window](const synapse::KeyCode keycode, const synapse::KeyMods &mods, const unsigned int scancode) {
        if (keycode == synapse::KeyCode::Escape && mods.shift) {
            window.lock()->trigger_close();
        }
    });

    window2.lock()->set_on_mouse_button_down([window2](const synapse::MouseButton &mouse_button, const glm::ivec2 &pos, const synapse::KeyMods &mods) {
        if (mouse_button == synapse::MouseButton::Right && mods.control && mods.shift) {
            window2.lock()->trigger_close();
        }
    });

    window.lock()->set_on_mouse_moved([](const glm::ivec2 &old_position, const glm::ivec2 &new_position, const glm::ivec2 &delta) {
        // std::cout << "Mouse Moved: " << glm::to_string(old_position) << " -> " << glm::to_string(new_position) << " | " << glm::to_string(delta) << std::endl;
    });

    using clock      = std::chrono::high_resolution_clock;
    using duration   = std::chrono::duration<double>;
    using time_point = std::chrono::time_point<clock, duration>;

    time_point this_frame = clock::now();
    duration   delta_time(1.0 / 60.0);
    time_point last_frame = this_frame - delta_time;

    while (synapse::Platform::get()->get_window_count()) {
        synapse::Platform::get()->step_event_loop();

        last_frame = this_frame;
        while ((clock::now() - last_frame).count() < 1.0 / 60.0) {
            (void)0;
        }
        this_frame = clock::now();
        delta_time = this_frame - last_frame;
    }
}

int main() {
    try {
        synapse::Platform::init();

        run();

        occipital::Occipital::cleanup();
        synapse::Platform::cleanup();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
