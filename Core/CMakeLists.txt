cmake_minimum_required(VERSION 3.10)

project(SFMLTestbedCore)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/Core/Source)

file(GLOB_RECURSE CORE_SRC
    "${CMAKE_SOURCE_DIR}/Core/Source/*.cpp"
    "${CMAKE_SOURCE_DIR}/Core/Source/*.h"
)


add_library(SFMLTestbedCore SHARED ${CORE_SRC})

target_compile_definitions(SFMLTestbedCore PRIVATE 
    ST_BUILD_DLL
    $<$<CONFIG:Debug>:ST_DEBUG>
    $<$<CONFIG:Release>:ST_RELEASE>
)

target_include_directories(SFMLTestbedCore PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
    $<INSTALL_INTERFACE:include>
)


target_link_libraries(SFMLTestbedCore PUBLIC        
        glad::glad
        glfw
        imgui::imgui
        glm::glm-header-only
        spdlog::spdlog_header_only
        ${Stb_INCLUDE_DIR}
)

target_precompile_headers(SFMLTestbedCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source/pch.h)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_SUFFIX "-x64")
else()
    set(PLATFORM_SUFFIX "-x86")
endif()

set_target_properties(SFMLTestbedCore PROPERTIES 
    DEBUG_POSTFIX "${PLATFORM_SUFFIX}d"
    RELEASE_POSTFIX "${PLATFORM_SUFFIX}"
    MINSIZEREL_POSTFIX "${PLATFORM_SUFFIX}"
    RELWITHDEBINFO_POSTFIX "${PLATFORM_SUFFIX}"
    OUTPUT_NAME "SFMLTestbedCore"
    PREFIX ""
)