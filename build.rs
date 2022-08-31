use std::env;

fn main() {
    let mut build = cxx_build::bridge("src/main.rs");

    build
        .flag_if_supported("-std=c++20")
        .flag_if_supported("-fcoroutines-ts")
        .include("include");
    if let Ok(include_dir) = env::var("DEP_CXX_ASYNC_INCLUDE") {
        build.include(include_dir);
    } else {
        panic!("Where's the `cxx_async` `DEP`?")
    }

    let folly = find_folly::probe_folly().expect("Couldn't find Folly!");
    build.includes(&folly.include_paths);
    for cflag in &folly.other_cflags {
        build.flag(cflag);
    }

    build
        .file("src/coroutine_example.cpp")
        .file("src/stb_image.cpp")
        .compile("cxx-async-example");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/coroutine_example.cpp");
    println!("cargo:rerun-if-changed=include/coroutine_example.h");
}
