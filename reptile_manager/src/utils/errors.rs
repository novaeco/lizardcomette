pub use anyhow::{Error, Result};

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn result_type_alias_works() -> Result<()> {
        let v: Result<u8> = Ok(42);
        assert_eq!(v?, 42);
        Ok(())
    }
}
