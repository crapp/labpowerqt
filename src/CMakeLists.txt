# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "-Wextra -O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")


# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

# Add our custom cmake search modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find Threads
#find_package(Threads REQUIRED)
# Find the QtWidgets library. This has dependencies on QtGui and QtCore!
find_package(Qt5Widgets 5.1 REQUIRED)
message(STATUS "Found Qt version ${Qt5Widgets_VERSION_STRING}")

find_package(Qwt REQUIRED)

# generate ui_*.h files
qt5_wrap_ui(${CMAKE_PROJECT_NAME}_FORMS ${UI_FILES})

# include this directories so all moc headers and wrapped ui files are found
include_directories(
    ${PROJECT_BINARY_DIR}
    ${QWT_INCLUDE_DIR}
)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

set(HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
)

add_executable(labpowerqt ${HEADER} ${SOURCE} ${${CMAKE_PROJECT_NAME}_FORMS})
target_link_libraries(labpowerqt Qt5::Widgets ${QWT_LIBRARY})

