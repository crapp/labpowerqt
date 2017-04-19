if (WIN32)
    cmake_minimum_required(VERSION 3.3 FATAL_ERROR) # this version knows of MSVC 14
else()
    cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR) # 3.1 provides an easy way to build with qt5 and to set properties for targets
endif(WIN32)

project(labpowerqt VERSION 0.1.3)

option (EALOGGER_EXTERNAL "Download and build ealogger" ON)
option (SANITIZER "Use AddressSanitizer (ASan) compiler flags" OFF)

add_subdirectory(forms)
add_subdirectory(external/qcustomplot)
add_subdirectory(external/switchbutton)
add_subdirectory(src)
