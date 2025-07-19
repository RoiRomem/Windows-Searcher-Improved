cmake_minimum_required(VERSION 3.10)
project(WSI)

set(CMAKE_CXX_STANDARD 20)
set(MAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

include_directories(
  external/imgui
  external/imgui/backends
)

file(GLOB IMGUI_SRC
  external/imgui/*.cpp
  external/imgui/backends/imgui_impl_win32.cpp
  external/imgui/backends/imgui_impl_dx11.cpp
)

file(GLOB_RECURSE SRC
  src/*.cpp
)

add_executable(WSI WIN32 ${SRC} ${IMGUI_SRC})

target_link_libraries(WSI
        d3d11
        dxgi
        d3dcompiler
        dwmapi
        imm32
        winmm
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
)
