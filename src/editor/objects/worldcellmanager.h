#ifndef TRAM_SDK_EDITOR_OBJECTS_WORLDCELLMANAGER_H
#define TRAM_SDK_EDITOR_OBJECTS_WORLDCELLMANAGER_H

#include <editor/editor.h>

#include <editor/objects/worldcell.h>

namespace Editor {

class WorldCellManager : public Object {
public:
    WorldCellManager (Object* parent) : Object (parent) {
        properties["name"] = std::string("worldcells");
    }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-world-cell-manager", "worldcell_manager", "", PROPERTY_CATEGORY}
        };
    }
    
    bool IsChildrenTreeable() { return true; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return false; }
    bool IsEditable() { return false; }
    bool IsCopyable() { return false; }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<WorldCell>(this); children.push_back(child); return child; }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_WORLDCELLMANAGER_H