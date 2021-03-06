#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QKeySequence>
#include <QList>
#include <QMessageBox>
#include <QTreeWidget>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

/// Подключение диалогов редактора точек
#include "gui/add_point_dialog.h"
#include "gui/to_selection_point_dialog.h"

/// Подключение диалогов команд
/*#include "commands/linedialog.h"
#include "commands/smlttlinedialog.h"
#include "commands/arcdialog.h"
#include "commands/arc2dialog.h"
#include "commands/ttzarcdialog.h"
#include "commands/smltttarcdialog.h"
#include "commands/labeldialog.h"
#include "commands/cycledialog.h"
#include "commands/procdialog.h"
#include "commands/smlswitchondialog.h"
#include "commands/smlswitchoffdialog.h"
#include "commands/rotatedialog.h"
#include "commands/gotodialog.h"
#include "commands/callprocdialog.h"
#include "commands/scaledialog.h"
#include "commands/velocitydialog.h"
#include "commands/smlpausedialog.h"
#include "commands/stopdialog.h"
#include "commands/smlcommentdialog.h"
#include "commands/splinedialog.h"
#include "commands/ttttsplinedialog.h"
#include "commands/smlvariabledialog.h"*/

/// Подключение подсветки синтаксиса G-кодов
#include "gui/utils/gcodes_syntax_highlighter.h"

/// Подключение диалогов опций
/*#include "gui/options/kabriolwindow.h"
#include "gui/options/toollengthsensorwindow.h"
#include "gui/options/lubricationsystemwindow.h"*/

#include "models/machine_tool.h"

#include "gui/sml_console_dialog.h"
#include "gui/widgets/spindel_control_widget.h"

#include "gui/program_processing_dialog.h"

#include "models/services/gcodes/view/gcodes_view_interactor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;

    /// Горячие клавиши кнопок для перемещения по осям станка
    QList< std::tuple<const char*, QPushButton*, const char*> > m_shortcutsMap;
    QList<QShortcut*> m_axisesShortcuts;

    QList<QMetaObject::Connection> m_connections;

    /// Подсветки синтаксиса в редакторе G-кодов
    QScopedPointer<GCodesSyntaxHighlighter> m_hightlighter;

    /// Проводит начальную настройку виджетов
    void setupWidgets();

    /// Проводит инициализацию дисплея датчиков
    void setupSensorsDisplay();

    /// Проводит инициализацию таблицы настроек датчиков
    void setupSensorsSettingsBoard();

    /// Проводит инициализацию панели шпинделей
    void setupSpindelsControlPanel();

    /// Проводит инициализацию таблицы настроек шпинделей
    void setupSpindelsSettingsBoard();

    /// Проводит инициализацию таблицы настроек сопуствующих устройств
    void setupSupportDevicesControlPanel();

    /// Проводит инициализацию таблицы настроек сопуствующих устройств
    void setupSupportDevicesSettingsBoard();

    /// Проводит настройку дисплеев координат
    void setupCoordinatesDisplays();

    /// Проводит инициализацию таблицы настроек осей
    void setupAxisesSettingsBoard();

    /// Проводит настройку кнопок перемещений в наладке и их горячих клавиш
    void setupAxisesButtons();

    /// Настраивает панель опций
    void setupOptionsPanel();

    /// Блокирует и скрывает виджеты, которые не нужны в данной сборке, но нужны в принципе
    void hideWidgets();

    /// Подключает необходимые слоты к полям и сигналам
    void setupConnections();

    /// Отключает слоты от полей и сигналов
    void resetConnections();

    /// Методы для настройки работы горячих клавиш движения по осям станка
    void setMovementButtonsShortcutsAutoRepeat(bool state);
    void setMovementButtonsAutoRepeat(bool state);

    void enableMotionWidgets();
    void disableMotionWidgets();
    void setMotionWidgetsState(bool enableWidgets);

