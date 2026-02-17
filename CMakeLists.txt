cmake_minimum_required(VERSION 3.21)
project(DyingAnima
  VERSION 1.0
  LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(box2d CONFIG REQUIRED)
find_package(Freetype CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)

add_executable(DyingAnima)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
  src/*.c
  src/**/*.c
  src/*.cc
  src/**/*.cc
  include/*.cc
  include/**/*.cc
)
target_sources(DyingAnima PRIVATE ${SOURCE_FILES})
target_include_directories(DyingAnima PRIVATE include)
target_compile_definitions(DyingAnima PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
)
if (MSVC)
  target_compile_options(DyingAnima PRIVATE
    $<$<CONFIG:Debug>:/Zi /Od /W4>
    $<$<CONFIG:Release>:/O2>
  )
else()
  target_compile_options(DyingAnima PRIVATE
    $<$<CONFIG:Debug>:-O0 -g -Wall -Wextra>
    $<$<CONFIG:Release>:-O3>
  )
endif()
target_link_libraries(DyingAnima PRIVATE
  box2d::box2d
  Freetype::Freetype
  glad::glad
  glfw
  glm::glm
  OpenGL::GL
  imgui::imgui
  pugixml::pugixml
  EnTT::EnTT
)
set_target_properties(DyingAnima PROPERTIES
  OUTPUT_NAME "Dying Anima"
)
add_custom_command(TARGET DyingAnima POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:DyingAnima>/assets
  COMMENT "Copying assets folder to build directory"
)
install(TARGETS DyingAnima
  RUNTIME DESTINATION bin
)
