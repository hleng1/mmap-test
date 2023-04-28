#include <iostream>
#include <fstream>
#include <cstdint>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>


bool open_mapped_file_portion(boost::iostreams::mapped_file_source& mmap_file,
                              const std::string& file_path, size_t offset, size_t length) {
    boost::iostreams::mapped_file_params params;
    params.path = file_path;
    params.flags = boost::iostreams::mapped_file_base::mapmode::readonly;
    params.offset = offset;
    params.length = length;

    try {
        mmap_file.open(params);
    } catch (const boost::iostreams::filesystem_error& e) {
        std::cerr << "Error mapping file: " << e.what() << std::endl;
        return false;
    }

    return mmap_file.is_open();
}

void close_mapped_file_portion(boost::iostreams::mapped_file_source& mmap_file) {
    mmap_file.close();
}

int main() {
    const std::string file_path = "large_file.bin";
    size_t offset = 0; // Set this to the desired starting position in the file
    size_t length = 1024 * 1024 * 1024; // 1 GB, adjust as needed

    boost::iostreams::mapped_file_source mmap_file;
    if (!open_mapped_file_portion(mmap_file, file_path, offset, length)) {
        std::cerr << "Error opening file for memory mapping" << std::endl;
        return 1;
    }

    const char* data = mmap_file.data();

    int64_t value;
    std::memcpy(&value, data, sizeof(int64_t));
    std::cout << "First 8 bytes as a 64-bit integer: " << value << std::endl;

    close_mapped_file_portion(mmap_file);
    return 0;
}

