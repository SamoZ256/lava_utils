#include "lvutils/disk_io/disk_io.hpp"

#include <filesystem>

namespace lv {

void dumpRawBinary(const char* filename, void* data, size_t size) {
    std::ofstream vertDataOut(filename, std::ios::out | std::ios::binary);
    vertDataOut.write((char*)data, size);
}

void loadRawBinary(const char* filename, void** data, size_t* size) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    *size = std::filesystem::file_size(filename);
    *data = malloc(*size);
    file.read((char*)*data, *size);
}

} //namespace lv
