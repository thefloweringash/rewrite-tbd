cmake_minimum_required(VERSION 3.18)
project(rewrite-tbd)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "-g")
ENDIF()

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBYAML REQUIRED IMPORTED_TARGET yaml-0.1)

add_executable(rewrite-tbd main.cpp wrappers.h tbdv2.cpp tbdv4.cpp yaml.cpp rewrite.h tbdv2.h tbdv4.h yaml.h rewrite.cpp)
target_link_libraries(rewrite-tbd
        PRIVATE
        PkgConfig::LIBYAML
        )

install(TARGETS rewrite-tbd)
