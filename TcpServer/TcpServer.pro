QT       += core gui widgets network sql

TARGET = TcpServer
TEMPLATE = app


SOURCES += main.cpp\
        tcpserver.cpp

HEADERS  += tcpserver.h \
    defs.h

FORMS    += tcpserver.ui
