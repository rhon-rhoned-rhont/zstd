cmake_minimum_required(VERSION 3.22.1)

project(expozstd)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(ZSTD_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zstd/lib)

include_directories(
  ${ZSTD_LIB_DIR}
  ${ZSTD_LIB_DIR}/common
)

file(GLOB ZSTD_COMMON_SRC
  ${ZSTD_LIB_DIR}/common/*.c
)

file(GLOB ZSTD_COMPRESS_SRC
  ${ZSTD_LIB_DIR}/compress/*.c
)

file(GLOB ZSTD_DECOMPRESS_SRC
  ${ZSTD_LIB_DIR}/decompress/*.c
)

file(GLOB ZSTD_DICT_SRC
  ${ZSTD_LIB_DIR}/dictBuilder/*.c
)

set(ZSTD_SOURCES
  ${ZSTD_COMMON_SRC}
  ${ZSTD_COMPRESS_SRC}
  ${ZSTD_DECOMPRESS_SRC}
  ${ZSTD_DICT_SRC}
)

add_library(
  expozstd
  SHARED
  expozstd_jni.cpp
  ${ZSTD_SOURCES}
)

find_library(log-lib log)

target_compile_definitions(expozstd PRIVATE
  ZSTD_MULTITHREAD=0
)

target_link_libraries(
  expozstd
  ${log-lib}
)
