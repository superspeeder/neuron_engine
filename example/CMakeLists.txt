set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(example src/main.cpp)
target_include_directories(example PRIVATE src/)
target_link_libraries(example PRIVATE neuron::neuron)

add_custom_target(run_prep
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:example> $<TARGET_RUNTIME_DLLS:example>
        COMMAND_EXPAND_LISTS
)
