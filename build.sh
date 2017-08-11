#!/bin/bash

CODE_PATH=code
BUILD_PATH=build
ASSETS_PATH=data
INSTALL_PATH="/usr/local/cprj/"
TEMPLATE_PATH="templates"
INCLUDES_PATH="includes"
SCRIPTS_PATH="scripts"

OPTIMIZED_SWITCHES="-fno-builtin -O2 -ffast-math -ftrapping-math"
DEBUG_SWITCHES=""

IGNORE_WARNING_FLAGS="-Wno-unused-function -Wno-unused-variable -Wno-missing-braces -Wno-c++11-compat-deprecated-writable-strings"
OSX_DEPENDENCIES="-framework Cocoa -framework IOKit -framework CoreAudio -framework AudioToolbox"

rm -rf $BUILD_PATH/*

clang -g $DEBUG_SWITCHES -Wall $IGNORE_WARNING_FLAGS -lstdc++ -DINSTALL_PATH="\"$INSTALL_PATH\"" -DVARIATIONS_INTERNAL $CODE_PATH/cprj.cc -o $BUILD_PATH/_cprj

rm -rf $INSTALL_PATH
mkdir -p $INSTALL_PATH

cp $BUILD_PATH/_cprj $INSTALL_PATH
cp -a $TEMPLATE_PATH $INSTALL_PATH
cp -a $INCLUDES_PATH $INSTALL_PATH
cp $SCRIPTS_PATH/* $INSTALL_PATH

exit 0
