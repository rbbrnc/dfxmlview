TARGET   = dfxmlview
TEMPLATE = app

QT += xml
CONFIG += silent

#-- Places for generated files
#DESTDIR = .
OBJECTS_DIR = out/objs
MOC_DIR     = out/mocs
UI_DIR      = out/ui
RCC_DIR     = out/resources

#INCLUDEPATH += $$PWD/QMagic
#LIBS += $$PWD/QMagic/libQMagic.a -lmagic

HEADERS += \
	dfxml.h \
	parser.h \
	byterun.h \
	fileobject.h \
	source.h \
	volume.h \
	model.h \
	mainwindow.h \
	sourceinfodialog.h \
	fileobject_info_dialog.h

SOURCES += \
	main.cpp \
	dfxml.cpp \
	parser.cpp \
	byterun.cpp \
	fileobject.cpp \
	source.cpp \
	volume.cpp \
	model.cpp \
	mainwindow.cpp \
	sourceinfodialog.cpp \
	fileobject_info_dialog.cpp

FORMS += mainwindow.ui \
	 sourceinfodialog.ui \
	 fileobject_info_dialog.ui

RESOURCES += \
	icons.qrc


#-- Common gcc flags
QMAKE_CXXFLAGS += -Wall -W -Wunreachable-code
QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections
QMAKE_LFLAGS   += -Wl,--gc-sections,--as-needed

#-- Debug Symbols
#QMAKE_CXXFLAGS += -g -ggdb
#QMAKE_LFLAGS   += -Wl,--print-gc-sections
DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

