# EngineRuntime/CMakeLists.txt
file(GLOB_RECURSE RUNTIME_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_executable(EngineRuntime ${RUNTIME_SOURCES})

target_include_directories(EngineRuntime PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(EngineRuntime PRIVATE EngineCore)

add_custom_command(TARGET EngineRuntime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/glfw/bin/glfw3.dll"
        "$<TARGET_FILE_DIR:EngineRuntime>"
)