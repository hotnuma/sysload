TEMPLATE = app
TARGET = sysload
CONFIG = c99 link_pkgconfig
DEFINES = _GNU_SOURCE bool=BOOL true=TRUE false=FALSE
INCLUDEPATH =
PKGCONFIG =

#PKGCONFIG += tinyc
#PKGCONFIG += gtk+-3.0
#PKGCONFIG += glib-2.0

HEADERS = \
    cpuload.h \
    gettemp.h \
    gpuload.h \

SOURCES = \
    _0temp.c \
    cpuload.c \
    gettemp.c \
    gpuload.c \
    main.c \

DISTFILES = \
    License.txt \
    Readme.md \
    install.sh \
    meson.build \


