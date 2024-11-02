#ifndef TRAM_SDK_EDITOR_OBJECTS_ENTITYGROUP_H
#define TRAM_SDK_EDITOR_OBJECTS_ENTITYGROUP_H

#include <editor/objects/entity.h>

namespace Editor {

class EntityGroup : public Object {
public:
    EntityGroup(Object* parent) : EntityGroup(parent, std::string("new-entity-group")) {}
    EntityGroup(Object* parent, std::string name) : Object(parent) {
        properties["name"] = name;
    }
    
    bool IsChildrenTreeable() { return false; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return true; }
    bool IsRemovable() { return properties["name"].str_value != "[default]"; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Entity>(this); children.push_back(child); return child; }

    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "name", "", PROPERTY_STRING},
            {"position-x", "x", "", PROPERTY_FLOAT},
            {"position-y", "y", "", PROPERTY_FLOAT},
            {"position-z", "z", "", PROPERTY_FLOAT}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-entity-group", "entity_group", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-entity-group", PROPERTY_STRING}
        };
    }
};


}

#endif // TRAM_SDK_EDITOR_OBJECTS_ENTITYGROUP_H