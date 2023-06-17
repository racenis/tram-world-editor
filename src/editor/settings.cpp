#include <editor/settings.h>

#include <editor/editor.h>
#include <editor/language.h>

#include <framework/logging.h>

#include <unordered_map>
#include <fstream>

namespace Editor::Settings {

Space TRANSFORM_SPACE = SPACE_WORLD;
Rotation ROTATION_UNIT = ROTATION_RADIANS;
bool CELL_LIST_FROM_FILESYSTEM = true;
Language INTERFACE_LANGUAGE = LANGUAGE_EN;
    
/// Definitions of all Settings.
/// They're only used for the Settings::Load() and Settings::Save() functions.
static std::unordered_map<std::string, std::pair<PropertyType, void*>> setting_map = {
    {"TRANSFORM_SPACE", {PROPERTY_ENUM, &Settings::TRANSFORM_SPACE}},
    {"ROTATION_UNIT", {PROPERTY_ENUM, &Settings::ROTATION_UNIT}},
    {"CELL_LIST_FROM_FILESYSTEM", {PROPERTY_BOOL, &Settings::CELL_LIST_FROM_FILESYSTEM}},
    {"INTERFACE_LANGUAGE", {PROPERTY_ENUM, &Settings::INTERFACE_LANGUAGE}}
};

using namespace tram;

/// Reads the Settings from disk.
void Load() {
    SetSystemLoggingSeverity(System::SYSTEM_PLATFORM, SEVERITY_WARNING);
    
    std::ifstream file ("data/editor_settings.ini");
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t eq_pos = line.find('=');
            if (eq_pos == std::string::npos) continue;
            
            std::string key = line.substr(0, eq_pos);
            std::string value = line.substr(++eq_pos);
            
            auto& bepis = setting_map[std::string(key)];
            if (!bepis.second) continue;
            switch (bepis.first) {
                case PROPERTY_ENUM:
                    *(uint32_t*) bepis.second = std::stoull(value);
                    break;
                case PROPERTY_BOOL:
                    *(bool*) bepis.second = std::stoull(value);;
                    break;
                default:
                    break;
            }
        }
    } else {
        std::wcout << selected_language->dialog_settings_not_found << std::endl;
    }
}

/// Writes the Settings to disk.
void Save() {
    std::ofstream file ("data/editor_settings.ini");
    
    if (file.is_open()) {
        for (auto& entry : setting_map) {
            file << entry.first << "=";
            switch (entry.second.first) {
                case PROPERTY_ENUM:
                    file << *(uint32_t*) entry.second.second;
                break;
                case PROPERTY_BOOL:
                    file << *(bool*) entry.second.second;
                break;
                default:
                    file << "0";
            }
            file << std::endl;
        }
    }
}

}
