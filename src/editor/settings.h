#ifndef TRAM_SDK_EDITOR_SETTINGS_H
#define TRAM_SDK_EDITOR_SETTINGS_H

namespace Editor::Settings {

enum Space {
    SPACE_WORLD,
    SPACE_ENTITY,
    SPACE_ENTITYGROUP
};

enum Rotation {
    ROTATION_RADIANS,
    ROTATION_DEGREES
};

extern Space TRANSFORM_SPACE;
extern Rotation ROTATION_UNIT;

extern bool CELL_LIST_FROM_FILESYSTEM;

enum Language {
    LANGUAGE_LV,
    LANGUAGE_EN
};

extern Language INTERFACE_LANGUAGE;

void Save();
void Load();
    
}

#endif // TRAM_SDK_EDITOR_SETTINGS_H