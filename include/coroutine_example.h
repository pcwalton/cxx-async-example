// cxx-async-example/include/coroutine_example.h

#ifndef COROUTINE_EXAMPLE_H
#define COROUTINE_EXAMPLE_H

#include <cstdint>
#include <vector>

#include "rust/cxx.h"
#include "rust/cxx_async.h"

CXXASYNC_DEFINE_FUTURE(rust::Vec<uint8_t>, RustFutureVecU8);

RustFutureVecU8 reencode_jpeg_async(rust::Slice<const uint8_t> jpeg_data);

#endif