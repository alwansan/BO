cmake_minimum_required(VERSION 3.22)
project(black_mod_render_lab LANGUAGES CXX)
if(NOT WIN32 OR NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "This lab requires Windows x64 Visual Studio clang-cl")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
foreach(required SDK_ROOT IMGUI_ROOT PREPARED_ROOT)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
add_executable(mod_render_lab
  render_lab.cpp
  "${PREPARED_ROOT}/d3d12_warp.cpp"
  "${SDK_ROOT}/runtime/gpu/shaders.cpp"
  "${SDK_ROOT}/runtime/gpu/vertex.cpp"
  "${SDK_ROOT}/runtime/gpu/texture.cpp"
  "${SDK_ROOT}/runtime/gpu/render_thread.cpp"
  "${SDK_ROOT}/runtime/kernel/memory.cpp"
  "${SDK_ROOT}/runtime/core/log.cpp"
  "${IMGUI_ROOT}/imgui.cpp"
  "${IMGUI_ROOT}/imgui_draw.cpp"
  "${IMGUI_ROOT}/imgui_tables.cpp"
  "${IMGUI_ROOT}/imgui_widgets.cpp"
  "${IMGUI_ROOT}/backends/imgui_impl_win32.cpp"
  "${IMGUI_ROOT}/backends/imgui_impl_dx12.cpp")
target_include_directories(mod_render_lab PRIVATE
  "${SDK_ROOT}/runtime" "${IMGUI_ROOT}" "${CMAKE_CURRENT_SOURCE_DIR}/../room_network")
target_compile_definitions(mod_render_lab PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS)
target_compile_options(mod_render_lab PRIVATE /W3 /EHsc /clang:-msse4.1)
target_link_libraries(mod_render_lab PRIVATE d3d12 dxgi d3dcompiler user32 gdi32 dwmapi imm32)
enable_testing()
add_test(NAME actual_sdk_warp_rasterization COMMAND mod_render_lab)
set_tests_properties(actual_sdk_warp_rasterization PROPERTIES TIMEOUT 180)
