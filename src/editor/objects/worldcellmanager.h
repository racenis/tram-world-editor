#ifndef TRAM_SDK_EDITOR_OBJECTS_WORLDCELLMANAGER_H
#define TRAM_SDK_EDITOR_OBJECTS_WORLDCELLMANAGER_H

#include <editor/editor.h>

#include <editor/objects/worldcell.h>

namespace Editor {

class WorldCellManager : public Object {
public:
    WorldCellManager(Object* parent) {
        properties["name"] = std::string("World");
    }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "Name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-world-cell-manager", "Worldcell Manager", "", PROPERTY_CATEGORY}
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

extern std::shared_ptr<WorldCellManager> WORLDCELLS;

}

#endif // TRAM_SDK_EDITOR_OBJECTS_WORLDCELLMANAGER_H