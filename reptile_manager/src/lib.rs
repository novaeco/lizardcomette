#![cfg_attr(not(test), no_std)]

pub mod config;
pub mod domain;
pub mod hardware;
pub mod network;
pub mod storage;
pub mod tasks;
pub mod ui;
pub mod utils;

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
