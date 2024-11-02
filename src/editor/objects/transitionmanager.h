#ifndef TRAM_SDK_EDITOR_OBJECTS_TRANSITIONMANAGER_H
#define TRAM_SDK_EDITOR_OBJECTS_TRANSITIONMANAGER_H

#include <editor/objects/transition.h>

namespace Editor {

class TransitionManager : public Object {
public:
    TransitionManager(Object* parent) : Object(parent) {
        properties["name"] = std::string("transitions");
    }
    
    std::string_view GetName() { return "transitions"; }
    
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
            {"group-transition-manager", "transition_manager", "", PROPERTY_CATEGORY}
        };
    }
    
    std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Transition>(this); children.push_back(child); return child; }
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_TRANSITIONMANAGER_H