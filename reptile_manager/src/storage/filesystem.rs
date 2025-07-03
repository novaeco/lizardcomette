//! Accès au système de fichiers.

use anyhow::Result;
use esp_idf_svc::fs::littlefs::Littlefs;
use esp_idf_svc::io::MountedLittlefs;
use serde::{Deserialize, Serialize};
use serde_json;

/// Sauvegarde des données sur le système de fichiers.
pub fn sauvegarder() {
    // TODO: écrire les données sur le disque
}

/// Monte une partition LittleFS chiffrée.
pub fn monter_littlefs_chiffre(label: &str, point_de_montage: &str) -> Result<MountedLittlefs<Littlefs<'static>>> {
    let littlefs = unsafe { Littlefs::new_partition(label)? };
    let fs = MountedLittlefs::mount(littlefs, point_de_montage)?;
    Ok(fs)
}

/// Exemple de lecture et écriture de JSON avec `serde`.
pub fn exemple_json(path: &str) -> Result<()> {
    #[derive(Serialize, Deserialize, PartialEq, Debug)]
    struct Exemple { nombre: u32 }

    let donnees = Exemple { nombre: 42 };
    let contenu = serde_json::to_string(&donnees)?;
    std::fs::write(path, &contenu)?;

    let lu = std::fs::read_to_string(path)?;
    let recupere: Exemple = serde_json::from_str(&lu)?;
    assert_eq!(recupere, donnees);
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn exemple_json_roundtrip() {
        let path = "/tmp/test.json";
        exemple_json(path).unwrap();
    }
}
