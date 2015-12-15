cmake_minimum_required(VERSION 2.8.12) #2.8.12 provides an easy way to build with qt5
project(labpowerqt)

#The version number
set (labpowerqt_VERSION_MAJOR 0)
set (labpowerqt_VERSION_MINOR 1)
set (labpowerqt_VERSION_PATCH 0)

option (SANITIZER "Use AddressSanitizer (ASan) compiler flags" OFF)
option (QT_NO_DEBUG_OUTPUT "Enable Qt debug output support" OFF)

add_subdirectory(forms)
add_subdirectory(external/qcustomplot)
add_subdirectory(external/switchbutton)
add_subdirectory(src)
