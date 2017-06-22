#-------------------------------------------------
#
# Project created by QtCreator 2016-10-08T10:16:38
#
#-------------------------------------------------

QT       += core gui opengl\
            sensors\
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KAMEditor
TEMPLATE = app

CONFIG += c++11

win32 {
  LIBS += -lglu32 -lopengl32
}
macx {
  LIBS += -framework OpenGl
}

SOURCES += main.cpp\
        gui/mainwindow.cpp \
    gui/points/addpointdialog.cpp \
    gui/points/mousetoselectionpointdialog.cpp \
    gui/points/editpointdialog.cpp \
#    gui/commands/linedialog.cpp \
#    gui/commands/arcdialog.cpp \
#    gui/commands/labeldialog.cpp \
#    gui/commands/ttlinedialog.cpp \
#    gui/commands/arc2dialog.cpp \
#    gui/commands/cycledialog.cpp \
#    gui/commands/offdialog.cpp \
#    gui/commands/ondialog.cpp \
#    gui/commands/rotatedialog.cpp \
#    gui/commands/gotodialog.cpp \
#    gui/commands/procdialog.cpp \
#    gui/commands/callprocdialog.cpp \
#    gui/commands/commanddialog.cpp \
#    gui/commands/scaledialog.cpp \
#    gui/commands/velocitydialog.cpp \
#    gui/commands/pausedialog.cpp \
#    gui/commands/stopdialog.cpp \
#    gui/commands/commentdialog.cpp \
#    gui/commands/ttzarcdialog.cpp \
#    gui/commands/tttarcdialog.cpp \
#    gui/commands/splinedialog.cpp \
#    gui/commands/ttttsplinedialog.cpp \
    gui/commands/adddevicedialog.cpp \
    gui/gcodessyntaxhighlighter.cpp \
    gui/oglwidget.cpp \
    library/machinetool/machinetool.cpp \
    library/machinetool/settingsManager/settingsmanager.cpp \
    library/machinetool/components/axises/axis.cpp \
    library/machinetool/components/sensors/sensor.cpp \
    library/machinetool/components/devices/device.cpp \
    library/machinetool/components/devices/spindel.cpp \
    library/machinetool/components/movementController/movementcontroller.cpp \
    library/machinetool/components/pointsManager/pointsManager/pointsmanager.cpp \
    library/machinetool/components/commandInterpreter/commandinterpreter.cpp \
    library/machinetool/components/commandManager/commandsmanager.cpp \
    library/machinetool/components/commandManager/commands/arc.cpp \
    library/machinetool/components/commandManager/commands/line.cpp \
    library/machinetool/components/movementController/dimensionsController/dimensionscontroller.cpp \
    library/mcuLinksProvider/controllerConnector/controllerconnector.cpp \
    library/mcuLinksProvider/binaryMaskHandler/binaryMaskHandler.cpp

HEADERS  += gui/mainwindow.h \
    gui/points/addpointdialog.h \
    gui/points/mousetoselectionpointdialog.h \
    gui/points/editpointdialog.h \
#    gui/commands/linedialog.h \
#    gui/commands/arcdialog.h \
#    gui/commands/labeldialog.h \
#    gui/commands/ttlinedialog.h \
#    gui/commands/arc2dialog.h \
#    gui/commands/cycledialog.h \
#    gui/commands/offdialog.h \
#    gui/commands/ondialog.h \
#    gui/commands/rotatedialog.h \
#    gui/commands/gotodialog.h \
#    gui/commands/procdialog.h \
#    gui/commands/callprocdialog.h \
#    gui/commands/commanddialog.h \
#    gui/commands/scaledialog.h \
#    gui/commands/velocitydialog.h \
#    gui/commands/pausedialog.h \
#    gui/commands/stopdialog.h \
#    gui/commands/commentdialog.h \
#    gui/commands/ttzarcdialog.h \
#    gui/commands/tttarcdialog.h \
#    gui/commands/splinedialog.h \
#    gui/commands/ttttsplinedialog.h \
    gui/commands/adddevicedialog.h \
    gui/gcodessyntaxhighlighter.h \
    gui/oglwidget.h \
    library/machinetool/structs.h \
    library/machinetool/machinetool.h \
    library/machinetool/settingsManager/settingsmanager.h \
    library/machinetool/components/axises/axis.h \
    library/machinetool/components/sensors/sensor.h \
    library/machinetool/components/devices/device.h \
    library/machinetool/components/devices/spindel.h \
    library/machinetool/components/movementController/movementcontroller.h \
    library/machinetool/components/pointsManager/pointsManager/pointsmanager.h \
    library/machinetool/components/pointsManager/point/point.h \
    library/machinetool/components/commandManager/commandsmanager.h \
    library/machinetool/components/commandManager/commands/command.h \
    library/machinetool/components/commandManager/commands/commands.h \
    library/machinetool/components/commandManager/commands/arc.h \
    library/machinetool/components/commandManager/commands/line.h \
    library/machinetool/components/commandInterpreter/commandinterpreter.h \
    library/machinetool/components/movementController/dimensionsController/dimensionscontroller.h \
    library/mcuLinksProvider/controllerConnector/controllerconnector.h \
    library/mcuLinksProvider/binaryMaskHandler/binaryMaskHandler.h \


FORMS    += gui/mainwindow.ui \
    gui/points/addpointdialog.ui \
    gui/points/mousetoselectionpointdialog.ui \
    gui/points/editpointdialog.ui \
    gui/commands/adddevicedialog.ui \
#    gui/commands/linedialog.ui \
#    gui/commands/arcdialog.ui \
#    gui/commands/labeldialog.ui \
#    gui/commands/ttlinedialog.ui \
#    gui/commands/arc2dialog.ui \
#    gui/commands/cycledialog.ui \
#    gui/commands/offdialog.ui \
#    gui/commands/ondialog.ui \
#    gui/commands/rotatedialog.ui \
#    gui/commands/gotodialog.ui \
#    gui/commands/procdialog.ui \
#    gui/commands/callprocdialog.ui \
#    gui/commands/scaledialog.ui \
#    gui/commands/velocitydialog.ui \
#    gui/commands/pausedialog.ui \
#    gui/commands/stopdialog.ui \
#    gui/commands/commentdialog.ui \
#    gui/commands/ttzarcdialog.ui \
#    gui/commands/tttarcdialog.ui \
#    gui/commands/splinedialog.ui \
#    gui/commands/ttttsplinedialog.ui

win32:RC_FILE = application.rc
macx:ICON = $${PWD}/applicationOSX.icns

RESOURCES += \
    gui/images.qrc

DISTFILES += \
    machinetool/components/movementController/about.txt
