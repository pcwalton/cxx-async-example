// cxx-async-example/src/coroutine_example.cpp
//
// A minimal practical example of `cxx-async` usage.

#define FOLLY_HAS_COROUTINES 1
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "coroutine_example.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <folly/Executor.h>
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/experimental/coro/Task.h>
#include <cstdint>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

folly::Executor::KeepAlive<folly::CPUThreadPoolExecutor> g_thread_pool(
    new folly::CPUThreadPoolExecutor(8));

folly::coro::Task<rust::Vec<uint8_t>> reencode_jpeg(rust::Slice<const uint8_t> jpeg_data)
{
    // Load the JPEG image.
    int width, height, channels;
    uint8_t *pixels = stbi_load_from_memory(
        jpeg_data.data(), jpeg_data.size(), &width, &height, &channels, 4);
    if (pixels == nullptr)
        throw std::runtime_error("Failed to load image!");

    // Write the PNG to a temporary file.
    // `stb_image_write` doesn't support writing directly to an in-memory buffer, so we have to go
    // through a file first.
    char tmpPath[32] = "/tmp/imageXXXXXX";
    int fd = mkstemp(tmpPath);
    if (fd < 0)
        throw std::runtime_error("Couldn't create temporary file!");
    int ok = stbi_write_png(tmpPath, width, height, 4, pixels, width * 4);
    if (ok == 0)
        throw std::runtime_error("Couldn't reencode image!");

    // Read that temporary file back to memory.
    rust::Vec<uint8_t> encodedPNG;
    uint8_t buffer[4096];
    size_t nRead;
    while ((nRead = read(fd, buffer, sizeof(buffer))) > 0)
        std::copy(&buffer[0], &buffer[nRead], std::back_inserter(encodedPNG));
    if (nRead < 0)
        throw std::runtime_error("Failed to reread written image file to memory!");

    // Clean up and return the decoded PNG to Rust.
    close(fd);
    unlink(tmpPath);
    co_return encodedPNG;
}

// Asynchronously reencodes a JPEG to PNG via a thread pool.
RustFutureVecU8 reencode_jpeg_async(rust::Slice<const uint8_t> jpeg_data)
{
    co_return co_await reencode_jpeg(std::move(jpeg_data)).semi().via(g_thread_pool);
}