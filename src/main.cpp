#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/system/error_code.hpp>

#include "options.h"
#include "sgf_file.h"

enum SGFErrorCodes {
    SGF_OKAY = 0,
    SGF_INVALID_PARAMETER,
    SGF_MISSING_INPUT_FILE,
    SGF_LOAD_FILE_FAILED,
};

int main(int argc, const char* argv[]) {
    ProgramCommand command;
    saved_game_format_file::SavedGameFormatFile saved_game_file;
    std::string internal_file;
    std::string read_key;
    std::string output_file;
    std::pair<std::string, std::string> kv_pair;

    boost::program_options::options_description arg_descriptions("Allowed arguments");
    arg_descriptions.add_options()
        ("help", "display the help screen")
        (
            "file",
            boost::program_options::value<std::string>(&saved_game_file.filename),
            "file to operate on"
        )
        (
            "command",
            boost::program_options::value<ProgramCommand>(&command)->default_value(COMMAND_DUMP),
            "command to perform: [update, add, dump]"
        )
        (
            "subfile",
            boost::program_options::value<std::string>(&internal_file),
            "internal file to read"
        )
        (
            "read_key",
            boost::program_options::value<std::string>(&read_key),
            "key to read from file"
        )
        (
            "write_key",
            boost::program_options::value<std::string>(&kv_pair.first),
            "key within the file to write"
        )
        (
            "write_value",
            boost::program_options::value<std::string>(&kv_pair.second),
            "value to update the key to"
        )
        (
            "output",
            boost::program_options::value<std::string>(&output_file),
            "optional file to save data to"
        )
    ;

    boost::program_options::variables_map vm;
    try {
        auto parsed_options = boost::program_options::parse_command_line(argc, argv, arg_descriptions);
        boost::program_options::store(parsed_options, vm);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: "<<e.what() << std::endl;
        std::cout << arg_descriptions << std::endl;
        return SGF_INVALID_PARAMETER;
    }

    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cerr << "Dumping help screen" << std::endl;
        std::cout << arg_descriptions << std::endl;
        return SGF_OKAY;
    } else if (!vm.count("file")) {
        std::cerr << "Missing input file" << std::endl;
        std::cout << arg_descriptions << std::endl;
        return SGF_MISSING_INPUT_FILE;
    }

    if (!saved_game_file.Validate()) {
        std::cerr << "Invalid input file" << std::endl;
        std::cout << arg_descriptions << std::endl;
        return SGF_MISSING_INPUT_FILE;
    }
    std::cout << "Processing File: "<< saved_game_file.filename << std::endl;
    std::cout << "Command: "<< command << std::endl;
    std::cout << "[optional] Internal File: " << internal_file << std::endl;
    std::cout << "[optional] Read Key: " << read_key << std::endl;
    std::cout << "[optional] Write Key: " << kv_pair.first << std::endl;
    std::cout << "[optional] Write Value: " << kv_pair.second << std::endl;
    std::cout << "[optional] Output File: " << output_file << std::endl;

    switch (command) {
        case COMMAND_LIST:
            {
                std::cout << "List files" << std::endl;
                saved_game_format_file::SavedGameFormatFile::path_listing file_list;
                saved_game_file.ListFiles(file_list);
                std::cout <<"Entry Count: " << file_list.size() << std::endl;
                for (auto file_name : file_list) {
                    std::cout << "\t" << file_name << std::endl;
                }
            }
            break;
        case COMMAND_UPDATE:
            {
                std::cout << "Update key-value pair" << std::endl;
                std::string file_contents;
                saved_game_format_file::SavedGameFormatFile::path_listing file_list;
                saved_game_file.DumpSubfile(internal_file, file_contents);
                std::cout << "File Contents Size: " << file_contents.size() << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << "Begin File Contents " << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << file_contents << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << "End File Contents " << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;

                boost::system::error_code json_parser_error;
                boost::json::value json_data = boost::json::parse(file_contents.c_str(), json_parser_error);
                if (json_parser_error) {
                    std::cerr << "Error parsing JSON content: " << json_parser_error.message() << std::endl;
                    break;
                }
                std::cout << "Parsed JSON data" << std::endl;
                std::cout << "JSON data: " << json_data << std::endl;

                try {
                    boost::json::object& jd_obj = json_data.as_object();
                    auto jd_value = jd_obj.contains(kv_pair.first);
                    if (!jd_value) {
                        std::cerr << "Unable to locate key " << kv_pair.first << std::endl;
                        break;
                    }

                    jd_obj[kv_pair.first] = kv_pair.second;
                } catch (const std::exception& e) {
                    std::cerr << "[UPDATE] JSON Decode Error: " << e.what() << std::endl;
                }
                std::cout << "Updated JSON: " << json_data << std::endl;

                std::string updated_data = boost::json::serialize(json_data);

                saved_game_file.UpdateSubfile(output_file, internal_file, updated_data);
            }
            break;
        case COMMAND_ADD:
            {
                std::cout << "Insert key-value pair" << std::endl;
                std::string file_contents;
                saved_game_format_file::SavedGameFormatFile::path_listing file_list;
                saved_game_file.DumpSubfile(internal_file, file_contents);
                std::cout << "File Contents Size: " << file_contents.size() << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << "Begin File Contents " << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << file_contents << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;
                std::cout << "End File Contents " << std::endl;
                std::cout << "---------------------------------------------------------------" << std::endl;

                boost::system::error_code json_parser_error;
                boost::json::value json_data = boost::json::parse(file_contents.c_str(), json_parser_error);
                if (json_parser_error) {
                    std::cerr << "Error parsing JSON content: " << json_parser_error.message() << std::endl;
                    break;
                }
                std::cout << "Parsed JSON data" << std::endl;
                std::cout << "JSON data: " << json_data << std::endl;

                try {
                    boost::json::object& jd_obj = json_data.as_object();
                    jd_obj[kv_pair.first] = kv_pair.second;
                } catch (const std::exception& e) {
                    std::cerr << "[ADD] JSON Decode Error: " << e.what() << std::endl;
                }
                std::cout << "Updated JSON: " << json_data << std::endl;

                std::string updated_data = boost::json::serialize(json_data);

                saved_game_file.UpdateSubfile(output_file, internal_file, updated_data);
            }
            break;
        case COMMAND_DUMP:
            {
                std::cout << "Dump sub-file contents" << std::endl;
                std::string file_contents;
                saved_game_format_file::SavedGameFormatFile::path_listing file_list;
                saved_game_file.DumpSubfile(internal_file, file_contents);
                std::cout << "File Contents Size: " << file_contents.size() << std::endl;
                if (!read_key.length()) {
                    std::cout << "---------------------------------------------------------------" << std::endl;
                    std::cout << "Begin File Contents " << std::endl;
                    std::cout << "---------------------------------------------------------------" << std::endl;
                    std::cout << file_contents << std::endl;
                    std::cout << "---------------------------------------------------------------" << std::endl;
                    std::cout << "End File Contents " << std::endl;
                    std::cout << "---------------------------------------------------------------" << std::endl;
                } else {
                    boost::system::error_code json_parser_error;
                    boost::json::value json_data = boost::json::parse(file_contents.c_str(), json_parser_error);
                    if (json_parser_error) {
                        std::cerr << "Error parsing JSON content: " << json_parser_error.message() << std::endl;
                        break;
                    }
                    try {
                        boost::json::object& jd_obj = json_data.as_object();
                        std::cout << "Parsed JSON data" << std::endl;
                        std::cout << "Key " << read_key << " = " << jd_obj[read_key] << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "[DUMP] JSON Decode Error: " << e.what() << std::endl;
                    }

                }
            }
            break;
        default:
            std::cout << "Unknown Operation" << std::endl;
            break;
    };

    return SGF_OKAY;
}
