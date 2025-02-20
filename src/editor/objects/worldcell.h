#ifndef TRAM_SDK_EDITOR_OBJECTS_WORLDCELL_H
#define TRAM_SDK_EDITOR_OBJECTS_WORLDCELL_H

#include <editor/objects/pathmanager.h>
#include <editor/objects/navmeshmanager.h>
#include <editor/objects/transitionmanager.h>
#include <editor/objects/entitygroupmanager.h>
#include <editor/objects/lightgraph.h>
#include <editor/objects/soundgraph.h>

namespace Editor {

class WorldCell : public Object {
public:
    WorldCell(Object* parent) : WorldCell(parent, "new-worldcell") {}
    WorldCell(Object* parent, std::string name) : Object(parent), 
        group_manager(std::make_shared<EntityGroupManager>(this)), 
        transition_manager(std::make_shared<TransitionManager>(this)),
        light_graph(std::make_shared<LightGraph>(this)),
        sound_graph(std::make_shared<SoundGraph>(this))
    {
        properties["name"] = name;
        properties["is-interior"] = false;
        properties["is-interior-lighting"] = false;
        
        children.push_back(group_manager);
        children.push_back(transition_manager);
        children.push_back(light_graph);
        children.push_back(sound_graph);
    }
    
    bool IsChildrenTreeable() { return true; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return false; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    
    void LoadFromDisk();
    void SaveToDisk();
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-worldcell", "Worldcell", "", PROPERTY_CATEGORY},
            {"name", "name", "group-worldcell", PROPERTY_STRING},
            {"is-interior", "interior", "group-worldcell", PROPERTY_BOOL},
            {"is-interior-lighting", "interior_lighting", "group-worldcell", PROPERTY_BOOL}
        };
    }
    
    std::shared_ptr<EntityGroupManager> group_manager;
    std::shared_ptr<TransitionManager> transition_manager;
    std::shared_ptr<LightGraph> light_graph;
    std::shared_ptr<SoundGraph> sound_graph;
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_WORLDCELL_H