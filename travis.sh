#!/bin/sh
#qtchooser -list-versions
#export QT_SELECT=qt5

set -evx

#env | sort

#qmake -v
#lrelease kOferta.pro
cmake -E make_directory build
cmake -E chdir build cmake -DKOFERTA_VERSION="${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}" -GNinja ..
cmake --build build

#build/kofertaSteps >/dev/null &
#cucumber features 
