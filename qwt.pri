#LIBS += -LC:/program1/QtSDK/Qwt-5.2.2/lib -lqwt5
#for Win32 only
#QWT version
VER_MAJ      = 5
QWT_ROOT = C:/QtSDK/Qwt-5.2.2
//QWT_ROOT = c:/program1/QtSDK/Qwt-5.2.2
CONFIG           += QwtDll
#CONFIG       += QwtPlot
#CONFIG     += QwtWidgets

DEBUG_SUFFIX      = d
RELEASE_SUFFIX =


INCLUDEPATH += $${QWT_ROOT}/include

SUFFIX_STR =

CONFIG(debug, debug|release) {
  SUFFIX_STR = $${DEBUG_SUFFIX}
}
else {
  SUFFIX_STR = $${RELEASE_SUFFIX}
}

QWTLIB   = qwt$${SUFFIX_STR}

contains(CONFIG, QwtDll) {
  DEFINES += QT_DLL QWT_DLL
  QWTLIB   = $${QWTLIB}$${VER_MAJ}
}

LIBS += -L$${QWT_ROOT}/lib -l$${QWTLIB}
