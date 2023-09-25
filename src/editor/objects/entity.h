#ifndef TRAM_SDK_EDITOR_OBJECTS_ENTITY_H
#define TRAM_SDK_EDITOR_OBJECTS_ENTITY_H

#include <editor/editor.h>

namespace tram {
    class RenderComponent;
}

namespace Editor {

class Entity : public Object {
public:
    Entity(Object* parent) : Entity(parent, "New Entity") {}
    Entity(Object* parent, std::string name) : Object(parent) {
        properties["name"] = name;
        properties["action"] = std::string("none");
        properties["position-x"] = 0.0f;
        properties["position-y"] = 0.0f;
        properties["position-z"] = 0.0f;
        properties["rotation-x"] = 0.0f;
        properties["rotation-y"] = 0.0f;
        properties["rotation-z"] = 0.0f;
        properties["entity-type"] = (int32_t) 0;
    }

    bool IsChildrenTreeable() { return false; }
    bool IsChildrenListable() { return false; }
    bool IsAddable() { return false; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    bool IsHidden() { return is_hidden; }
    
    void SetHidden(bool is_hidden);
    
    std::shared_ptr<Object> Duplicate();
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions();
    std::vector<PropertyDefinition> GetSerializationPropertyDefinitions();
    
    void SetEntityType (std::string type);
    
    PropertyValue GetProperty (std::string property_name);
    
    void SetProperty (std::string property_name, PropertyValue property_value);
    
    void CheckModel();
    tram::RenderComponent* model = nullptr;
};

Entity* GetEntityFromViewmodel(tram::RenderComponent* model);

}

#endif // TRAM_SDK_EDITOR_OBJECTS_ENTITY_H