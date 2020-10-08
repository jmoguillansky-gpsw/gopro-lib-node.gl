cmake_minimum_required(VERSION 3.10)
project (NodeGL)

set (NGL_BACKEND_NGFX 1)
if (NGL_BACKEND_NGFX)
    set (NGL_BACKEND ngfx)
elseif (NGL_BACKEND_GL)
    set (NGL_BACKEND gl)
    set (GRAPHICS_BACKEND_LIBS glfw GL GLEW)
    include_directories(porting/glfw)
endif()

file (GLOB_RECURSE NGL_ADAPTER_SOURCE_FILES src/* porting/${NGL_BACKEND}/*)
include_directories(src porting/${NGL_BACKEND} ${SXPLAYER_DIR})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(NODEGL_BUILD_TYPE STATIC)
else()
	set(NODEGL_BUILD_TYPE SHARED)
endif()

set (SXPLAYER_LIBS sxplayer avformat avfilter avcodec avutil pthread)
link_directories (/usr/local/lib)
add_library(NodeGL ${NODEGL_BUILD_TYPE} ${NGL_ADAPTER_SOURCE_FILES})
target_compile_definitions(NodeGL PUBLIC ${GRAPHICS_BACKEND_CFLAGS})
target_compile_options(NodeGL PUBLIC -std=c++17)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set (SXPLAYER_LIBS ${SXPLAYER_LIBS}
    "-framework CoreMedia"
    "-framework CoreVideo"
    "-framework VideoToolbox"
)
endif()
target_link_libraries(NodeGL PUBLIC ngfx ${GRAPHICS_BACKEND_LIBS} ${SXPLAYER_LIBS})
link_directories(${PYTHON_LIB_PATH})

build_test(nglViewer)
build_test(pyShell)
build_test(mediaPlayer)
target_link_libraries(nglViewer NodeGL)
target_include_directories(pyShell PUBLIC ${PYTHON_INCLUDE_PATH})
target_link_libraries(pyShell NodeGL ${PYTHON_LIBS})
target_link_libraries(mediaPlayer ${SXPLAYER_LIBS})
