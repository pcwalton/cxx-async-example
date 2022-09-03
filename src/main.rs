// cxx-async-example/src/main.rs
//
//! A minimal practical example of `cxx-async` usage.

use actix_web::web::Bytes;
use actix_web::{post, App, HttpServer, Responder};
use futures::channel::oneshot::{self, Sender};
use std::io::Result as IoResult;

// The `cxx` bridge that declares the asynchronous function we want to call.
#[cxx::bridge]
mod ffi {
    extern "Rust" {
        type CallbackContext;
    }

    unsafe extern "C++" {
        include!("coroutine_example.h");
        fn reencode_jpeg_async(
            jpeg_data: &[u8],
            callback: fn(Box<CallbackContext>, result: Vec<u8>),
            context: Box<CallbackContext>,
        );
    }
}

pub struct CallbackContext(Sender<Vec<u8>>);

// Our REST endpoint, which calls the asynchronous C++ function.
#[post("/convert")]
async fn convert(jpeg_data: Bytes) -> impl Responder {
    let (sender, receiver) = oneshot::channel();
    let context = Box::new(CallbackContext(sender));
    ffi::reencode_jpeg_async(
        &jpeg_data,
        |context, encoded| drop(context.0.send(encoded)),
        context,
    );
    receiver.await.unwrap()
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
