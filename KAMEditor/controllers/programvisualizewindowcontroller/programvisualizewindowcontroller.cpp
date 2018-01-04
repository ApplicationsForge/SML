#include "programvisualizewindowcontroller.h"

ProgramVisualizeWindowController::ProgramVisualizeWindowController(MainWindowController *_mainWindowController, QObject *parent) : QObject(parent)
{
    setup(_mainWindowController);
    emit ready();
}

ProgramVisualizeWindowController::~ProgramVisualizeWindowController()
{
    delete programVisualizeBridge;
}

std::vector<std::shared_ptr<SMLCommand> > ProgramVisualizeWindowController::getCommands()
{
    return interpretedCommands;
}

std::vector<std::shared_ptr<Point3D> > ProgramVisualizeWindowController::get3DPoints()
{
    std::vector< std::shared_ptr<Point3D> > points;

    size_t pointsCount = pointsManager->pointCount();

    for(unsigned int i = 0; i < pointsCount; i++)
    {
        try
        {
            std::shared_ptr<Point> originPoint = pointsManager->operator [](i);
            Point3D* point = new Point3D(originPoint->get("X"), originPoint->get("Y"), originPoint->get("Z"));
            points.push_back(std::shared_ptr<Point3D>(point));
        }
        catch(std::out_of_range e)
        {
            QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
            break;
        }
    }

    return points;
}

double ProgramVisualizeWindowController::getGridMaximalAccuracy() const
{
    return gridMaximalAccuracy;
}

void ProgramVisualizeWindowController::setup(MainWindowController* mainWindowController)
{
    programVisualizeBridge = new ProgramVisualizeWindowBridge();

    interpretedCommands = mainWindowController->interpretCommands();

    pointsManager = mainWindowController->machineTool->getPointsManager();

    SettingsManager settingsManager;
    try
    {
        gridMaximalAccuracy = settingsManager.get("Visualisation", "GridMaximalAccuracy").toDouble();
    }
    catch(std::invalid_argument e)
    {
        QMessageBox(QMessageBox::Warning, "Ошибка", e.what()).exec();
    }
}

QStringList ProgramVisualizeWindowController::getCommandsHeaders()
{
    return programVisualizeBridge->commandsHeaders();
}

QStringList ProgramVisualizeWindowController::getCommandsNumbers()
{
    return programVisualizeBridge->commandsNumbers(interpretedCommands.size());
}

QList<QTableWidgetItem *> ProgramVisualizeWindowController::getCommandsNames()
{
    return programVisualizeBridge->commandsNames(interpretedCommands);
}

QList<QTableWidgetItem *> ProgramVisualizeWindowController::getCommandsArguments()
{
    return programVisualizeBridge->commandsArguments(interpretedCommands);
}