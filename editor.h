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
    struct EntityGroup;
    struct Entity {
        uint64_t id;
        std::string name;
        glm::vec3 location;
        glm::vec3 rotation;
        std::string action;
        WorldCell* parent;
        EntityGroup* group = nullptr;
        Core::SerializedEntityData* ent_data = nullptr;
        Core::RenderComponent* model = nullptr;
        void FromString (std::string_view& str);
        void ToString (std::string& str);
        void Show ();
        void Hide ();
        void ModelUpdate ();
    };
    
    struct Transition {
        struct Point {
            glm::vec3 location;
        };
        
        Point* NewPoint();
        void DeletePoint(Point* point);
        
        std::string name;
        WorldCell* parent;
        std::vector<Point*> points;
        Core::WorldCell::Transition* v_transition = nullptr;
    };
    
    struct EntityGroup {
        std::vector<Entity*> entities;
        WorldCell* parent;
        
        Entity* NewEntity();
        void DeleteEntity(Entity* entity);
    };
    
    struct Path {
        struct Curve {
            glm::vec3 location1;
            glm::vec3 location2;
            glm::vec3 location3;
            glm::vec3 location4;
        };
        
        Curve* NewCurve();
        void DeleteCurve(Curve* curve);
        
        std::string name;
        WorldCell* parent;
        std::vector<Curve*> curves;
        //Core::WorldCell::Transition* v_transition = nullptr;
    };
    
    struct Navmesh {
        struct Node {
            glm::vec3 location;
        };
        
        Node* NewNode();
        void DeleteNode(Node* node);
        
        std::string name;
        WorldCell* parent;
        std::vector<Node*> nodes;
    };
    
    struct WorldCell {
        std::string name;
        bool is_interior = false;
        bool is_interior_lighting = false;
        
        bool is_visible = false;
        std::vector<Entity*> entities;
        std::vector<Transition*> transitions;
        std::vector<EntityGroup*> groups;
        std::vector<Path*> paths;
        std::vector<Navmesh*> navmeshes;
        
        Entity* NewEntity();
        void DeleteEntity(size_t id);
        Transition* NewTransition();
        void DeleteTransition(Transition* transition);
        EntityGroup* NewEntityGroup();
        void DeleteEntityGroup(EntityGroup* group);
        Path* NewPath();
        void DeletePath(Path* path);
        Navmesh* NewNavmesh();
        void DeleteNavmesh(Navmesh* navmesh);
        
        void Load();
        void Save();
    };
    
    struct WorldCellIndirector {
        enum {
            CELL_ITSELF,
            CELL_ENTITIES,
            CELL_TRANSITIONS,
            CELL_PATHS,
            CELL_NAVMESH,
            TRANSITION,
            ENTITY_GROUP,
            PATH,
            NAVMESH
        } indirection_type;
        WorldCell* into = nullptr;
        Transition* trans = nullptr;
        EntityGroup* group = nullptr;
        Path* path = nullptr;
        Navmesh* navmesh = nullptr;
        
        bool IsVisible();
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