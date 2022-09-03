// cxx-async-example/include/coroutine_example.h

#ifndef COROUTINE_EXAMPLE_H
#define COROUTINE_EXAMPLE_H

#include <cstdint>
#include <vector>

#include "rust/cxx.h"

rust::Vec<uint8_t> reencode_jpeg(rust::Slice<const uint8_t> jpeg_data);

#endif