#include <boost/algorithm/string.hpp>
#include <boost/program_options/errors.hpp>

#include "options.h"

std::ostream& operator<< (std::ostream& out, ProgramCommand pc) {
    switch (pc) {
        case COMMAND_LIST:
            out<<pcList;
            break;
        case COMMAND_UPDATE:
            out<<pcUpdate;
            break;
        case COMMAND_ADD:
            out<<pcAdd;
            break;
        case COMMAND_DUMP:
            out<<pcDump;
            break;
        default:
            out<<"UNKONWN";
            break;
    };
    return out;
}

std::istream& operator>> (std::istream &in, ProgramCommand &command) {
    std::string token;
    in >> token;
    boost::to_lower(token);
    if (token == pcUpdate) {
        command = COMMAND_UPDATE;
    } else if (token == pcAdd) {
        command = COMMAND_ADD;
    } else if (token == pcDump) {
        command = COMMAND_DUMP;
    } else if (token == pcList) {
        command = COMMAND_LIST;
    } else {
        throw boost::program_options::validation_error(
            boost::program_options::validation_error::invalid_option_value,
            "Invalid Argument"
        );
    }

    return in;
}
