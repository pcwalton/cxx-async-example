# `cxx-async` example

## Overview

This is a minimal example server to demonstrate [`cxx-async`](https://github.com/pcwalton/cxx-async). It uses the [`cxx`](https://cxx.rs/), [Actix Web](https://actix.rs/), [`stb_image`](https://github.com/nothings/stb), [`stb_image_write`](https://github.com/nothings/stb), and [Folly](https://github.com/facebook/folly) libraries, together with the C++20 coroutines feature, to create a simple asynchronous Rust REST server that accepts JPEG images and reencodes them as PNGs via a C++ library. The image processing is proxied out to a thread pool *on the C++ side* and fully interoperates with the async nature of Actix. This allows you to use arbitrary threaded C++ libraries without the callback soup that would otherwise occur. The Rust and C++ combined come out to only 80 lines of code!

The logic lives in `src/main.rs` and `src/coroutine_example.cpp`.

## Running this example

    $ cargo run
    Running `target/debug/cxx-async-example`
    running on 127.0.0.1:8765

In another terminal, replacing `ferris.jpg` with whatever image you want to reencode:

    $ curl --data-binary @ferris.jpg -H "Content-Type: image/jpeg" --output out.png http://127.0.0.1:8765/convert

Open `out.png` to see the results.

## License

Any copyright is dedicated to the Public Domain.

https://creativecommons.org/share-your-work/public-domain/cc0/
