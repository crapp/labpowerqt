# Initialize CXXFLAGS for Linux, OS X and MinGW on Windows
if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
    set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
    # No reorder warnings because these pop up frequently if you are using the Q_OBJECT macro
    set(CMAKE_CXX_FLAGS_DEBUG         "${CMAKE_CXX_FLAGS_DEBUG} -Wextra -Wno-reorder")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

# Compiler-specific C++11 activation.
if (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" AND ${MSVC_VERSION} LESS 1900)
    message(
        # MSVC 1900 has the best C++11 support so far. So, fatal_error if you try to
        # use anything else.
      FATAL_ERROR
      "LabPowerQt does not support compiling on Windows with "
      "${CMAKE_CXX_COMPILER_ID}. Please use MSVC.")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

if (${SANITIZER})
    message(WARNING "Using address sanitizer compiler option!")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()


# Add our custom cmake search modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library. This has dependencies on QtGui and QtCore!
find_package(Qt5Widgets 5.2 REQUIRED)
message(STATUS "Found Qt version ${Qt5Widgets_VERSION_STRING}")
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Sql REQUIRED)

#find_package(Qwt 6.1 REQUIRED)

# generate ui_*.h files
qt5_wrap_ui(${CMAKE_PROJECT_NAME}_FORMS ${UI_FILES})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} # config.h
    ${CMAKE_SOURCE_DIR}/external/qcustomplot
    ${CMAKE_SOURCE_DIR}/external/switchbutton
    ${CMAKE_SOURCE_DIR}/external/qaccordion/include
)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

set(HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/aboutme.h
    ${CMAKE_CURRENT_SOURCE_DIR}/clickablelabel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/databasedef.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dbconnector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardconnection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardfinal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardintro.h
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardoptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/displayarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/koradscpi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/floatingvaluesdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/labpowercontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/labpowermodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plottingarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/powersupplyscpi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/powersupplystatus.h
    ${CMAKE_CURRENT_SOURCE_DIR}/recordarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/recordsqlmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serialcommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serialqueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settingsdefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settingsdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tabcontrol.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tabhistory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tabprogram.h
    ${CMAKE_CURRENT_SOURCE_DIR}/valuedoublespinbox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/yaxishelper.h
)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/aboutme.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/clickablelabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dbconnector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardconnection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardfinal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardintro.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/devicewizardoptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/displayarea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/floatingvaluesdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/koradscpi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/labpowercontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/labpowermodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plottingarea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/powersupplyscpi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/recordarea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/recordsqlmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/serialqueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/settingsdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tabcontrol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tabhistory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tabprogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/valuedoublespinbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/yaxishelper.cpp
)

set(ICON_RESOURCE ${PROJECT_SOURCE_DIR}/icons/icons.qrc)
# add resource files so they can be compiled into the binary
qt5_add_resources(ICON_RESOURCE_ADDED ${ICON_RESOURCE})

add_executable(labpowerqt
    ${HEADER}
    ${SOURCE}
    ${${CMAKE_PROJECT_NAME}_FORMS}
    ${CUSTOMPLOT_HEADER}
    ${CUSTOMPLOT_SOURCE}
    ${SWITCHBUTTON_HEADER}
    ${SWITCHBUTTON_SOURCE}
    ${ICON_RESOURCE_ADDED}
)
# define our c++ standard and make sure cmake fails if the requirement is not met.
#set_property(TARGET labpowerqt PROPERTY CXX_STANDARD_REQUIRED ON)
#set_property(TARGET labpowerqt PROPERTY CXX_STANDARD 11)

add_definitions(-DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT)

target_link_libraries(labpowerqt
Qt5::Widgets
Qt5::SerialPort
Qt5::Quick
Qt5::Qml
Qt5::PrintSupport
Qt5::Sql)
