cmake_minimum_required(VERSION 3.10)

project(ST2DEditor)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML CONFIG REQUIRED COMPONENTS graphics system window)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(Ceres CONFIG REQUIRED)

link_directories(${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/Core/Source)
include_directories(${CMAKE_SOURCE_DIR}/Editor/Source)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)

file(GLOB_RECURSE EDITOR_SRC
    "${CMAKE_SOURCE_DIR}/Editor/Source/*.cpp"
    "${CMAKE_SOURCE_DIR}/Editor/Source/*.h"
)

add_executable(ST2DEditor ${EDITOR_SRC})

target_compile_definitions(ST2DEditor PRIVATE 
    ST_BUILD_EDITOR
    $<$<CONFIG:Debug>:ST_DEBUG>
    $<$<CONFIG:Release>:ST_RELEASE>
)

target_link_libraries(ST2DEditor PRIVATE 
    ST2DCore
    autodiff::autodiff
    sfml-system sfml-network sfml-graphics sfml-window
    ImGui-SFML::ImGui-SFML
    Eigen3::Eigen
    Ceres::ceres
)

target_precompile_headers(ST2DEditor PUBLIC ../Core/Source/pch.h)

add_custom_command(
    TARGET ST2DEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Editor/Resource $<TARGET_FILE_DIR:ST2DEditor>/Resource
)