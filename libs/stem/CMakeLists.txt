cmake_minimum_required(VERSION 3.29)
project(neuron_stem VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)

# glm must be built as a static library.
set(NEURON_SHARED_BUILD ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

FetchContent_Declare(
        VulkanHeaders
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers
        GIT_TAG        v1.3.298
)

FetchContent_MakeAvailable(glm VulkanHeaders)
set(BUILD_SHARED_LIBS ${NEURON_SHARED_BUILD})

add_library(neuron_stem src/neuron/stem/stem.cpp
        src/neuron/stem/stem.hpp
        src/neuron/stem/pre.hpp
        src/neuron/stem/interfaces.hpp)
target_include_directories(neuron_stem PUBLIC src/)
target_link_libraries(neuron_stem PUBLIC glm::glm Vulkan::Headers)
target_compile_definitions(neuron_stem PUBLIC -DGLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(neuron_stem PUBLIC NEURON_STEM_VERSION="${neuron_stem_VERSION}" NEURON_STEM_VERSION_MAJOR=${neuron_stem_VERSION_MAJOR} NEURON_STEM_VERSION_MINOR=${neuron_stem_VERSION_MINOR} NEURON_STEM_VERSION_PATCH=${neuron_stem_VERSION_PATCH})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(neuron_stem PUBLIC -DNEURON_STEM_SHARED)
    target_compile_definitions(neuron_stem PRIVATE -DNEURON_STEM_SHARED_EXPORTS)
endif()

if (WIN32)
    target_compile_definitions(neuron_stem PUBLIC -DNOMINMAX) # fuck you windows why do you make min and max a macro
endif()

add_library(neuron::stem ALIAS neuron_stem)
