#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Editor {
    struct Entity {
        uint64_t id;
        std::string name;
        float location[3];
        float rotation[3];
        std::string action;
    };
    
    struct WorldCell {
        std::string name;
        bool is_interior = false;
        bool is_interior_lighting = false;
        std::vector<Entity*> entities;
        
        Entity* NewEntity();
        void DeleteEntity(size_t id);
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
    
    
    
    void Init();
    
}

#endif // EDITOR_H