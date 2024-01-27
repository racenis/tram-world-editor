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

enum Snap {
    SNAP_0_01,
    SNAP_0_10,
    SNAP_0_25,
    SNAP_0_50,
    SNAP_1_00,
    SNAP_15,
    SNAP_30,
    SNAP_45,
    SNAP_90
};

extern Space TRANSFORM_SPACE;
extern Rotation ROTATION_UNIT;
extern Snap TRANSLATION_SNAP;
extern Snap ROTATION_SNAP;

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