cmake_minimum_required(VERSION 2.8.11) #2.8.11 provides an easy way to build with qt5
project(labpowerqt)

#The version number
set (labpowerqt_VERSION_MAJOR 0)
set (labpowerqt_VERSION_MINOR 1)
set (labpowerqt_VERSION_PATCH 0)

add_subdirectory(forms)
add_subdirectory(external/qcustomplot)
add_subdirectory(src)
