#ifndef SAVED_GAME_FORMAT_SGF_FILE_H__
#define SAVED_GAME_FORMAT_SGF_FILE_H__

#include <string>
#include <iostream>

namespace saved_game_format_file {

class SavedGameFormatFile {
    public:
        friend std::ostream& operator<<(std::ostream& out, const SavedGameFormatFile& sgff);
        friend std::istream& operator<<(std::istream& in, SavedGameFormatFile& sgff);

        SavedGameFormatFile() {}
        SavedGameFormatFile(std::string _default): filename(_default) {}
        
        inline std::string GetFilename() { return filename; }
        bool Validate();

        std::string filename;
    protected:
    private:
};

}

#endif //SAVED_GAME_FORMAT_SGF_FILE_H__
