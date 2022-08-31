// cxx-async-example/src/main.rs
//
//! A minimal practical example of `cxx-async` usage.

use actix_web::web::Bytes;
use actix_web::{post, App, HttpServer, Responder};
use std::future::Future;
use std::io::Result as IoResult;

// The `cxx` bridge that declares the asynchronous function we want to call.
#[cxx::bridge]
mod ffi {
    unsafe extern "C++" {
        include!("coroutine_example.h");
        type RustFutureVecU8 = crate::RustFutureVecU8;
        fn reencode_jpeg_async(jpeg_data: &[u8]) -> RustFutureVecU8;
    }
}

// The `cxx_async` bridge that defines the future we want to return.
#[cxx_async::bridge]
unsafe impl Future for RustFutureVecU8 {
    type Output = Vec<u8>;
}

// Our REST endpoint, which calls the C++ coroutine.
#[post("/convert")]
async fn convert(jpeg_data: Bytes) -> impl Responder {
    ffi::reencode_jpeg_async(&jpeg_data).await.unwrap()
}

// The server entry point.
#[actix_web::main]
async fn main() -> IoResult<()> {
    println!("running on 127.0.0.1:8765");
    HttpServer::new(|| App::new().service(convert))
        .bind(("127.0.0.1", 8765))?
        .run()
        .await
}
