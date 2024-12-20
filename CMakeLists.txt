cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_STANDARD 99)
project(VDL C)

# Create the shared library target
add_library(VDL SHARED
        # Source
        src/system/gnu_linux/linux_audio.c
        src/system/gnu_linux/linux_video.c
        src/system/terminal.c
        src/system/Windows/WIN_audio.c
        src/system/Windows/WIN_video.c
        src/system/Windows/WIN_io.c

        # Headers
        include/terminal.h
        include/audio.h
        include/vdl.h

        # STB
        include/stb_easy_font.h
        include/stb_image.h

)

if (UNIX AND NOT APPLE)
  find_package(OpenGL REQUIRED)
  target_link_libraries(VDL PRIVATE X11 OpenGL::GL m)
elseif(WIN32)
  target_link_libraries(VDL winmm opengl32 gdi32)
endif ()

add_executable(vdl_test main.c)

# Link VDL to vdl_test executable
target_link_libraries(vdl_test PRIVATE VDL m)

if (UNIX AND NOT APPLE)
	  target_link_libraries(vdl_test PRIVATE OpenGL::GL X11)
endif ()

#[[ 
Copyright (C) 2024 Ellouze Adam <elzadam11@tutamail.com>
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
]]

