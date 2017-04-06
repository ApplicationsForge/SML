#include "commandhandler.h"


void CMD_ARC_HANDLER(arguments args)
{
    MachineTool::Instance().stepMove( {1, 0, 0, 0, 0} );
}

std::string getNameByCommand(COMMAND cmd)
{
    return commandNameMap[cmd];
}


COMMAND getCommandByName(std::string name)
{
    for(auto it : commandNameMap)
    {
        if (it.second == name)
        {
            return it.first;
        }
    }
    return CMD_UNDEFINED;
}
