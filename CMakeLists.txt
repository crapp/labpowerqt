if (WIN32)
    cmake_minimum_required(VERSION 3.1.3) # this version knows of MSVC 14
else()
    cmake_minimum_required(VERSION 2.8.12) #2.8.12 provides an easy way to build with qt5
endif(WIN32)
project(labpowerqt)

#The version number
set (labpowerqt_VERSION_MAJOR 0)
set (labpowerqt_VERSION_MINOR 1)
set (labpowerqt_VERSION_PATCH 0)

option (EALOGGER_EXTERNAL "Download and build ealogger" ON)
option (SANITIZER "Use AddressSanitizer (ASan) compiler flags" OFF)

add_subdirectory(forms)
add_subdirectory(external/qcustomplot)
add_subdirectory(external/switchbutton)
add_subdirectory(src)
