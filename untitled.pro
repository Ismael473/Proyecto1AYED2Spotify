QT += core gui multimedia multimediawidgets widgets

# Archivos del proyecto
SOURCES += \
    Aplicacion.cpp \
    main.cpp

HEADERS += \
    Aplicacion.h \
    main.h

FORMS += \
    main.ui

CONFIG += c++17

# Configuración para TagLib con vcpkg
VCPKG_PATH = C:/vcpkg-master/installed/x64-windows
INCLUDEPATH += $$VCPKG_PATH/include
LIBS += -L$$VCPKG_PATH/lib -ltag

# Copia automática de tag.dll al directorio de salida
VCPKG_BIN = C:/vcpkg-master/installed/x64-windows/bin

win32:CONFIG(release, debug|release): {
    QMAKE_POST_LINK += copy /y $$quote($$VCPKG_BIN/tag.dll) $$quote($$OUT_PWD/release/)
}
win32:CONFIG(debug, debug|release): {
    QMAKE_POST_LINK += copy /y $$quote($$VCPKG_BIN/tagd.dll) $$quote($$OUT_PWD/debug/)
}

# Reglas predeterminadas para el despliegue
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
