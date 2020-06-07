cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{DOLCESDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DOLCESDK}/share/dolce.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define DOLCESDK to point to your SDK path!")
  endif()
endif()

project(superstoner)
include("${DOLCESDK}/share/dolce.cmake" REQUIRED)

set(VITA_APP_NAME "Super Stoner 420")
set(VITA_TITLEID  "WEED42069")
set(VITA_VERSION  "04.20")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D FOR_VITA -D HAS_MIXER -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D FOR_VITA -D HAS_MIXER -std=c++17")

include_directories(
)
file(GLOB cpp_files
    "src/*.c"
)
add_executable(${PROJECT_NAME}
  ${cpp_files}
)

target_link_libraries(${PROJECT_NAME}
  SDL2_ttf
  SDL2_mixer
  SDL2_image
  SDL2
  FLAC
  freetype
  mikmod
  mpg123
  jpeg
  png
  vorbisfile
  vorbis
  ogg
  z
  vita2d
  SceDisplay_stub
  SceCtrl_stub
  SceAudio_stub
  SceSysmodule_stub
  SceGxm_stub
  SceCommonDialog_stub
  SceTouch_stub
  SceHid_stub
  m
)
dolce_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
dolce_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys sce_sys
  FILE src/SuperStoner420 assets
  #FILE assets assets
  )