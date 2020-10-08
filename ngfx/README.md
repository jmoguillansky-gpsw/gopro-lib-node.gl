# NGFX Graphics Abstraction API
NGFX is a low-level graphics abstraction API on top of Vulkan, DirectX12, and Metal.  
It exposes the benefits of next-generation graphics technology via a common
platform abstraction API.  It also supports optional access to the backend data structures,
enabling platform specific optimizations.

## Architecture

The graphics platform features a layered architecture.  
<img src="doc/NodeGFX-High Level Architecture.svg">

## Build Instructions

1) Install Dependencies  
   On Vulkan Platform (primarily for Linux): Install Vulkan SDK  
   On Windows: Install DirectX 12 SDK
   Install glew, glfw, stb_image
   On Linux: use apt for debian-based systems, or pacman for arch-linux based systems
   	for debian based: apt-get install glew glfw (etc.)
	for archlinux based: pacman -S glew glfw (etc.)
   On MacOS: use brew package manager
   
   For stb_image: may need to download the header manually from https://github.com/nothings/stb/blob/master/stb_image.h
   
2) set environment:  
	For Vulkan Platform:  
		source <VULKAN_SDK_PATH>/setup-env.sh  
		Ensure the following environment variables are set: PATH, VULKAN_SDK, VK_LAYER_PATH, LD_LIBRARY_PATH  
	On Windows:  
		(Optional) Install Windows Subsystem for Linux.  

3) Install SDK
	For Vulkan Platform:  
		Install Vulkan SDK to system directories:  
		see https://vulkan.lunarg.com/doc/view/1.1.106.0/linux/getting_started.html  
		
4) Compile shaders:  
	python3 scripts/compile_shaders_<backend>.py
	 
5) Run CMake to generate makefiles  
	From top-level directory run the helper script: bash scripts/run_cmake_[option].sh  
	This runs the following command:  
		on Linux: cmake -H. -Bcmake-build-debug -DGRAPHICS_BACKEND_VULKAN=ON  
		on Windows: cmake.exe -H. -Bcmake-build-debug -G "Visual Studio 16 2019" -A x64 -DGRAPHICS_BACKEND_DIRECTX12=ON (or -DGRAPHICS_BACKEND_VULKAN=ON)  
		on MacOS: cmake -H. -Bcmake-build-debug -G Xcode -DGRAPHICS_BACKEND_METAL=ON (or -DGRAPHICS_BACKEND_VULKAN=ON)  
	
	For release build:  
	cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -DGRAPHICS_BACKEND_[option]=ON (where option is VULKAN, METAL, or DIRECTX12)
	
6)  Build project

	Using IDE  
	On Linux: recommend Qt Creator  
	On Windows: recommend Visual Studio 2019  
	On MacOS: recommend XCode  
	
	From command line:  
	cmake --build cmake-build-debug  
	
	For release build:  
	cmake --build cmake-build-release --config Release  
	
7) Copy resource files to build folder  
	bash scripts/copy_data.sh  

## Programming Guide
See: https://github.com/gopro/personal--graphics-engine/blob/master/doc/NodeGFX%20Programming%20Guide.docx?raw=true


## References  

Vulkan Examples / Tutorials:   
	https://github.com/SaschaWillems/Vulkan/  
	https://raw.githubusercontent.com/ARM-software/vulkan_best_practice_for_mobile_developers/  
DirectX12 Examples:  
	https://github.com/Microsoft/DirectX-Graphics-Samples  
Metal Examples:  
	https://developer.apple.com/metal/sample-code/  
Debugging Tools:  
	https://renderdoc.org/  
