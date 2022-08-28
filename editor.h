#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <serializeddata.h>

namespace Core {
    class RenderComponent;
}

namespace Editor {
    struct WorldCell;
    struct Entity {
        uint64_t id;
        std::string name;
        glm::vec3 location;
        glm::vec3 rotation;
        std::string action;
        WorldCell* parent;
        Core::SerializedEntityData* ent_data = nullptr;
        Core::RenderComponent* model = nullptr;
        void FromString (std::string_view& str);
        void ToString (std::string& str);
        void Show ();
        void Hide ();
        void ModelUpdate ();
    };
    
    struct WorldCell {
        std::string name;
        bool is_interior = false;
        bool is_interior_lighting = false;
        
        bool is_visible = false;
        std::vector<Entity*> entities;
        
        Entity* NewEntity();
        void DeleteEntity(size_t id);
        
        void Load();
        void Save();
    };
    
    struct WorldCellIndirector {
        enum {
            CELL_ITSELF,
            CELL_ENTITIES,
            CELL_TRANSITIONS,
            CELL_PATHS,
            CELL_NAVMESH
        } indirection_type;
        WorldCell* into;
        
        void Show();
        void Hide();
        void Begonis();
        void Delete();
    };
    
    WorldCell* NewWorldCell();
    
    extern std::vector<WorldCell*> worldCells;
    extern std::unordered_map<std::string, Core::SerializedEntityData* (*)(void)> entityDatas;
    //extern std::vector<std::pair<std::string, Core::SerializedEntityData* (*)(void)>> entityDatasSorted;
    
    
    
    void Init();
    
}

#endif // EDITOR_H