cmake_minimum_required(VERSION 3.10)

project(ST2DEditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(GSL REQUIRED)
find_package(Freetype REQUIRED)

link_directories(${CMAKE_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/Core/Source)
include_directories(${CMAKE_SOURCE_DIR}/Editor/Source)
include_directories(${CMAKE_SOURCE_DIR}/Editor/ThirdParty)

file(GLOB_RECURSE EDITOR_SRC
    "${CMAKE_SOURCE_DIR}/Editor/Source/*.cpp"
    "${CMAKE_SOURCE_DIR}/Editor/Source/*.h"
)

add_executable(ST2DEditor ${EDITOR_SRC})

target_include_directories(ST2DEditor PRIVATE
    ${CMAKE_SOURCE_DIR}/ThirdParty/autodiff
)

target_compile_definitions(ST2DEditor PRIVATE 
    ST_BUILD_EXE
    $<$<CONFIG:Debug>:ST_DEBUG>
    $<$<CONFIG:Release>:ST_RELEASE>
)

target_link_libraries(ST2DEditor PRIVATE 
    ST2DCore
    glad::glad
    glfw
    glm::glm-header-only
    imgui::imgui
    Freetype::Freetype
    GSL::gsl GSL::gslcblas
)

target_precompile_headers(ST2DEditor PUBLIC ../Core/Source/pch.h)

add_custom_command(
    TARGET ST2DEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Editor/Resource $<TARGET_FILE_DIR:ST2DEditor>/Resource
)