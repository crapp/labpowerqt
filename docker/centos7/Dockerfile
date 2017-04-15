FROM centos:7

ARG workdir="/workdir"
ARG cmake_ver="cmake-3.8.0-Linux-x86_64"
ARG make_core=4
ENV PATH="${workdir}/${cmake_ver}/bin:${PATH}"
ENV PATH="/opt/rh/devtoolset-6/root/usr/bin:${PATH}"
ENV PATH="/usr/lib64/qt5/bin/:${PATH}"

RUN mkdir -pv $workdir
RUN yum -y update && yum -y install wget epel-release centos-release-scl
RUN yum -y install \
        devtoolset-6-gcc devtoolset-6-gcc-c++ make file \
        qt5-qtbase-devel qt5-qtbase-gui qt5-qtserialport-devel qt5-qtquickcontrols2-devel \
        fuse fuse-libs \
        git
RUN cd $workdir && \
        wget -nv "https://cmake.org/files/v3.8/${cmake_ver}.tar.gz" && \
        tar -xf "${cmake_ver}.tar.gz"
RUN cd $workdir && \
        git clone "https://github.com/crapp/labpowerqt.git" && \
        cd labpowerqt && \
        mkdir -pv build && cd build && \
        cmake -DCMAKE_BUILD_TYPE=Release ../ && \
        make -j $make_core

RUN cd $workdir && \
        wget -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" && \
        cd labpowerqt/build && \
        find \( -name "moc_*" -or -name "*.o" -or -name "qrc_*" -or -name "Makefile*" -or -name "*.a" \) -exec rm -v {} \; && \
        mv -v ../../linuxdeployqt-continuous-x86_64.AppImage ./linuxdeployqt && chmod a+x linuxdeployqt
        #./linuxdeployqt -verbose=3 -appimage src/labpowerqt

CMD g++ --version
