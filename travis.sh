#!/bin/sh
#qtchooser -list-versions
#export QT_SELECT=qt5

set -evx

#env | sort

#qmake -v
#lrelease kOferta.pro
cmake -E make_directory build
cmake -E chdir build cmake -DKOFERTA_VERSION="${TRAVIS_BRANCH}-${TRAVIS_BUILD_NUMBER}" -GNinja -DKOFERTA_SQL_USER="${KOFERTA_SQL_USER}" -DKOFERTA_SQL_PWD="${KOFERTA_SQL_PWD}" ..
cmake --build build

#build/kofertaSteps >/dev/null &
#cucumber features 
