#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options/errors.hpp>

#include "sgf_file.h"

using namespace saved_game_format_file;

std::ostream& operator<<(std::ostream& out, const saved_game_format_file::SavedGameFormatFile& sgff) {
    out << sgff.filename;
    return out;
}

std::istream& operator<<(std::istream& in, saved_game_format_file::SavedGameFormatFile& sgff) {
    in >> sgff.filename;
    // validate that the file exists, else throw an error
    return in;
}

bool SavedGameFormatFile::Validate() {
    if (!filename.length() ) {
        return false;
    }
    if (!boost::filesystem::exists(filename)) {
        return false;
    }

    return true;
}
