#ifndef SAVED_GAME_FORMAT_MAIN_OPTIONS_H__
#define SAVED_GAME_FORMAT_MAIN_OPTIONS_H__

#include <string>
#include <iostream>

const std::string pcUpdate("update");
const std::string pcAdd("add");
const std::string pcDump("dump");

enum ProgramCommand {
    COMMAND_UPDATE=0,
    COMMAND_ADD,
    COMMAND_DUMP,
};

std::ostream& operator<< (std::ostream& out, ProgramCommand pc);
std::istream& operator>> (std::istream &in, ProgramCommand &command);

#endif //SAVED_GAME_FORMAT_MAIN_OPTIONS_H__
