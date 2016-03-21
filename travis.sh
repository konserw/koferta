#!/bin/sh
#qtchooser -list-versions
#export QT_SELECT=qt5

set -evx

#env | sort

case $OSTYPE in darwin*)
    brew update > /dev/null
    brew install qt5
    brew install ninja
    ;;
 esac
cmake --version

#qmake -v
#lrelease kOferta.pro
cmake -E make_directory build
cmake -E chdir build cmake .. # -GNinja ..
cmake --build build

build/kofertaSteps >/dev/null &
cucumber features 
