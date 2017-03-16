#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <thread>
#include <stack>
#include <string>
#include <map>

#include "commandhandler.h"



class CommandInterpreter
{
public:
    void Run();
    void Stop();
    void Step();


    std::vector <Command> getCommands();
    unsigned int getSelectedCommand();
    void setSelectedCommand(unsigned int number);

    bool getSelectedCommandEditSignal();
    void setSelectedCommandEditSignal(bool value);

    void addCommand(Command cmd, unsigned int selected_command);
    void addCommand(std::string cmd);
    void editCommand(Command cmd, unsigned int selected_command);
    void deleteSelectedCommands(unsigned int numbers);

    static CommandInterpreter& Instance();

private:
    std::vector<Command> commands;
    unsigned int selectedCommand;
    bool selectedCommandEditSignal;

    unsigned int currentCommand;


    std::thread t;

    std::stack<int> callStack;
    std::stack<std::pair<int, int> > loopStack;
    std::map<std::string, int> functionsMap;
    std::map<std::string, int> labelsMap;

    bool running;
    void _Run();

    CommandInterpreter();
    ~CommandInterpreter();

    CommandInterpreter(const CommandInterpreter&);
    CommandInterpreter& operator=(const CommandInterpreter&);


};

#endif // COMMANDINTERPRETER_H
