#-------------------------------------------------
#
# Project created by QtCreator 2016-01-21T09:54:22
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = refrf
TEMPLATE = app
#QWT_ROOT=/usr/local/qwt-6.1.6
include (config.pro)
include ( /usr/local/qwt-6.1.6/features/qwt.prf )
QT += widgets
VERSION = 3.0.0.0
#from ver 3.0.0.0 add in git
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
INCLUDEPATH += ../
MOC_DIR += ../moc
OBJECTS_DIR +=../obj

#IN
SOURCES +=  main.cpp\
            main_refrf.cpp httpProcess.cpp \
            data_plot.cpp


HEADERS  += main_refrf.h httpProcess.h\
            data_plot.h

#OUT
DESTDIR = ../$${RELEASEDIR}
TRANSLATIONS +=rfamp_ru.ts

