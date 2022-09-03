// cxx-async-example/include/coroutine_example.h

#ifndef COROUTINE_EXAMPLE_H
#define COROUTINE_EXAMPLE_H

#include <cstdint>
#include <vector>

#include "cxx-async-example/src/main.rs.h"
#include "rust/cxx.h"

void reencode_jpeg_async(
    rust::Slice<const uint8_t> jpeg_data,
    rust::Fn<void(rust::Box<CallbackContext>, rust::Vec<uint8_t>)> callback,
    rust::Box<CallbackContext> context);

#endif