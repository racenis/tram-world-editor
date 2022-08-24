#ifndef EDITOR_H
#define EDITOR_H

#include <string>
#include <vector>
#include <unordered_map>

namespace Editor {
    struct WorldCell {
        std::string name;
        bool is_interior = false;
        bool is_interior_lighting = false;
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