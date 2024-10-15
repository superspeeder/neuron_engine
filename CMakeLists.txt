cmake_minimum_required(VERSION 3.26)
project(neuron VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to specify custom path to glslc executable
set(NEURON_GLSLC_PATH "" CACHE STRING "Custom path to the glslc executable")

# Include FetchContent for external dependencies
include(FetchContent)

# Preserve original BUILD_SHARED_LIBS value
set(O_BSL ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)

# Fetch glm library
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

# Restore original BUILD_SHARED_LIBS value
set(BUILD_SHARED_LIBS ${O_BSL})
unset(O_BSL)

# Gather generic source files (excluding platform-specific)
file(GLOB_RECURSE GENERIC_SRCS CONFIGURE_DEPENDS
        src/neuron/generic.cpp
        src/neuron/*.cxx
        src/neuron/*.cc
        src/*.c
        src/*.s
)

# Initialize PLATFORM_SRCS as empty
set(PLATFORM_SRCS)

# Add platform-specific source files based on the target OS
if(APPLE)
    list(APPEND PLATFORM_SRCS
            src/neuron/platform/macos.mm
            src/neuron/platform/cocoas.mm
    )
elseif(UNIX)
    list(APPEND PLATFORM_SRCS
            src/neuron/platform/linux.cpp
    )
elseif(WIN32)
    list(APPEND PLATFORM_SRCS
            src/neuron/platform/win32.cpp
    )
else()
    message(FATAL_ERROR "Unsupported system")
endif()

# Combine all source files
set(SRCS ${GENERIC_SRCS} ${PLATFORM_SRCS})

# Create the neuron library
add_library(neuron ${SRCS})

# Include directories
target_include_directories(neuron PUBLIC src/) # TODO: consider using an include/ folder which contains the interface and changing the src folder to PRIVATE
target_include_directories(neuron PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/cfi)

# Link external libraries
target_link_libraries(neuron PUBLIC glm::glm)

# Enable GLM experimental features
target_compile_definitions(neuron PUBLIC GLM_ENABLE_EXPERIMENTAL)

# Link platform-specific libraries
if(APPLE)
    target_link_libraries(neuron PUBLIC "-framework Cocoa" "-framework IOKit" "-framework CoreFoundation")
elseif(UNIX)
    target_link_libraries(neuron PUBLIC X11 Xrandr Xinerama Xi Xcursor)
elseif(WIN32)
    target_link_libraries(neuron PUBLIC Dwmapi)
endif()

# Find Vulkan package
find_package(Vulkan REQUIRED)
target_link_libraries(neuron PUBLIC Vulkan::Vulkan)

# Handle glslc executable
if (NOT (NEURON_GLSLC_PATH STREQUAL ""))
    set(GLSLC_FOUND 1)
    set(GLSLC ${NEURON_GLSLC_PATH})
else()
    set(GLSLC_FOUND 0)

    if (Vulkan_glslc_FOUND)
        set(GLSLC Vulkan::glslc)
        set(GLSLC_FOUND 1)
    else()
        find_program(GLSLC glslc NAMES glslc.exe HINTS ENV VULKAN_SDK PATH_SUFFIXES Bin bin)
        if (GLSLC STREQUAL "GLSLC-NOTFOUND")
            message(WARNING "Failed to find GLSLC. Shaders must be compiled manually or a path to GLSLC should be specified via the NEURON_GLSLC_PATH variable")
        else()
            set(GLSLC_FOUND 1)
        endif()
    endif()
endif()

# Handle shaderc libraries
if (Vulkan_shaderc_combined_FOUND)
    target_link_libraries(neuron PUBLIC Vulkan::shaderc_combined)
else()
    if (BUILD_SHARED_LIBS)
        find_library(SHADERC_SHARED_PATH shaderc_shared HINTS ENV VULKAN_SDK PATH_SUFFIXES Lib lib REQUIRED)
        target_link_libraries(neuron PUBLIC ${SHADERC_SHARED_PATH})
    else()
        find_library(SHADERC_COMBINED_PATH shaderc_combined HINTS ENV VULKAN_SDK PATH_SUFFIXES Lib lib REQUIRED)
        target_link_libraries(neuron PUBLIC ${SHADERC_COMBINED_PATH})
    endif()

    find_path(SHADERC_INCLUDE_DIR shaderc/shaderc.hpp HINTS ENV VULKAN_SDK PATH_SUFFIXES Include include REQUIRED)
    target_include_directories(neuron PUBLIC ${SHADERC_INCLUDE_DIR})
endif()

# Define shared library symbols if building shared libraries
if (BUILD_SHARED_LIBS)
    target_compile_definitions(neuron PUBLIC NEURON_SHARED)
    target_compile_definitions(neuron PRIVATE NEURON_SHARED_EXPORTS)
endif()

# Create an alias for the neuron library
add_library(neuron::neuron ALIAS neuron)

# Add example subdirectory
add_subdirectory(example)