#ifndef TRAM_SDK_EDITOR_OBJECTS_WORLD_H
#define TRAM_SDK_EDITOR_OBJECTS_WORLD_H

#include <editor/editor.h>

#include <editor/objects/pathmanager.h>
#include <editor/objects/navmeshmanager.h>
#include <editor/objects/worldcellmanager.h>

namespace Editor {

/// Overobject that contains all of the Objects.
class World : public Object {
public:
    World (Object* parent) : 
        path_manager (std::make_shared<PathManager>(this)),
        navmesh_manager (std::make_shared<NavmeshManager>(this)),
        worldcell_manager (std::make_shared<WorldCellManager>(this))
    {
        properties["name"] = std::string("world");
        
        children.push_back(worldcell_manager);
        children.push_back(path_manager);
        children.push_back(navmesh_manager);
    }
    
    std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"name", "name", "", PROPERTY_STRING}
        };
    }
    
    std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
        return std::vector<PropertyDefinition> {
            {"group-world", "world", "", PROPERTY_CATEGORY}
        };
    }
    
    bool IsChildrenTreeable() { return true; }
    bool IsChildrenListable() { return true; }
    bool IsAddable() { return false; }
    bool IsRemovable() { return false; }
    bool IsEditable() { return false; }
    bool IsCopyable() { return false; }
    
    std::shared_ptr<PathManager> path_manager;
    std::shared_ptr<NavmeshManager> navmesh_manager;
    std::shared_ptr<WorldCellManager> worldcell_manager;
};

extern std::shared_ptr<World> WORLD;

}

#endif // TRAM_SDK_EDITOR_OBJECTS_WORLD_H