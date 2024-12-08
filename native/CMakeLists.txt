# The Flutter tooling requires that developers have CMake 3.10 or later
# installed. You should not increase this version, as doing so will cause
# the plugin to fail to compile for some customers of the plugin.
cmake_minimum_required(VERSION 3.10)

project(pro_miniaudio_library VERSION 0.0.1 LANGUAGES C)

set(SOURCES
  "src/context.c"
  "src/logger.c"
  "src/miniaudio.c"
  "src/playback_device.c"
  "src/waveform.c"
)

add_library(pro_miniaudio SHARED ${SOURCES})

set(PUBLIC_HEADERS
  "include/context.h"
  "include/logger.h"
  "include/playback_device.h"
  "include/waveform.h"
  "include/platform.h"
)

set_target_properties(pro_miniaudio PROPERTIES
  PUBLIC_HEADER "${PUBLIC_HEADERS}"
  OUTPUT_NAME "pro_miniaudio"
)

target_compile_definitions(pro_miniaudio PUBLIC DART_SHARED_LIB)
