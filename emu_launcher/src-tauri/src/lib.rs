use std::process::Command;
use tauri::Manager;

#[tauri::command]
fn greet(name: &str) -> String {
    format!("Hello, {}! You've been greeted from Rust!", name)
}

#[tauri::command]
fn launch_emulator(rom_path: String, app: tauri::AppHandle) -> Result<(), String> {
    let resource_dir = app.path().resource_dir().map_err(|e| e.to_string())?;

    let release_path = resource_dir.join("../MacOS/GameBoyCpp");
    let dev_path = resource_dir.join("GameBoyCpp");

    let binary_path = if release_path.exists() {
        release_path
    } else {
        dev_path
    };

    Command::new(&binary_path)
        .arg(&rom_path)
        .spawn()
        .map_err(|e| e.to_string())?;
    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_shell::init())
        .plugin(tauri_plugin_dialog::init())
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![greet, launch_emulator])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
