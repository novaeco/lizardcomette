use std::env;

fn main() {
    println!("cargo:rerun-if-changed=partitions.csv");
    env::set_var("ESP_IDF_PARTITION_TABLE_CSV", "partitions.csv");
    esp_idf_sys::build();
}
