FROM ubuntu:trusty

ARG wdir="/workdir"
WORKDIR $wdir

ARG cmake_ver="cmake-3.8.0-Linux-x86_64"
ARG make_core=4
ENV PATH="${wdir}/${cmake_ver}/bin:${PATH}"
ENV PATH="/opt/qt58/bin:${PATH}"

RUN apt-get update && apt-get install -y software-properties-common
#RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y && \
RUN add-apt-repository ppa:beineri/opt-qt58-trusty -y
RUN apt-get update && apt-get install -y mesa-common-dev clang-3.5 wget curl make \
        qt58base qt58serialport qt58quickcontrols2 \
        git libfuse2
RUN wget -nv "https://cmake.org/files/v3.8/${cmake_ver}.tar.gz" && \
        tar -xf "${cmake_ver}.tar.gz"

# use git_rev to make sure the last to layers are run again if there are new
# commits
ARG git_rev="HEAD"
ARG git_branch="master"

RUN git clone -b $git_branch "https://github.com/crapp/labpowerqt.git" && \
        cd labpowerqt && \
        git checkout $git_rev && \
        mkdir -pv build && cd build && \
        cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_C_COMPILER=clang-3.5 \
          -DCMAKE_CXX_COMPILER=clang++-3.5 \
          -DCMAKE_PREFIX_PATH=/opt/qt58 \
          -DCMAKE_INSTALL_PREFIX=/usr ../ && \
        make -j $make_core && \
        make DESTDIR=appdir install

RUN wget -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" && \
        cd labpowerqt/build && \
        #find \( -name "moc_*" -or -name "*.o" -or -name "qrc_*" -or -name "Makefile*" -or -name "*.a" \) -exec rm -v {} \; && \
        mv -v ../../linuxdeployqt-continuous-x86_64.AppImage ./linuxdeployqt && \
        chmod a+x linuxdeployqt

CMD /bin/bash

####
# HOW TO USE
# run docker container interactively and allow access to fuse device
# docker run -it --cap-add SYS_ADMIN --device /dev/fuse trustylab
# enter labpowerqt build directory and run linuxdeployqt
#
# source /opt/qt5*/bin/qt5*-env.sh
# ./linuxdeployqt appdir/usr/share/applications/*.desktop -bundle-non-qt-libs && \
# ./linuxdeployqt appdir/usr/share/applications/*.desktop -appimage
#
# use docker cp to copy the generated appimage. the appimage has the same
# architecture than the host system.
#
