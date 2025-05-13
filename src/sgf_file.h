#ifndef SAVED_GAME_FORMAT_SGF_FILE_H__
#define SAVED_GAME_FORMAT_SGF_FILE_H__

#include <deque>
#include <fstream>
#include <iostream>
#include <string>

#include <archive.h>

namespace saved_game_format_file {

    class SavedGameFormatFile {
        public:
            friend std::ostream& operator<<(std::ostream& out, const SavedGameFormatFile& sgff);
            friend std::istream& operator<<(std::istream& in, SavedGameFormatFile& sgff);

            typedef std::deque<std::string> path_listing;

            SavedGameFormatFile() {}
            SavedGameFormatFile(std::string _default) : filename(_default) {}

            inline std::string GetFilename() { return filename; }
            bool Validate();

            void ListFiles(path_listing& listing);
            void DumpSubfile(const std::string& subfile_name, std::string& data);
            bool UpdateSubfile(const std::string& output_file, const std::string& subfile_name, const std::string& data);

            std::string filename{};

        protected:
            FILE* do_open_file(std::string _filename, bool write);

        private:
    };

}

#endif //SAVED_GAME_FORMAT_SGF_FILE_H__
