cmake_minimum_required(VERSION "3.17.2")

project(WeeabooPoint)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_LIBRARY_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

if (!MSVC)
    add_compile_options(-O3)
    add_compile_options(-Wfatal-errors)
    add_compile_options(-fpermissive)
endif()


if (MSVC_IDE)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}")
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}")
endif()

include(FetchContent)
FetchContent_Declare(
	glfw
	GIT_REPOSITORY https://github.com/glfw/glfw
	GIT_TAG	       master
)
FetchContent_MakeAvailable(glfw)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_executable(WeeabooPoint 
	${PROJECT_SOURCE_DIR}/src/main.cpp
	${PROJECT_SOURCE_DIR}/src/gamestate.cpp
	${PROJECT_SOURCE_DIR}/src/geometry.cpp
	${PROJECT_SOURCE_DIR}/src/parser.cpp
	${PROJECT_SOURCE_DIR}/src/scene.cpp
	${PROJECT_SOURCE_DIR}/src/text.cpp
        ${PROJECT_SOURCE_DIR}/src/fileloader.cpp
	${PROJECT_SOURCE_DIR}/src/eventhandler.cpp
	${PROJECT_SOURCE_DIR}/src/graphing.cpp

	${PROJECT_SOURCE_DIR}/src/imgui.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_impl_glfw.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_impl_opengl2.cpp
	${PROJECT_SOURCE_DIR}/src/imgui_widgets.cpp
	
	${PROJECT_SOURCE_DIR}/include/eventhandler.h
	${PROJECT_SOURCE_DIR}/include/eventtypes.h
	${PROJECT_SOURCE_DIR}/include/gamestate.h
	${PROJECT_SOURCE_DIR}/include/geometry.h
	${PROJECT_SOURCE_DIR}/include/globals.h
	${PROJECT_SOURCE_DIR}/include/parser.h
	${PROJECT_SOURCE_DIR}/include/scene.h
	${PROJECT_SOURCE_DIR}/include/text.h
        ${PROJECT_SOURCE_DIR}/include/fileloader.h
	${PROJECT_SOURCE_DIR}/include/graphing.h
	${PROJECT_SOURCE_DIR}/include/utils.h
	)

target_include_directories(WeeabooPoint PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(WeeabooPoint glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(WeeabooPoint OpenGL::GL)

install(TARGETS WeeabooPoint DESTINATION ${PROJECT_SOURCE_DIR}/bin)


