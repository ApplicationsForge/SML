#include "commandsinterpreter.h"

CommandsInterpreter::CommandsInterpreter()
{
}

CommandsInterpreter::~CommandsInterpreter()
{
}

std::vector< std::shared_ptr<SMLCommand> > CommandsInterpreter::updateProgram(
        std::vector< std::shared_ptr<SMLCommand> > commands,
        PointsManager* pointsManager,
        DevicesManager* devicesManager)
{
    return makeProgram(commands, pointsManager, devicesManager);
}

std::vector< std::shared_ptr<SMLCommand> > CommandsInterpreter::makeProgram(
        std::vector< std::shared_ptr<SMLCommand> > commands,
        PointsManager* pointsManager,
        DevicesManager* devicesManager)
{
    std::vector< std::shared_ptr<SMLCommand> > m_commands;

    for(unsigned int i = 0; i < commands.size(); i++)
    {
        std::shared_ptr<SMLCommand> cmd = commands[i];
        m_commands.push_back(SMLCommandsBuilder::buildCommand(cmd->getId(), cmd->getArguments(), pointsManager, devicesManager));
    }


    m_commands = eraseComments(m_commands);
    m_commands = inlineVariables(m_commands);
    return m_commands;
}

std::vector<std::shared_ptr<SMLCommand> > CommandsInterpreter::eraseCommands(QList<size_t> indexes, std::vector<std::shared_ptr<SMLCommand> > commands)
{
    std::sort(indexes.begin(), indexes.begin() + indexes.size());
    for(int i = indexes.size() - 1; i >= 0; i--)
    {
        commands.erase(commands.begin() + indexes[i]);
    }
    return commands;
}

std::vector<std::shared_ptr<SMLCommand> > CommandsInterpreter::inlineVariables(std::vector<std::shared_ptr<SMLCommand> > commands)
{
    std::vector<std::shared_ptr<SMLCommand> > tmpCommands = commands;
    QList<size_t> variablesIndexes;
    for(size_t i = 0; i < tmpCommands.size(); i++)
    {
        if(tmpCommands[i]->getId() == CMD_VARIABLE)
        {
            variablesIndexes.push_back(i);
            QStringList variableArguments = tmpCommands[i]->getArguments();
            commands = inlineVariable(variableArguments[0], variableArguments[1], commands);
        }
    }
    commands = eraseCommands(variablesIndexes, commands);
    return commands;
}

std::vector<std::shared_ptr<SMLCommand> > CommandsInterpreter::inlineVariable(QString key, QString value, std::vector<std::shared_ptr<SMLCommand> > commands)
{
    for(auto command : commands)
    {
        size_t id = command->getId();
        if(id != CMD_VARIABLE)
        {
            QStringList arguments = command->getArguments();
            if(arguments.contains(key))
            {
                for(int i = 0; i < arguments.size(); i++)
                {
                    if(arguments[i] == key)
                    {
                        arguments[i] = value;
                    }
                }
            }
            command->setArguments(arguments);
        }
    }
    return commands;
}

std::vector<std::shared_ptr<SMLCommand> > CommandsInterpreter::eraseComments(std::vector<std::shared_ptr<SMLCommand> > commands)
{
    QList<size_t> indexes;
    for(size_t i = 0; i < commands.size(); i++)
    {
        if(commands[i]->getId() == CMD_COMMENT)
        {
            indexes.push_back(i);
        }
    }
    commands = eraseCommands(indexes, commands);
    return commands;
}

