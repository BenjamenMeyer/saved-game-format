#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options/errors.hpp>

#include <archive.h>
#include <archive_entry.h>

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

FILE* SavedGameFormatFile::do_open_file(std::string _filename, bool write) {
    const char* permissions = "r+b";
    if (write) {
        permissions = "w+b";
    }

    FILE* data_file = fopen(_filename.c_str(), permissions);
    if (data_file == nullptr) {
        std::cerr << "unable to open data file. errno: " << errno << std::endl;
        return nullptr;
    }

    int ferr = fseeko(data_file, 0, SEEK_END);
    if (ferr != 0) {
        std::cerr << "unable to set file pointer, errno " << errno << std::endl;
    } else {
        off_t file_length = ftello(data_file);
        std::cerr << "file is " << file_length << " bytes long" << std::endl;
    }

    return data_file;
}

void SavedGameFormatFile::ListFiles(path_listing& listing) {
    FILE* data_file = do_open_file(filename, false);
    if (data_file == nullptr) {
        std::cerr << "unable to open file " << filename << std::endl;
        return;
    }

    int ferr = fseeko(data_file, 0, SEEK_SET);
    if (ferr != 0) {
        std::cerr << "unable to reset file pointer, errno " << errno << std::endl;
        return;
    }

    struct archive* archive_file = archive_read_new();
    archive_read_support_filter_all(archive_file);
    archive_read_support_format_all(archive_file);

    int aerr = archive_read_open_FILE(archive_file, data_file);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error opening archive: " << archive_error_string(archive_file) << std::endl;
        return;
    }

    struct archive_entry* current_archive_entry = nullptr;
    while (archive_read_next_header(archive_file, &current_archive_entry) == ARCHIVE_OK) {
        auto entry_path = archive_entry_pathname(current_archive_entry);
        std::string value(entry_path);
        listing.push_back(value);
    }
    aerr = archive_read_free(archive_file);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error closing archive: " << archive_error_string(archive_file) << std::endl;
        return;
    }
    fclose(data_file);

    return;
}

void SavedGameFormatFile::DumpSubfile(const std::string& subfile_name, std::string& data) {
    FILE* data_file = do_open_file(filename, false);
    if (data_file == nullptr) {
        std::cerr << "unable to open file " << filename << std::endl;
        return;
    }

    int ferr = fseeko(data_file, 0, SEEK_SET);
    if (ferr != 0) {
        std::cerr << "unable to reset file pointer, errno " << errno << std::endl;
        return;
    }

    struct archive* archive_file = archive_read_new();
    archive_read_support_filter_all(archive_file);
    archive_read_support_format_all(archive_file);

    int aerr = archive_read_open_FILE(archive_file, data_file);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error opening archive: " << archive_error_string(archive_file) << std::endl;
        return;
    }

    struct archive_entry* current_archive_entry = nullptr;
    while (archive_read_next_header(archive_file, &current_archive_entry) == ARCHIVE_OK) {
        auto entry_path = archive_entry_pathname(current_archive_entry);
        std::string value(entry_path);

        if (value == subfile_name) {
            const void* buffer = nullptr;
            size_t buffer_size = 0;
            la_int64_t archive_offset = 0;

            aerr = archive_read_data_block(archive_file, &buffer, &buffer_size, &archive_offset);
            if (aerr == ARCHIVE_EOF) {
                break;
            }
            if (aerr < ARCHIVE_OK) {
                std::cerr << "Error reading archive chunk: " << archive_error_string(archive_file) << std::endl;
                break;
            }
            data += (char*)buffer;
        }
    }
    aerr = archive_read_free(archive_file);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error closing archive: " << archive_error_string(archive_file) << std::endl;
        return;
    }
    fclose(data_file);

    return;
}

bool SavedGameFormatFile::UpdateSubfile(const std::string& output_file, const std::string& subfile_name, const std::string& data) {
    FILE* data_file = do_open_file(filename, false);
    if (data_file == nullptr) {
        std::cerr << "unable to open file " << filename << std::endl;
        return false;
    }

    int ferr = fseeko(data_file, 0, SEEK_SET);
    if (ferr != 0) {
        std::cerr << "unable to reset file pointer, errno " << errno << std::endl;
        return false;
    }

    FILE* data_output = do_open_file(output_file, true);
    if (data_output == nullptr) {
        std::cerr << "unable to open file " << output_file << std::endl;
        return false;
    }

    ferr = fseeko(data_output, 0, SEEK_SET);
    if (ferr != 0) {
        std::cerr << "unable to reset file pointer, errno " << errno << std::endl;
        return false;
    }

    struct archive* archive_file = archive_read_new();
    archive_read_support_filter_all(archive_file);
    archive_read_support_format_all(archive_file);

    struct archive* archive_output = archive_write_new();
    // we can't copy over the settings from the input file
    // so we have to set on for the output
    archive_write_add_filter_bzip2(archive_output);
    archive_write_set_format_gnutar(archive_output);

    int aerr = archive_read_open_FILE(archive_file, data_file);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error opening input archive: " << archive_error_string(archive_file) << std::endl;
        return false;
    }

    aerr = archive_write_open_FILE(archive_output, data_output);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error opening output archive: " << archive_error_string(archive_file) << std::endl;
        return false;
    }

    struct archive_entry* current_archive_entry = nullptr;
    while (archive_read_next_header(archive_file, &current_archive_entry) == ARCHIVE_OK) {
        auto entry_path = archive_entry_pathname(current_archive_entry);
        std::string value(entry_path);


        if (value == subfile_name) {
            auto entry = archive_entry_clone(current_archive_entry);
            archive_entry_set_size(entry, data.size());

            la_ssize_t data_written = archive_write_data(archive_output, data.c_str(), data.size());
            if (data_written != data.size()) {
                std::cerr << "Only " << data_written << " bytes of " << data.size() << " bytes were actually written" << std::endl;
                std::cerr << "Update File Data Error: " << archive_error_string(archive_output) << std::endl;
            }
        } else {
            // read the data for the file
            archive_write_header(archive_output, current_archive_entry);
            while (true) {
                const void* buffer = nullptr;
                size_t buffer_size = 0;
                la_int64_t archive_offset = 0;

                aerr = archive_read_data_block(archive_file, &buffer, &buffer_size, &archive_offset);
                if (aerr == ARCHIVE_EOF) {
                    break;
                }
                if (aerr < ARCHIVE_OK) {
                    std::cerr << "Error reading archive chunk: " << archive_error_string(archive_file) << std::endl;
                    break;
                }

                //la_ssize_t data_written = archive_write_data_block(archive_output, buffer, buffer_size, archive_offset);
                la_ssize_t data_written = archive_write_data(archive_output, buffer, buffer_size);
                if (data_written != buffer_size) {
                    std::cerr << "Only " << data_written << " bytes of " << buffer_size << " bytes were actually written" << std::endl;
                    std::cerr << "Write Existing File Data Error: " << archive_error_string(archive_output) << std::endl;
                }
            }
        }
    }
    aerr = archive_read_free(archive_file);
    if (aerr != ARCHIVE_OK) {
        std::cerr << "Error closing archive: " << archive_error_string(archive_file) << std::endl;
        return false;
    }
    fclose(data_file);
    fclose(data_output);

    return true;
}
