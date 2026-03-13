cmake_minimum_required(VERSION 3.20)

project(InjectorBridge LANGUAGES CXX)

if(NOT WIN32)
    message(FATAL_ERROR "InjectorBridge must be built on Windows with a Visual Studio generator.")
endif()

add_executable(injector_bridge
    bridge.cpp
)

target_compile_features(injector_bridge PRIVATE cxx_std_17)

target_include_directories(injector_bridge PRIVATE
    "${CMAKE_SOURCE_DIR}/Memproc"
)

target_link_libraries(injector_bridge PRIVATE
    "${CMAKE_SOURCE_DIR}/Memproc/vmm.lib"
    "${CMAKE_SOURCE_DIR}/Memproc/leechcore.lib"
)
