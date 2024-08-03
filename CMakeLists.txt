cmake_minimum_required(VERSION 3.28)
project(dbms101 LANGUAGES C CXX)
set(PROJECT_NAME dbms101)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # optional, ensure standard is supported
set(CMAKE_CXX_EXTENSIONS OFF)  # optional, keep compiler extensions off
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

# Spdlog
set(SPDLOG_BUILD_EXAMPLES OFF)
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_BUILD_SHARED OFF)
set(SPDLOG_ENABLE_PCH ON)
set(SPDLOG_BUILD_EXAMPLE OFF)
set(SPDLOG_BUILD_TESTS OFF)
set(SPDLOG_BUILD_BENCH OFF)

set(JSON_BuildTests OFF CACHE INTERNAL "")

include_directories(/usr/local/include)
include_directories(third_party/StringZilla/include/stringzilla)
include_directories(third_party/mio/single_include/mio)
include_directories(third_party/fast_io/include)
include_directories(third_party/fast_io/include)
include_directories(third_party/xxHash)
include_directories(include)

add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/Catch2)
add_subdirectory(third_party/json)
add_subdirectory(third_party/EventBus/lib)

link_directories(/usr/local/lib)
add_executable(${PROJECT_NAME}
        main.cpp
        third_party/xxHash/xxhash.c
#        third_party/xxHash/xxh_x86dispatch.c
        src/IncludeSanity.cpp
        include/hash.hpp
)

target_link_libraries(${PROJECT_NAME}
        spdlog::spdlog
        Catch2::Catch2
        nlohmann_json::nlohmann_json
        Dexode::EventBus
)
