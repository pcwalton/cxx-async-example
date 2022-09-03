// cxx-async-example/src/coroutine_example.cpp
//
// A minimal practical example of `cxx-async` usage.

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "coroutine_example.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <cstdint>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

boost::asio::thread_pool g_thread_pool(8);

void reencode_jpeg(
    rust::Slice<const uint8_t> jpeg_data,
    rust::Fn<void(rust::Box<CallbackContext>, rust::Vec<uint8_t>)> callback,
    rust::Box<CallbackContext> context)
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

    callback(std::move(context), std::move(encodedPNG));
}

// Asynchronously reencodes a JPEG to PNG via a thread pool.
void reencode_jpeg_async(
    rust::Slice<const uint8_t> jpeg_data,
    rust::Fn<void(rust::Box<CallbackContext>, rust::Vec<uint8_t>)> callback,
    rust::Box<CallbackContext> context)
{
    boost::asio::post(g_thread_pool, [
        jpeg_data,
        callback = std::move(callback),
        context = std::move(context)
    ]() mutable {
        reencode_jpeg(jpeg_data, std::move(callback), std::move(context));
    });
}