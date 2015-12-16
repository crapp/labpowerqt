# LabPowerQt

LabPowerQt is an application to control laboratory power supplies and to record and visualize the data.
The software is written with the Qt Framework and therefor works on Linux, Windows and OS X.


## Setting up LabPowerQt

### Dependencies

The following dependencies are required to run this software:
* cmake >= 2.8.12 (On Windows you need at least version 3.3)
* One of these Compilers:
    * gcc >= 4.7
    * clang >= 3.4
    * MSVC >= 14 (Visual Studio 2015)
    * MinGW 
* Qt >= 5.2 (Qt5Widgets, Qt5Gui, Qt5Core, Qt5SerialPort, Qt5Sql, Qt5PrintSupport, Qt5Quick)

### Installation

In order to install LabPowerQt you can obtain the source code from [github](https://github.com/crapp/labpowerqt) or use one of the precompiled packages.

#### Linux and OS X

Compiling LabPowerQt on Linux and OS X  

```shell
# download the source code and change to the directory
# create a build directory
mkdir build
cd build
# run cmake to create makefiles
cmake -DCMAKE_BUILD_TYPE=RELEASE ../
# now compile the source code and create the application. you can speed up 
# compilation with the j option.
make 
# install the application
sudo make install
```

There are packages for the following Linux distributions:

* Archlinux (AUR)

#### Windows

Compiling LabPowerQt on Windows is currently a bit challenging as there are no precompiled Qt libraries available for Visual Studio 2015.


Currently we only support Devices using the [Korad Protocol](http://sigrok.org/wiki/Velleman_PS3005D)

