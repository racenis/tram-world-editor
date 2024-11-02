#ifndef TRAM_SDK_EDITOR_OBJECTS_ENTITYGROUPMANAGER_H
#define TRAM_SDK_EDITOR_OBJECTS_ENTITYGROUPMANAGER_H

#include <editor/objects/entitygroup.h>

namespace Editor {

class EntityGroupManager : public Object {
public:
    EntityGroupManager(Object* parent) : Object(parent) {
        auto default_group = std::make_shared<EntityGroup>(this, "[default]");
        children.push_back(default_group);
        
        properties["name"] = std::string("entity-manager");
    }
    
    bool IsChildrenTreeable() { return true; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return false; }
    bool IsEditable() { return false; }
    bool IsCopyable() { return false; }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-entity-group-manager", "entity_group_manager", "", PROPERTY_CATEGORY}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<EntityGroup>(this); children.push_back(child); return child; }
};


}

#endif // TRAM_SDK_EDITOR_OBJECTS_ENTITYGROUPMANAGER_H