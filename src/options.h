#ifndef SAVED_GAME_FORMAT_MAIN_OPTIONS_H__
#define SAVED_GAME_FORMAT_MAIN_OPTIONS_H__

#include <string>
#include <iostream>

const std::string pcAdd("add");
const std::string pcDump("dump");
const std::string pcList("list");
const std::string pcUpdate("update");

enum ProgramCommand {
    COMMAND_LIST=0,
    COMMAND_ADD,
    COMMAND_DUMP,
    COMMAND_UPDATE,
};

std::ostream& operator<< (std::ostream& out, ProgramCommand pc);
std::istream& operator>> (std::istream &in, ProgramCommand &command);

#endif //SAVED_GAME_FORMAT_MAIN_OPTIONS_H__
