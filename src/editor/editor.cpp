#include <editor/editor.h>
#include <editor/language.h>
#include <editor/settings.h>

#include <editor/objects/world.h>
#include <editor/objects/worldcellmanager.h>

#include <framework/file.h>

#include <iostream>
#include <fstream>
#include <filesystem>

namespace Editor {

using namespace tram;

std::shared_ptr<Selection> SELECTION;
std::shared_ptr<Selection> STASH;
std::list<std::unique_ptr<Action>> PERFORMED_ACTIONS;
std::list<std::unique_ptr<Action>> UNPERFORMED_ACTIONS;
bool data_modified = false;

std::unordered_map<std::string, std::vector<std::string>> PROPERTY_ENUMERATIONS = {
    {"entity-type", {"[none]"}}
};

void Reset() {
    if (Settings::CELL_LIST_FROM_FILESYSTEM || WORLD == nullptr) {
        WORLD = std::make_shared<World>(nullptr);
    }
    
    SELECTION = std::make_shared<Selection>();
    
    Editor::UNPERFORMED_ACTIONS.clear();
    Editor::UNPERFORMED_ACTIONS.clear();
    Editor::data_modified = false;
    
    PathManager* path_manager = WORLD->path_manager.get();
    NavmeshManager* navmesh_manager = WORLD->navmesh_manager.get();
    WorldCellManager* worldcell_manager = WORLD->worldcell_manager.get();
    
    if (!Settings::CELL_LIST_FROM_FILESYSTEM) {
        return;
    }
    
    if (std::filesystem::exists("data/worldcells/")) {
        auto cell_dir = std::filesystem::directory_iterator("data/worldcells/");
        for (auto& cell : cell_dir) {
            if (cell.is_regular_file() && cell.path().extension() == ".cell") {
                auto cell_name = cell.path().stem().string();
                auto worldcell = std::make_shared<WorldCell>(worldcell_manager, cell_name);
                ((Object*) worldcell_manager)->AddChild(worldcell);
            }
        }
    }
        
    if (std::filesystem::exists("data/paths/")) {
        auto path_dir = std::filesystem::directory_iterator("data/paths/");
        for (auto& path_file : path_dir) {
            if (path_file.is_regular_file() && path_file.path().extension() == ".path") {
                auto path_name = path_file.path().stem().string();
                auto path_object = std::make_shared<Path>(path_manager, path_name);
                ((Object*) path_manager)->AddChild(path_object);
            }
        }
    }
    
    if (std::filesystem::exists("data/navmeshes/")) {
        auto navmesh_dir = std::filesystem::directory_iterator("data/navmeshes/");
        for (auto& navmesh_file : navmesh_dir) {
            if (navmesh_file.is_regular_file() && navmesh_file.path().extension() == ".navmesh") {
                auto navmesh_name = navmesh_file.path().stem().string();
                auto navmesh_object = std::make_shared<Path>(navmesh_manager, navmesh_name);
                ((Object*) navmesh_manager)->AddChild(navmesh_object);
            }
        }
    }
}

PropertyDefinition ReadEntityField(File& file) {
    name_t type = file.read_name();
    name_t name = file.read_name();
    
    // TODO: add checks for forbidden field names, e.g. rotation-x or name or whatever
    
    if (type == "string") {
        return {name, name, "group-entity-specific", PROPERTY_STRING};
    } else if (type == "int") {
        return {name, name, "group-entity-specific", PROPERTY_INT};
    } else if (type == "uint") {
        return {name, name, "group-entity-specific", PROPERTY_UINT};
    } else if (type == "float") {
        return {name, name, "group-entity-specific", PROPERTY_FLOAT};
    } else if (type == "vector") {
        return {name, name, "group-entity-specific", PROPERTY_VECTOR};
    } else if (type == "origin") {
        return {name, name, "group-entity-specific", PROPERTY_ORIGIN};
    } else if (type == "direction") {
        return {name, name, "group-entity-specific", PROPERTY_DIRECTION};
    }

    // in the future some possible other values could be:
    // - angles
    // - flags

    std::cout << "unrezgonized entry type: " << type << std::endl;
    
    return {};
}

PropertyValue ReadDefaultValue(File& file, PropertyType type) {
    
    switch (type) {
        case PROPERTY_STRING:
            return PropertyValue::String(std::string(file.read_string()));
        case PROPERTY_INT:
            return PropertyValue::Int(file.read_int32());
        case PROPERTY_UINT:
            return PropertyValue::UInt(file.read_uint32());
        case PROPERTY_FLOAT:
            return PropertyValue::Float(file.read_float32());
        case PROPERTY_ORIGIN:
        case PROPERTY_DIRECTION:
        case PROPERTY_VECTOR:
            return PropertyValue::Vector({file.read_float32(), file.read_float32(), file.read_float32()});
        default:
            break;
        
    }
    
    std::cout << "cannot read default value for entdef: " << PROPERTY_STRING << std::endl;
    
    return {};
}

WidgetDefinition ReadEntityWidget(File& file) {
    auto color = file.read_name();
    auto type = file.read_name();
    auto field = file.read_name();
    
    WidgetDefinition::Color widget_color;
    WidgetDefinition::Type widget_type;
    
    if (type == "cyan") {
        widget_color = WidgetDefinition::WIDGET_CYAN;
    } else {
        widget_color = WidgetDefinition::WIDGET_CYAN;
    }
    
    if (type == "point") {
        widget_type = WidgetDefinition::WIDGET_POINT;
    } else if (type == "normal") {
        widget_type = WidgetDefinition::WIDGET_NORMAL;
    } else {
        widget_type = WidgetDefinition::WIDGET_NORMAL;
    }
    
    return {widget_color, widget_type, field};
}

void ReadEntityDefinition(File& file) {
    name_t entity_name;
    name_t model_name;
    uint32_t entity_version = 0;
    
    std::vector<PropertyDefinition> property_defs;
    std::vector<WidgetDefinition> widget_defs;
    std::vector<std::pair<std::string, PropertyValue>> default_properties;
    
    while (file.is_continue()) {
        name_t entry_type = file.read_name();
        
        if (entry_type == "name") {
            entity_name = file.read_name();
        } else if (entry_type == "version") {
            entity_version = file.read_uint32();
        } else if (entry_type == "model") {
            model_name = file.read_name();
        } else if (entry_type == "field") {
            auto definition = ReadEntityField(file);
            property_defs.push_back(definition);
            default_properties.push_back({definition.name, ReadDefaultValue(file, definition.type)});
        } else if (entry_type == "gizmo") {
            widget_defs.push_back(ReadEntityWidget(file));
        } else if (entry_type == "end") {
            break;
        } else {
            std::cout << "Unrecognized entdef entry: " << entry_type << std::endl;
        }
        
    }
    
    // TODO: check for dupes
    // TODO: check if not using builitin properrtyy names
    
    if (!entity_name) {
        std::cout << "Entity type is not set!" << std::endl;
        return;
    }
    
    EntityDefinition definition;
    
    definition.version = entity_version;
    
    definition.name = (const char*)entity_name;
    definition.model_name = (const char*)model_name;
    
    definition.definitions = property_defs;
    definition.widgets = widget_defs;
    definition.default_properties = default_properties;
    
    RegisterEntityType(definition);
}

void Init() {
    for (const auto &entry : std::filesystem::recursive_directory_iterator("data/")) {
        if (entry.path().extension() != ".entdef") continue;
        
        auto path = entry.path().string();
        
        //File file("data/entities.entdef", File::READ);
        File file(path.c_str(), File::READ);
    
        if (!file.is_open()) {
            std::cout << "Entity file defintiion did not open!" << std::endl;
        }
        
        while (file.is_continue()) {
            name_t record_type = file.read_name();
            
            if (record_type == "begin") {
                ReadEntityDefinition(file);
            } else {
                std::cout << "Unrezognized record: " << record_type << std::endl;
                return;
            }
            
        }
        
    }
}

}