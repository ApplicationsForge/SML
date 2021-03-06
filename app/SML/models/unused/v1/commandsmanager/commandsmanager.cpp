#include "commandsmanager.h"

CommandsManager::CommandsManager(std::vector<std::shared_ptr<SMLCommand> > cmds) :
    m_commands(cmds)
{
}

CommandsManager::CommandsManager(const CommandsManager &object) :
    m_commands(object.m_commands)
{

}

std::shared_ptr<SMLCommand>& CommandsManager::operator[](size_t idx)
{
    if (idx < m_commands.size())
    {
        return m_commands[idx];
    }
    else
    {
        std::string errMsg = "Нет команды с номером " + std::to_string(idx);
        errMsg += " (всего " + std::to_string(m_commands.size()) + " команд)";

        throw std::out_of_range(errMsg);
    }
}

unsigned int CommandsManager::commandsCount()
{
    return m_commands.size();
}

/*std::vector<std::shared_ptr<Command> >& CommandsManager::getCommands()
{
    return m_commands;
}*/

void CommandsManager::addCommand(SMLCommand *cmd)
{
    addCommand(std::shared_ptr<SMLCommand>(cmd));
}

void CommandsManager::addCommand(std::shared_ptr<SMLCommand> cmd)
{
    m_commands.push_back(cmd);
}

void CommandsManager::deleteCommand(SMLCommand *cmd)
{
    deleteCommand(std::shared_ptr<SMLCommand>(cmd));
}

void CommandsManager::deleteCommand(std::shared_ptr<SMLCommand> cmd)
{
    m_commands.erase( std::remove(m_commands.begin(), m_commands.end(), cmd), m_commands.end() );
}

void CommandsManager::deleteCommand(size_t idx)
{
    if (idx < m_commands.size())
    {
        m_commands.erase(m_commands.begin() + idx);
    }
    else
    {
        std::string errMsg = "Нет команды с номером " + std::to_string(idx);
        errMsg += " (всего " + std::to_string(m_commands.size()) + " команд)";

        throw std::out_of_range(errMsg);
    }
}

void CommandsManager::deleteCommands(size_t beginIdx, size_t endIdx)
{
    size_t commandsSize = commandsCount();
    if((beginIdx <= commandsSize) && (endIdx <= commandsSize))
    {
        m_commands.erase(m_commands.begin() + beginIdx, m_commands.begin() + endIdx);
    }
}

void CommandsManager::insertCommand(size_t pos, std::shared_ptr<SMLCommand> cmd)
{
    if (pos < m_commands.size())
    {
        m_commands.insert(m_commands.begin() + pos, cmd);
    }
    else
    {
        m_commands.push_back(cmd);
    }
}
