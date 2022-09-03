fn main() {
    let mut build = cxx_build::bridge("src/main.rs");

    build
        .flag_if_supported("-std=c++20")
        .include("include");

    build
        .file("src/coroutine_example.cpp")
        .file("src/stb_image.cpp")
        .compile("cxx-async-example");

    println!("cargo:rerun-if-changed=src/main.rs");
    println!("cargo:rerun-if-changed=src/coroutine_example.cpp");
    println!("cargo:rerun-if-changed=include/coroutine_example.h");
}
