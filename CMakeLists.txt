option(EALOGGER_EXTERNAL "Download and build ealogger" ON)
option(SANITIZER "Use AddressSanitizer (ASan) compiler flags" OFF)

# 3.1.3 provides an easy way to build with qt5 and to set properties for targets
# 3.4 is needed to export symbols on windows (needed by ealogger)
if(EALOGGER_EXTERNAL)
    cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
else()
    cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)
endif()

project(labpowerqt VERSION 0.1.3)

add_subdirectory(forms)
add_subdirectory(external/qcustomplot)
add_subdirectory(external/switchbutton)
add_subdirectory(src)
add_subdirectory(resources)
