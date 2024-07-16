cmake_minimum_required(VERSION 3.10)

project(SFMLTestbedEditor)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML CONFIG REQUIRED COMPONENTS graphics system window)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Ceres CONFIG REQUIRED)

link_directories(${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/Core/Source)
include_directories(${CMAKE_SOURCE_DIR}/Core/)
include_directories(${CMAKE_SOURCE_DIR}/Source/)

file(GLOB_RECURSE EDITOR_SRC
    "${CMAKE_SOURCE_DIR}/Editor/Source/*.cpp"
    "${CMAKE_SOURCE_DIR}/Editor/Source/*.h"
)

add_executable(SFMLTestbedEditor ${EDITOR_SRC})

target_compile_definitions(SFMLTestbedEditor PRIVATE 
    ST_BUILD_EDITOR
    $<$<CONFIG:Debug>:ST_DEBUG>
    $<$<CONFIG:Release>:ST_RELEASE>
)

target_link_libraries(SFMLTestbedEditor PRIVATE 
    SFMLTestbedCore
    
    sfml-system sfml-network sfml-graphics sfml-window
    ImGui-SFML::ImGui-SFML
    Eigen3::Eigen
    Ceres::ceres
)

target_precompile_headers(SFMLTestbedEditor PUBLIC ../Core/Source/pch.h)

add_custom_command(
    TARGET SFMLTestbedEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Editor/Resource $<TARGET_FILE_DIR:SFMLTestbedEditor>/Resource
)