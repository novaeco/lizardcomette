//! Accès au système de fichiers.

use anyhow::{anyhow, Result};
use esp_idf_svc::nvs::{EspDefaultNvsPartition, EspNvs};
use littlefs2::{
    driver::Storage,
    fs::{Allocation, Filesystem},
};
use serde::{Deserialize, Serialize};
use serde_json;

/// Sauvegarde des données sur le système de fichiers.
pub fn sauvegarder() {
    // TODO: écrire les données sur le disque
}

/// Monte un stockage LittleFS à l'aide de `littlefs2`.
///
/// Le stockage doit implémenter [`littlefs2::driver::Storage`]. L'option de
/// chiffrage est lue depuis la NVS sous la clé `fs_encrypt`.
pub fn monter_littlefs_chiffre<S: Storage>(
    storage: &mut S,
    alloc: &mut Allocation<S>,
) -> Result<Filesystem<S>> {
    if option_chiffrage()? {
        Filesystem::mount(alloc, storage).map_err(|e| anyhow::anyhow!("{:?}", e))
    } else {
        Filesystem::format(storage).map_err(|e| anyhow::anyhow!("{:?}", e))?;
        Filesystem::mount(alloc, storage).map_err(|e| anyhow::anyhow!("{:?}", e))
    }
}

/// Lit l'option de chiffrage dans la NVS.
pub fn option_chiffrage() -> Result<bool> {
    let part = EspDefaultNvsPartition::take()?;
    let nvs = EspNvs::new(part, "settings", true)?;
    Ok(nvs.get_u8("fs_encrypt")?.unwrap_or(0) != 0)
}

/// Modifie l'option de chiffrage dans la NVS.
pub fn definir_chiffrage(val: bool) -> Result<()> {
    let part = EspDefaultNvsPartition::take()?;
    let nvs = EspNvs::new(part, "settings", true)?;
    nvs.set_u8("fs_encrypt", if val { 1 } else { 0 })?;
    Ok(())
}

/// Exemple de lecture et écriture de JSON avec `serde`.
pub fn exemple_json(path: &str) -> Result<()> {
    #[derive(Serialize, Deserialize, PartialEq, Debug)]
    struct Exemple {
        nombre: u32,
    }

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
