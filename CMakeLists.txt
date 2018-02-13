# Copyright (c) 2013-2018 Sébastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT)

cmake_minimum_required(VERSION 2.8.12) # first version with add_compile_options()
project(CppSkeleton CXX)

# Verbose Makefile
#set (CMAKE_VERBOSE_MAKEFILE ON)

# Print some standard CMake variables
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "CMAKE_SYSTEM_NAME/_VERSION      '${CMAKE_SYSTEM_NAME}' '${CMAKE_SYSTEM_VERSION}'")
message(STATUS "CMAKE_CXX_COMPILER_ID/_VERSION  '${CMAKE_CXX_COMPILER_ID}' '${CMAKE_CXX_COMPILER_VERSION}'")
if (NOT MSVC)
    message(STATUS "CMAKE_BUILD_TYPE                '${CMAKE_BUILD_TYPE}'")
endif (NOT MSVC)

# Define useful variables to handle OS/Compiler differences
if (MSVC)
    # Flags for linking with multithread static C++ runtime
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CPPLINT_ARG_OUTPUT      "--output=vs7")
    set(CPPCHECK_ARG_TEMPLATE   "--template=vs")
    set(DEV_NULL                "NUL")
    set(SYSTEM_LIBRARIES        "")
    add_definitions(/D_CRT_SECURE_NO_WARNINGS)
else (MSVC)
    set(CPPLINT_ARG_OUTPUT      "--output=eclipse")
    set(CPPCHECK_ARG_TEMPLATE   "--template=gcc")
    set(DEV_NULL                "/dev/null")
    set(SYSTEM_LIBRARIES        "rt")

    # C++11
    add_compile_options(-std=c++0x) # equivalent to "-std=c++11" but backward compatible for GCC 4.6 on Travic-CI
    # Stack protection
    add_compile_options(-fstack-protector-all)

    if (CMAKE_COMPILER_IS_GNUCXX)
        # For stacktraces:
        add_compile_options(-rdynamic)
    endif (CMAKE_COMPILER_IS_GNUCXX)
endif (MSVC)


# Link to googletest unit-test suite.
option(CPPSKELETON_RUN_TESTS "Run googletest unit-test suite." ON)
if (CPPSKELETON_RUN_TESTS)
    # add the subdirectory containing the CMakeLists.txt for the googletest library
    add_subdirectory(googletest)
    include_directories("${PROJECT_SOURCE_DIR}/googletest/googletest/include")
endif (CPPSKELETON_RUN_TESTS)


# Enable maximum Warning level for our own code (after the googletest lib)
if (MSVC)
    # Set warning level to maximum (instead of default /W3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else (MSVC)
    if (CMAKE_COMPILER_IS_GNUCXX)
        # Enable maximum of Warnings for GCC:
        add_compile_options(-Wall -Wextra -Wswitch-default -Wswitch-enum -Winit-self -Wformat-security -Wfloat-equal -Wcast-qual -Wconversion -Wlogical-op -Winline)
        if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL "4.9" OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.9")
            add_compile_options (-Wfloat-conversion)
            add_compile_options (-Wshadow)
        endif ()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # Enable maximum of Warnings for Clang:
        add_compile_options(-Weverything)
        add_compile_options(-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-covered-switch-default -Wno-unreachable-code)
    endif (CMAKE_COMPILER_IS_GNUCXX)
endif (MSVC)


# List all sources/headers files
set(source_files
 ${CMAKE_SOURCE_DIR}/src/Main.cpp
 ${CMAKE_SOURCE_DIR}/src/Utils/DateTime.cpp src/Utils/DateTime.h
 ${CMAKE_SOURCE_DIR}/src/Utils/Measure.cpp src/Utils/Measure.h
 ${CMAKE_SOURCE_DIR}/src/Utils/Random.cpp src/Utils/Random.h
 ${CMAKE_SOURCE_DIR}/src/Utils/Time.cpp src/Utils/Time.h
 ${CMAKE_SOURCE_DIR}/src/Utils/Utils.h
)
source_group(src     FILES ${source_files})

# List script files
set(script_files
 ${CMAKE_SOURCE_DIR}/.travis.yml
 ${CMAKE_SOURCE_DIR}/appveyor.yml
 ${CMAKE_SOURCE_DIR}/build.bat
 ${CMAKE_SOURCE_DIR}/build.sh
)
source_group(scripts FILES ${script_files})

# List doc files
set(doc_files
 ${CMAKE_SOURCE_DIR}/README.md
 ${CMAKE_SOURCE_DIR}/LICENSE.txt
)
source_group(doc     FILES ${doc_files})

# All includes are relative to the "src" directory
include_directories("${PROJECT_SOURCE_DIR}/src")

# add the application executable
add_executable(CppSkeleton ${source_files} ${doc_files} ${script_files})
target_link_libraries(CppSkeleton ${SYSTEM_LIBRARIES})


