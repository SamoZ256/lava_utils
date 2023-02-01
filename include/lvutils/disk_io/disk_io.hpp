#ifndef DISK_IO_H
#define DISK_IO_H

#include <iostream>
#include <fstream>
#include <sstream>

namespace lv {

void dumpRawBinary(const char* filename, void* data, size_t size);

void loadRawBinary(const char* filename, void** data, size_t* size);

} //namespace lv

#endif
