//! Communication réseau.

pub mod api;
pub mod http;
pub mod mqtt;
pub mod wifi;
pub mod ota;

#[cfg(test)]
mod tests {
    #[test]
    fn placeholder() {}
}
