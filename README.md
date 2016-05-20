# LabPowerQt

[![Build Status](https://travis-ci.org/crapp/labpowerqt.svg?branch=master)](https://travis-ci.org/crapp/labpowerqt)

LabPowerQt is an application to control laboratory power supplies and to record
and visualize the data.

The software is written using the Qt Framework and therefor works on Linux,
Windows and OS X. Although it works cross platform the main targets for this
project are Linux and osx.

The application is in an early stage of development make sure to read the **known
issues** section.

## Setting up LabPowerQt

### Dependencies

The following dependencies are required to run this software:
* cmake >= 2.8.12 (On Windows you need at least version 3.3)
* One of these Compilers:
    * gcc >= 4.9
    * clang >= 3.4
    * MSVC >= 14 (Visual Studio 2015)
    * MinGW
* Qt >= 5.2 (Qt5Widgets, Qt5Gui, Qt5Core, Qt5SerialPort, Qt5Sql, Qt5PrintSupport, Qt5Quick)

### Installation

In order to install LabPowerQt you can obtain the source code from [github](https://github.com/crapp/labpowerqt) or use one of the precompiled packages.

#### Linux and OS X

Compiling LabPowerQt on Linux and OS X using unix make files.

```shell
# download the source code and change to the directory
# create a build directory
mkdir build
cd build
# run cmake to create makefiles. Use -DCMAKE_PREFIX_PATH if cmake doesn't find
# ypur installation of Qt5
cmake -DCMAKE_BUILD_TYPE=RELEASE ../
# now compile the source code and create the application. you can speed up
# compilation with make's j option.
make
# install the application
sudo make install
```

There are packages for the following Linux distributions:

* Archlinux (AUR)

#### Windows

You can compile LabPowerQt on Windows using cmake's [Visual Studio Generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators).

```shell
# create build directory inside labpowerqt source directory
mkdir build
cd build
# Assuming you are using Visual Studio 2015 on a 64bit windows installation and
# Qt 5.6 installed to C:\\Qt
# Please change these options so they suit your build evironment.
cmake -G"Visual Studio 12 2015 Win64" -DCMAKE_PREFIX_PATH="C:\\Qt\\5.6\\msvc2015_64" ../
```

Other possibilities are cmake's NMake Generator and mingw.

### Supported Hardware

Currently we only support Devices using the [Korad SCPI Protocol](http://sigrok.org/wiki/Velleman_PS3005D)

## Using the Application

The Application is easy to use and most thing should be self explanatory. Many
controls offer tooltips about what they do.

### Settings

The Settings can be accessed from the Main Application Window through the File Menu.

## Bugs, feature requests, ideas

Please use the [github bugtracker](https://github.com/crapp/labpowerqt/issues)
to submit bugs or feature requests


## Known issues

* The application does store the visibility for each graph (current, wattage ...)
  and this state is reapplied from the settings file whenever you start labpowerqt.
  Currently this is not working correctly with QCustomPlot and there is nothing I
  can do about it as I believe it is because of a bug in their code.


