#ifndef TRAM_SDK_EDITOR_OBJECTS_NAVMESHMANAGER_H
#define TRAM_SDK_EDITOR_OBJECTS_NAVMESHMANAGER_H

#include <editor/objects/navmesh.h>

namespace Editor {

class NavmeshManager : public Object {
public:
    NavmeshManager(Object* parent) : Object(parent) {
        properties["name"] = std::string("navmeshes");
    }
    
    std::string_view GetName() { return "navmeshes"; }
    
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
            {"group-navmesh-manager", "navmesh_manager", "", PROPERTY_CATEGORY}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Navmesh>(this); children.push_back(child); return child; }
};


}

#endif // TRAM_SDK_EDITOR_OBJECTS_NAVMESHMANAGER_H