#-------------------------------------------------
#
# Project created by QtCreator 2011-10-28T09:54:36
#
#-------------------------------------------------
RELEASEDIR=release
CONFIG += c++11
CONFIG += release
CONFIG -=declarative_debug
#CONFIG += debug

CONFIG(debug, debug|release) {CURRENT_BUILD = Run_project_debug}
CONFIG(release, debug|release) {CURRENT_BUILD = Run_project_release}

CONFIG(debug, debug|release) {SUFFIX =  d}
CONFIG(release, debug|release) {SUFFIX =}

#CONFIG(release, debug|release) { DEFINES+=QT_NO_DEBUG_OUTPUT }

