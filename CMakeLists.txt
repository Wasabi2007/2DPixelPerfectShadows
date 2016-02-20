cmake_minimum_required(VERSION 3.2)
project(2DShadows)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

add_definitions(-DGLEW_STATIC)
if(WIN32)
    add_definitions(-DWIN32)
endif()

add_library(lodepng libs/lodepng/lodepng.h libs/lodepng/lodepng.cpp libs/lodepng/lodepng_util.h libs/lodepng/lodepng_util.cpp)


if(WIN32)
    include_directories(libs/nanogui/ext/glfw/include)
    add_subdirectory(libs/nanogui/ext/glew)
endif()

#add_subdirectory(libs/glfw)
add_subdirectory(libs/glm)


include_directories(libs/glm)
include_directories(libs/lodepng)

add_subdirectory(libs/nanogui)
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(libs/nanogui/include)
include_directories(libs/nanogui/ext/eigen)
include_directories(libs/nanogui/ext/glew/include)
include_directories(libs/nanogui/ext/nanovg/src)

set(SOURCE_FILES main.cpp src/UI/main_screen.cpp src/UI/main_screen.hpp src/Render/render_engine.cpp src/Render/render_engine.hpp src/ECS/entity_manager.cpp src/ECS/entity_manager.hpp src/Render/shader.cpp src/Render/shader.hpp src/Render/texture.hpp src/Render/model.hpp)
add_executable(2DShadows ${SOURCE_FILES})


set(libs nanogui glm glfw glew)


if(WIN32)
    set(libs ${libs} opengl32 )
else(WIN32)
    set(libs ${libs} GL )
endif(WIN32)

target_link_libraries(2DShadows ${libs} lodepng)

FILE(COPY resources/. DESTINATION .)