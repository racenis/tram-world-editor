#include <editor.h>
#include <iostream>

namespace Editor {
    std::vector<WorldCell*> worldCells;
    
    void WorldCellIndirector::Show() {
        std::cout << "SHOWING CELL: " << into->name << std::endl;
    }
    
    void WorldCellIndirector::Hide() {
        std::cout << "HIDING CELL: " << into->name << std::endl;
    }
    
    void WorldCellIndirector::Begonis() {
        std::cout << "BEGONISING CELL: " << into->name << std::endl;
    }
    
    void WorldCellIndirector::Delete() {
        std::cout << "YEETING CELL: " << into->name << std::endl;
        for (auto it = worldCells.begin(); it < worldCells.end(); it++) {
            if (*it == into) {
                worldCells.erase(it);
                return;
            }
        }
    }
    
    WorldCell* NewWorldCell() {
        auto cell = new WorldCell{"jauns"};
        worldCells.push_back(cell);
        return cell;
    }
    
    Entity* WorldCell::NewEntity() {
        auto ent = new Entity {0, "Entītija"};
        entities.push_back(ent);
        return ent;
    }
    
    void WorldCell::DeleteEntity(size_t id) {
        entities.erase(entities.begin() + id);
    }
    
    
    
    void Init(){
        auto cell1 = new WorldCell{"chongon"};
        auto cell2 = new WorldCell{"benigon"};
        auto cell3 = new WorldCell{"dongon"};
        auto cell4 = new WorldCell{"dingodongo"};
        
        worldCells.push_back(cell1);
        worldCells.push_back(cell2);
        worldCells.push_back(cell3);
        worldCells.push_back(cell4);
        
        cell1->entities.push_back(new Entity{0, "benis"});
        cell1->entities.push_back(new Entity{4, "impressive"});
        cell1->entities.push_back(new Entity{5, "nice"});
        
        cell2->entities.push_back(new Entity{1, "benisoner"});
    }
}