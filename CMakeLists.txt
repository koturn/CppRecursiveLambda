cmake_minimum_required(VERSION 3.1)
project(CMakeProject CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "CMAKE_BUILD_TYPE (default): ${CMAKE_BUILD_TYPE}")
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


file(GLOB SRCS *.cpp *.cxx *.cc)
add_executable(
  fibonacci
  ${SRCS})

if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DNDEBUG)
endif()


if(MSVC)
  set(CMAKE_USE_RELATIVE_PATHS "ON")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  add_definitions(
    -DWIN32_LEAN_AND_MEAN
    -DNOMINMAX
    -D_USE_MATH_DEFINES
    -D_CRT_NONSTDC_NO_WARNINGS
    -D_CRT_SECURE_NO_WARNINGS)

  foreach(FLAG_VAR
      CMAKE_CXX_FLAGS
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/RTC[^ ]*" "" ${FLAG_VAR} "${${FLAG_VAR}}")
  endforeach(FLAG_VAR)

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Oi /Oy")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ob2 /Oi /Ot /Ox /Oy /GL")
  # set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Os")

  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
  foreach(WARNING_FLAG
      -Wall
      -Wextra
      -Wabi
      -Wcast-align
      -Wcast-qual
      -Wconversion
      -Wdisabled-optimization
      -Wfloat-equal
      -Wformat=2
      -Winit-self
      -Winvalid-pch
      -Wmissing-declarations
      -Wpointer-arith
      -Wredundant-decls
      -Wstack-protector
      -Wstrict-aliasing=2
      -Wstrict-overflow=5
      -Wswitch-enum
      -Wswitch-default
      -Wunknown-pragmas
      -Wunreachable-code
      -Wwrite-strings
      -pedantic)
    set(GNU_CLANG_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} ${WARNING_FLAG}")
  endforeach(WARNING_FLAG)

  option(ENABLE_ADDITIONAL_WARNING_FLAGS "Enable additional warning flags." OFF)
  if(ENABLE_ADDITIONAL_WARNING_FLAGS)
    set(GNU_CLANG_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Winline -Wsign-conversion")
  endif()

  set(GNU_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Wlogical-op")
  if(ENABLE_ADDITIONAL_WARNING_FLAGS)
    set(GNU_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Wunsafe-loop-optimizations")
  endif()

  set(CLANG_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Wsign-promo")

  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CXX_WARNING_FLAGS "${GNU_COMMON_WARNING_FLAGS}")
    foreach(WARNING_FLAG
        -Weffc++
        -Wctor-dtor-privacy
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wreorder
        -Wsign-promo
        -Wstrict-null-sentinel)
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
    endforeach(WARNING_FLAG)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.3 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.3)
      message("-- Add warning flags implemented in g++ 4.3")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wc++0x-compat")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.6 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.6)
      message("-- Add warning flags implemented in g++ 4.6")
      foreach(WARNING_FLAG
          -Wdouble-promotion
          -Wnoexcept
          -Wsuggest-attribute=const
          -Wsuggest-attribute=noreturn
          -Wsuggest-attribute=pure
          -Wtrampolines)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7)
      message("-- Add warning flags implemented in g++ 4.7")
      string(REGEX REPLACE "-Wc\\+\\+0x-compat" "-Wc++11-compat" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      foreach(WARNING_FLAG
          -Wdelete-non-virtual-dtor
          -Wvector-operation-performance
          -Wno-return-local-addr
          -Wzero-as-null-pointer-constant)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.8)
      message("-- Add warning flags implemented in g++ 4.8")
      foreach(WARNING_FLAG
          -Wsuggest-attribute=format
          -Wuseless-cast)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9)
      message("-- Add warning flags implemented in g++ 4.9")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wconditionally-supported")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5.1)
      message("-- Add warning flags implemented in g++ 5.1")
      foreach(WARNING_FLAG
          -Wc++14-compat
          -Wformat-signedness
          -Wsuggest-final-methods
          -Wsuggest-final-types
          -Wsuggest-override)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6.0)
      message("-- Add warning flags implemented in g++ 6.0")
      foreach(WARNING_FLAG
          -Wduplicated-cond
          -Wplacement-new=2
          -Wshift-overflow=2)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 7.0)
      message("-- Add warning flags implemented in g++ 7.0")
      string(REGEX REPLACE "-Wabi" "-Wabi=11" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      foreach(WARNING_FLAG
          -Wc++17-compat
          -Walloc-zero
          -Wduplicated-branches
          -Wformat-overflow=2
          -Wformat-truncation=2
          -Wregister
          -Wrestrict
          -Wshadow-local
          -Wstringop-overflow=4)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 8.0)
      message("-- Add warning flags implemented in g++ 8.0")
      string(REGEX REPLACE "-Wcast-align" "-Wcast-align=strict" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wsuggest-attribute=malloc")
    endif()
  else()
    set(CXX_WARNING_FLAGS "${CLANG_COMMON_WARNING_FLAGS}")
    foreach(WARNING_FLAG
        -Wc++11-compat
        -Weffc++
        -Wctor-dtor-privacy
        -Wdelete-non-virtual-dtor
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wreorder)
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
    endforeach(WARNING_FLAG)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.5 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.5)
      message("-- Add warning flags implemented in clang++ 3.5")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wc++14-compat")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.8 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.8)
      message("-- Add warning flags implemented in clang++ 3.8")
      foreach(WARNING_FLAG
          -Wc++1z-compat
          -Wdouble-promotion)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5.0)
      message("-- Add warning flags implemented in clang++ 5.0")
      string(REGEX REPLACE "-Wc\\+\\+1z-compat" "-Wc++17-compat" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wzero-as-null-pointer-constant")
    endif()
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_WARNING_FLAGS} ${CXX_WARNING_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O0 -ftrapv -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -mtune=native -march=native")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -g3 -Og")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -mtune=native -march=native")

  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
  set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} -s")

  if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_FORTIFY_SOURCE=2 -D_GLIBCXX_DEBUG)
  endif()
endif()

string(REGEX REPLACE "^ +" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")


message(STATUS "Variables")

foreach(VARNAME
    CMAKE_SYSTEM_NAME
    CMAKE_SOURCE_DIR
    CMAKE_BINARY_DIR
    CMAKE_BUILD_TYPE
    CMAKE_CONFIGURATION_TYPES
    CMAKE_COMPILER_IS_GNUCC
    CMAKE_COMPILER_IS_GNUCXX
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_EXE_LINKER_FLAGS
    CMAKE_EXE_LINKER_FLAGS_DEBUG
    CMAKE_EXE_LINKER_FLAGS_RELEASE
    CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
    CMAKE_EXE_LINKER_FLAGS_MINSIZEREL)
  message(STATUS "${VARNAME}: ${${VARNAME}}")
endforeach(VARNAME)
