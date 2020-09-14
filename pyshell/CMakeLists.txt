cmake_minimum_required(VERSION 3.10)
project (pyshell)

file (GLOB_RECURSE PYSHELL_SRC_FILES *.cpp)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set (PYTHON_INCLUDE_PATH /usr/include/python3.6m)
        set (PYTHON_LIBS python3.6)
        set (PYTHON_LIB_PATH /usr/lib/python3.6/config-3.6m-x86_64-linux-gnu/)
endif()

link_directories(${PYTHON_LIB_PATH})
add_executable(pyshell ${PYSHELL_SRC_FILES})
target_include_directories(pyshell PUBLIC ${PYTHON_INCLUDE_PATH})
target_link_libraries(pyshell PUBLIC ${PYTHON_LIBS})
