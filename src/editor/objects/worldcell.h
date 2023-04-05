#ifndef TRAM_SDK_EDITOR_OBJECTS_WORLDCELL_H
#define TRAM_SDK_EDITOR_OBJECTS_WORLDCELL_H

#include <editor/objects/pathmanager.h>
#include <editor/objects/navmeshmanager.h>
#include <editor/objects/transitionmanager.h>
#include <editor/objects/entitygroupmanager.h>

namespace Editor {

class WorldCell : public Object {
public:
    WorldCell(Object* parent) : WorldCell(parent, "New Worldcell") {}
    WorldCell(Object* parent, std::string name) : Object(parent), 
        group_manager(std::make_shared<EntityGroupManager>(this)), 
        transition_manager(std::make_shared<TransitionManager>(this)),
        path_manager(std::make_shared<PathManager>(this)),
        navmesh_manager(std::make_shared<NavmeshManager>(this)) {
        properties["name"] = name;
        properties["is-interior"] = false;
        properties["is-interior-lighting"] = false;
        
        children.push_back(group_manager);
        children.push_back(transition_manager);
        children.push_back(path_manager);
        children.push_back(navmesh_manager);
    }
    
    bool IsChildrenTreeable() { return true; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return false; }
    bool IsRemovable() { return true; }
    bool IsEditable() { return true; }
    bool IsCopyable() { return true; }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "Name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-worldcell", "Worldcell", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-worldcell", PROPERTY_STRING},
            {"is-interior", "Interior", "group-worldcell", PROPERTY_BOOL},
            {"is-interior-lighting", "Interior Lighting", "group-worldcell", PROPERTY_BOOL}
        };
    }
    
    std::shared_ptr<EntityGroupManager> group_manager;
    std::shared_ptr<TransitionManager> transition_manager;
    std::shared_ptr<PathManager> path_manager;
    std::shared_ptr<NavmeshManager> navmesh_manager;
};

}

#endif // TRAM_SDK_EDITOR_OBJECTS_WORLDCELL_H