#-------------------------------------------------
#
# Project created by QtCreator 2016-01-21T09:54:22
#
#-------------------------------------------------
# git commit rules
# https://habr.com/ru/post/164297/
# https://habr.com/ru/company/mailru/blog/572560/
# https://ru.stackoverflow.com/questions/640787/%D0%A1%D1%82%D0%B8%D0%BB%D1%8C-%D0%BA%D0%BE%D0%BC%D0%BC%D0%B8%D1%82%D0%BE%D0%B2-%D0%B2-git
# https://docs.google.com/document/d/1QrDFcIiPjSLDn3EL15IJygNPiHORgU1_OOAqWjiDU5Y/edit#heading=h.uyo6cb12dt6w
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