private slots:
    /// Слоты для отображения поддержки/отсутсвия связи со станком
    void onMachineTool_ErrorStateChanged(QList<ERROR_CODE> errors);

    /// слот для обновления дисплея датчиков
    void onMachineTool_SensorStateChanged(QString name, bool state);

    void onMachineTool_SpindelStateChanged(QString index, bool enable, size_t rotations);

    void onMachineTool_BasingStateChanged(bool state);

    /// Слот для обновления дисплея координат
    void updateCoordinatesDisplays();
    void showCoordinates(QListWidget* display, Point p);

    /// Слот для обновления дисплея заряда батареи
    void updateBatteryStatusDisplay();

    void onMachineTool_GCodesFileContentUpdated(QStringList data);
    void onMachineTool_GCodesFilePathUpdated(QString path);

    void onMachineTool_TaskCompletedSuccesfully();
    void onMachineTool_TaskCompletedWithErrors();

    void onMachineTool_CurrentCoordinatesChanged();

    // слот пока не используется
    void onMachineTool_EdgesControlStatusChanged(bool state);

    void addPoint();
    void editPoint(QModelIndex index);
    void deletePoints(QModelIndexList indexes);
    void onPointsUpdated();

    /// Слоты для обработки сигналов виджета дерева SML-команд
    //void commandsCopySlot();
    //void commandsCutSlot();
    //void commandsPasteSlot();
    //void commandsUndoSlot();
    //void deleteSelectedCommands(QModelIndexList indexes);

    /// Слоты для прямого взаимодействия с элеменами интерфейса
    void on_discreteRadioButton_1_clicked();
    void on_discreteRadioButton_2_clicked();
    void on_discreteRadioButton_3_clicked();
    void on_discreteRadioButton_4_clicked();
    void on_discreteRadioButton_5_clicked();
    void on_movementXPositivePushButton_clicked();
    void on_movementXNegativePushButton_clicked();
    void on_movementYPositivePushButton_clicked();
    void on_movementYNegativePushButton_clicked();
    void on_movementXPositiveYPositivePushButton_clicked();
    void on_movementXPositiveYNegativePushButton_clicked();
    void on_movementXNegativeYPositivePushButton_clicked();
    void on_movementXNegativeYNegativePushButton_clicked();
    void on_movementZNegativePushButton_clicked();
    void on_movementZPositivePushButton_clicked();
    void on_movementAPositivePushButton_clicked();
    void on_movementANegativePushButton_clicked();
    void on_movementBNegativePushButton_clicked();
    void on_movementBPositivePushButton_clicked();
    void on_movementCNegativePushButton_clicked();
    void on_movementCPositivePushButton_clicked();
    void on_movementUNegativePushButton_clicked();
    void on_movementUPositivePushButton_clicked();
    void on_movementVNegativePushButton_clicked();
    void on_movementVPositivePushButton_clicked();
    void on_movementWNegativePushButton_clicked();
    void on_movementWPositivePushButton_clicked();
    void on_feedrateScrollBar_valueChanged(int value);
    Q_NORETURN void on_exit_action_triggered();
    void on_pointsAmountToolButton_clicked();
    void on_pointAddToolButton_clicked();
    void on_pointDeleteToolButton_clicked();
    void on_pointCursorToolButton_clicked();
    void on_pointEditToolButton_clicked();
    void on_pointCopyToolButton_clicked();
    void on_open_action_triggered();
    void on_gcodesEditorPlainTextEdit_textChanged();
    void on_importsettings_action_triggered();
    void on_savesettings_action_triggered();
    //void on_smlEditorTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    //void on_commandsToolsListWidget_clicked(const QModelIndex &index);
    //void on_devicesButtonsListWidget_clicked(const QModelIndex &index);
    void on_add_action_triggered();
    void on_create_action_triggered();
    void on_save_action_triggered();
    void on_saveas_action_triggered();
    void on_view_action_triggered();
    void on_consoleOpenPushButton_clicked();
    void on_edgesControlCheckBox_clicked();
    void on_syntaxHighlightingCheckBox_clicked();
    void on_runPushButton_clicked();
    void on_toBasePushButton_clicked();
    void on_zeroPushButton_clicked();
    void on_toZeroPushButton_clicked();
    void on_parkPushButton_clicked();
    void on_toParkPushButton_clicked();
};


#endif // MAINWINDOW_H
