use anyhow::Result;
use serde::{Deserialize, Serialize};

/// Représente une entrée générique du registre.
#[derive(Debug, Serialize, Deserialize, PartialEq)]
pub struct Record {
    pub id: u32,
    pub notes: String,
}

/// Sauvegarde la structure `Record` au format JSON sur le disque.
pub fn sauvegarder(record: &Record, path: &str) -> Result<()> {
    let contenu = serde_json::to_string(record)?;
    std::fs::write(path, contenu)?;
    Ok(())
}

/// Charge une structure `Record` depuis un fichier JSON.
pub fn charger(path: &str) -> Result<Record> {
    let contenu = std::fs::read_to_string(path)?;
    let record = serde_json::from_str(&contenu)?;
    Ok(record)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn sauvegarde_et_chargement() {
        let r = Record { id: 1, notes: "ok".into() };
        let path = "/tmp/record.json";
        sauvegarder(&r, path).unwrap();
        let loaded = charger(path).unwrap();
        assert_eq!(loaded, r);
        std::fs::remove_file(path).unwrap();
    }
}
