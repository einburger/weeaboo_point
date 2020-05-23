cmake_minimum_required(VERSION "3.17.2")

project(WeeabooPoint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_compile_options(-Wfatal-errors)
# add_compile_options(-O3)
# add_compile_options(-Wpedantic)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/.cpp)

add_executable(WeeabooPoint 
	${PROJECT_SOURCE_DIR}/src/main.cpp
	${PROJECT_SOURCE_DIR}/src/gamestate.cpp
	${PROJECT_SOURCE_DIR}/src/geometry.cpp
	${PROJECT_SOURCE_DIR}/src/parser.cpp
	${PROJECT_SOURCE_DIR}/src/scene.cpp
	${PROJECT_SOURCE_DIR}/src/text.cpp

	${PROJECT_SOURCE_DIR}/src/imgui.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_impl_glfw.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_impl_opengl2.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_widgets.cpp
	)

target_include_directories(WeeabooPoint PUBLIC ${PROJECT_SOURCE_DIR}/includes
                                        PUBLIC ${PROJECT_SOURCE_DIR}/imgui)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glfw)
target_link_libraries(WeeabooPoint glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(WeeabooPoint OpenGL::GL)

install(TARGETS WeeabooPoint DESTINATION ${PROJECT_SOURCE_DIR}/bin)