# Optional additional targets:

# Run cpplint C++ StyleGuide linter tool.
option(CPPSKELETON_RUN_CPPLINT "Run cpplint.py tool for Google C++ StyleGuide." ON)
if (CPPSKELETON_RUN_CPPLINT)
    find_package(PythonInterp 2.6)
    if (PYTHONINTERP_FOUND)
        set(CPPLINT_ARG_LINELENGTH "--linelength=120")
        set(CPPLINT_ARG_VERBOSE    "--verbose=1")
        if(EXISTS "${PROJECT_SOURCE_DIR}/cpplint/cpplint.py")
            # add a cpplint target to the "all" target
            add_custom_target(cpplint
                ALL
                COMMAND ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/cpplint/cpplint.py ${CPPLINT_ARG_OUTPUT} ${CPPLINT_ARG_LINELENGTH} ${CPPLINT_ARG_VERBOSE} ${source_files}
            )
            message(STATUS "CPPSKELETON_RUN_CPPLINT        ON")
        else ()
            message(STATUS "cpplint submodule missing")
        endif ()
    else ()
        message(STATUS "Python not found, not runing cpplint")
    endif (PYTHONINTERP_FOUND)
else (CPPSKELETON_RUN_CPPLINT)
    message(STATUS "CPPSKELETON_RUN_CPPLINT        OFF")
endif (CPPSKELETON_RUN_CPPLINT)

# Run cppcheck C++ static analysis tool.
option(CPPSKELETON_RUN_CPPCHECK "Run cppcheck C++ static analysis tool." ON)
if (CPPSKELETON_RUN_CPPCHECK)
    find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
    if (CPPCHECK_EXECUTABLE)
        # add a cppcheck target to the "all" target
        add_custom_target(cppcheck
            ALL
            COMMAND ${CPPCHECK_EXECUTABLE} -j 8 --enable=style --quiet ${CPPCHECK_ARG_TEMPLATE} ${PROJECT_SOURCE_DIR}/src
        )
        execute_process(COMMAND "${CPPCHECK_EXECUTABLE}" --version OUTPUT_VARIABLE CPPCHECK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
        message(STATUS "Found Cppcheck:                ${CPPCHECK_EXECUTABLE} ${CPPCHECK_VERSION}")
    else (CPPCHECK_EXECUTABLE)
        message(STATUS "Could NOT find cppcheck")
    endif (CPPCHECK_EXECUTABLE)
else (CPPSKELETON_RUN_CPPCHECK)
    message(STATUS "CPPSKELETON_RUN_CPPCHECK       OFF")
endif (CPPSKELETON_RUN_CPPCHECK)

# Run Doxygen C++ documentation tool.
option(CPPSKELETON_RUN_DOXYGEN "Run Doxygen C++ documentation tool." ON)
if (CPPSKELETON_RUN_DOXYGEN)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        # add a Doxygen target to the "all" target
        add_custom_target(doxygen
            ALL
            COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile > ${DEV_NULL}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
    endif (DOXYGEN_FOUND)
else (CPPSKELETON_RUN_DOXYGEN)
    message(STATUS "CPPSKELETON_RUN_DOXYGEN        OFF")
endif (CPPSKELETON_RUN_DOXYGEN)

# Run CLOC line counting tool.
option(CPPSKELETON_RUN_CLOC "Run CLOC line counting tool." ON)
if (CPPSKELETON_RUN_CLOC)
    find_program(CLOC_EXECUTABLE cloc DOC "CLOC tool for counting lines of code.")
    if (CLOC_EXECUTABLE)
        # add a cloc target to the "all" target
        add_custom_target(cloc
            ALL
            COMMAND "${CLOC_EXECUTABLE}" ${source_files} ${script_files} --quiet
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )
    else (CLOC_EXECUTABLE)
        message(STATUS "Could NOT find cloc")
    endif (CLOC_EXECUTABLE)
else (CPPSKELETON_RUN_CLOC)
    message(STATUS "RUN_CLOC 		           OFF")
endif (CPPSKELETON_RUN_CLOC)

# Run googletest unit-test suite.
if (CPPSKELETON_RUN_TESTS)
    # add the test executable
    add_executable(TestSkeleton
        src/Utils/Time.cpp src/Utils/Time.h src/Utils/Time_test.cpp)
    target_link_libraries(TestSkeleton gtest_main ${SYSTEM_LIBRARIES})

    # add a "test" target:
    enable_testing()

    # does the application runs
    add_test(ApplicationRuns CppSkeleton)

    # does the tests pass
    add_test(TestsPass TestSkeleton)
else (CPPSKELETON_RUN_TESTS)
    message(STATUS "CPPSKELETON_RUN_TESTS          OFF")
endif (CPPSKELETON_RUN_TESTS)

