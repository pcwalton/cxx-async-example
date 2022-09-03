use std::env;
use std::path::PathBuf;

fn main() {
    let mut build = cxx_build::bridge("src/main.rs");

    let boost_root = PathBuf::from(env::var("BOOST_ROOT").expect(
        "You must set `BOOST_ROOT`; e.g. if Boost is installed via Homebrew, try \
        `BOOST_ROOT=/usr/local cargo build`"));

    build
        .flag_if_supported("-std=c++20")
        .include("include")
        .include(&boost_root.join("include"));

    println!("cargo:rustc-link-search={}", boost_root.join("lib").display());
    println!("cargo:rustc-link-lib=boost_system-mt");

    build
        .file("src/coroutine_example.cpp")
        .file("src/stb_image.cpp")
        .compile("cxx-async-example");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/coroutine_example.cpp");
    println!("cargo:rerun-if-changed=include/coroutine_example.h");
}
