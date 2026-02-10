#ifndef TRAM_SDK_EDITOR_OBJECTS_ENTITY_H
#define TRAM_SDK_EDITOR_OBJECTS_ENTITY_H

#include <editor/editor.h>

namespace tram {
    class RenderComponent;
}

namespace Editor {

struct Signal {
    std::string type;
    std::string target;
    float delay;
    int limit;
    std::string message;
    std::string param_type;
    std::string param;
};
    
    
class Entity : public Object {
public:
    Entity(Object* parent) : Entity(parent, "none") {}
    Entity(Object* parent, std::string name) : Object(parent) {
        properties["name"] = name;
        properties["action"] = std::string("none");
        properties["entity-flags"] = PropertyValue::Flag(0);
        properties["position-x"] = Viewport::CURSOR_X;
        properties["position-y"] = Viewport::CURSOR_Y;
        properties["position-z"] = Viewport::CURSOR_Z;
        properties["rotation-x"] = 0.0f;
        properties["rotation-y"] = 0.0f;
        properties["rotation-z"] = 0.0f;
        properties["entity-type"] = (int32_t) 0;
        GenerateNewRandomId();
    }

    bool IsChildrenTreeable() { return false; }
    bool IsChildrenListable() { return false; }
    bool IsAddable() { return false; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    bool IsHidden() { return is_hidden; }
    
    void SetHidden(bool is_hidden);
    
    void GenerateNewRandomId();
    
    std::shared_ptr<Object> Duplicate();
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions();
    std::vector<PropertyDefinition> GetSerializationPropertyDefinitions();
    std::vector<PropertyDefinition> GetSerializationPropertyDefinitions(uint32_t);
    
    void SetEntityType (std::string type);
    void InitDefaultPropertyValues();
    
    std::vector<WidgetDefinition> GetWidgetDefinitions();
    
    PropertyValue GetProperty (std::string property_name);
    
    void SetProperty (std::string property_name, PropertyValue property_value);
    
    void Draw();
    void CheckModel();
    
    void CenterOrigin();
    void WorldspawnOffset(Entity* worldspawn);
    
    std::vector<Signal> signals;
    tram::RenderComponent* model = nullptr;
};

Entity* GetEntityFromViewmodel(tram::RenderComponent* model);

uint32_t GetEntityTypeVersion(int32_t type);

}

#endif // TRAM_SDK_EDITOR_OBJECTS_ENTITY_H