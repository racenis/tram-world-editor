#ifndef TRAM_SDK_EDITOR_OBJECTS_PATHMANAGER_H
#define TRAM_SDK_EDITOR_OBJECTS_PATHMANAGER_H

#include <editor/objects/path.h>

namespace Editor {

class PathManager : public Object {
public:
    PathManager(Object* parent) : Object(parent) {
        properties["name"] = std::string("Paths");
    }
    
    std::string_view GetName() { return "Paths"; }
    
    bool IsChildrenTreeable() { return true; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return false; }
    bool IsCopyable() { return false; }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-path-manager", "path_manager", "", PROPERTY_CATEGORY}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Path>(this); children.push_back(child); return child; }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_PATHMANAGER_H