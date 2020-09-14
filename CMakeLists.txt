cmake_minimum_required(VERSION 3.10)
project (gopro-nodegl)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set (SXPLAYER_LIBS sxplayer avformat avfilter avcodec avutil pthread)

add_subdirectory(libnodegl)
add_subdirectory(pyshell)
