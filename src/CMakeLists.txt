if(${SANITIZER})
    message(WARNING "Using address sanitizer compiler option! This will slow \
    down the application and increase memory consumption")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

add_definitions(-DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT)

if(EALOGGER_EXTERNAL)
    message(STATUS "Trying to get ealogger sources from github and compile the \
    library")
    include(ExternalProject)

    # add ealogger as external project. this will pull the sources from github,
    # compile them and install the library to a subfolder. as we are using a
    # static library we can link it directly
    ExternalProject_Add(
        ealogger_external
        URL https://github.com/crapp/ealogger/archive/v0.8.1.tar.gz
        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/ealogger"
        INSTALL_DIR install
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
                   -DCMAKE_BUILD_TYPE=Release
                   -DBUILD_SHARED_LIBS=OFF
                   -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                   -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        )
    # get the install dir
    ExternalProject_Get_Property(ealogger_external install_dir)
    set(EALOGGER_LIB
        "${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ealogger${CMAKE_STATIC_LIBRARY_SUFFIX}"
        )
    include_directories(
        "${install_dir}/include"
        )
    set(CMAKE_THREAD_PREFER_PTHREAD ON)
    find_package(Threads REQUIRED)
else()
    message(STATUS "Searching for ealogger in default locations")
    find_library(EALOGGER_LIB
        NAMES ealogger)
    if(EALOGGER_LIB)
        message(STATUS "Found ealogger library")
    else()
        message(FATAL_ERROR "ealogger library not found")
    endif()
    # search for ealogger
endif()

# Add our custom cmake search modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# discover ui files automagically
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/forms")

# Find the QtWidgets library. This has dependencies on QtGui and QtCore!
find_package(Qt5Widgets 5.4 REQUIRED)
message(STATUS "Found Qt version ${Qt5Widgets_VERSION_STRING}")
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Sql REQUIRED)

# # generate ui_*.h files
# qt5_wrap_ui(${CMAKE_PROJECT_NAME}_FORMS ${UI_FILES})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} # config.h
    ${CMAKE_SOURCE_DIR}/external/qcustomplot
    ${CMAKE_SOURCE_DIR}/external/switchbutton
    ${CMAKE_SOURCE_DIR}/external/qaccordion/include
)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
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
    ${CMAKE_CURRENT_SOURCE_DIR}/log_instance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plottingarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/powersupplyscpi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/powersupplystatus.h
    ${CMAKE_CURRENT_SOURCE_DIR}/recordarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/recordsqlmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serialcommand.h
    ${CMAKE_CURRENT_SOURCE_DIR}/serialqueue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settingsdefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/settingsdefault.h
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
# define our c++ standard and make sure cmake fails if the requirement is not
# met.
set_property(TARGET labpowerqt PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET labpowerqt PROPERTY CXX_STANDARD 11)

target_link_libraries(labpowerqt
Qt5::Widgets
Qt5::SerialPort
Qt5::Quick
Qt5::Qml
Qt5::PrintSupport
Qt5::Sql
${EALOGGER_LIB})

# make sure dependencies are build before our main target
if(EALOGGER_EXTERNAL)
    add_dependencies(labpowerqt ealogger_external)
    target_link_libraries(labpowerqt Threads::Threads)
endif()

if(WIN32)
    # have to add this explcicitly to avoid linker errors on msvc
    add_definitions(-D_WINSOCKAPI_)
    # make this a windows gui application without opening a console
    target_link_libraries(labpowerqt Qt5::WinMain)
    set_property(TARGET labpowerqt PROPERTY WIN32_EXECUTABLE true)
endif()

install(TARGETS labpowerqt DESTINATION bin)
