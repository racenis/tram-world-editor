#ifndef EDITOR_H
#define EDITOR_H

#include <list>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <serializeddata.h>

namespace Core {
    class RenderComponent;
}

namespace Editor {
    typedef void* worldTreeID_t;
    void Init();
    void Yeet();
    void ProduceTestData();

    class Action {
    public:
        virtual void Perform() = 0;
        virtual void Unperform() = 0;
    };
    
    extern std::list<std::unique_ptr<Action>> performed_actions;
    
    template <typename action, typename ...Args> void PerformAction (Args && ...args) {
        // TODO: check if performed_actions list isn't too full!
        performed_actions.push_back(std::make_unique<action>(std::forward<Args>(args)...));
    }
    
    class Object : public std::enable_shared_from_this<Object> {
    public:
        Object() = default;
        Object(Object* parent) : parent(parent) {};
        virtual ~Object() {}
        
        std::shared_ptr<Object> GetPointer() { return shared_from_this(); }
    
        bool hidden = true;
        Object* parent = nullptr;
        
        virtual std::list<std::shared_ptr<Object>> GetChildren() { std::cout << "GetChildren() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual std::string_view GetName() { std::cout << "GetName() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsChildrenListable() { std::cout << "IsChildrenListable() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual void SetHidden(bool is_hidden) { std::cout << "SetHidden() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual std::shared_ptr<Object> AddChild() { std::cout << "AddChild(void) not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual void AddChild(std::shared_ptr<Object>) { std::cout << "AddChild(std::shared_ptr) not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual void RemoveChild(std::shared_ptr<Object>) { std::cout << "RemoveChild() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
    };
    
    class Selection {
    public:
        std::list<std::shared_ptr<Object>> objects;
    };
    
    extern std::shared_ptr<Selection> selection;
}

namespace Editor {
    class Entity : public Object {
    public:
        Entity(Object* parent) : Object(parent), name("untitled entity") {}
        Entity(Object* parent, std::string name) : Object(parent), name(name) {}
    
        std::string_view GetName() { return name; }
        bool IsChildrenListable() { return false; }
        
        std::string name;
    };
    
    class EntityGroup : public Object {
    public:
        EntityGroup(Object* parent) : Object(parent), name("untitled entitygroup") {}
        EntityGroup(Object* parent, std::string name) : Object(parent), name(name) {}
        std::string name;
        std::list<std::shared_ptr<Entity>> entities;
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>>(); }
        std::string_view GetName() { return name; }
        bool IsChildrenListable() { return false; }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Entity>(this); entities.push_back(child); return child; }
        void AddChild(std::shared_ptr<Object> child) { entities.push_back(std::dynamic_pointer_cast<Entity>(child)); }
        void RemoveChild(std::shared_ptr<Object> child) { entities.remove(std::dynamic_pointer_cast<Entity>(child)); }
    };
    
    class EntityGroupManager : public Object {
    public:
        EntityGroupManager(Object* parent) : Object(parent) {
            auto default_group = std::make_shared<EntityGroup>(this, "[default]");
            groups.push_back(default_group);
        }
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>>(groups.begin(), groups.end()); }
        std::string_view GetName() { return "Entities"; }
        bool IsChildrenListable() { return true; }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<EntityGroup>(this); groups.push_back(child); return child; }
        void AddChild(std::shared_ptr<Object> child) { groups.push_back(std::dynamic_pointer_cast<EntityGroup>(child)); }
        void RemoveChild(std::shared_ptr<Object> child) { groups.remove(std::dynamic_pointer_cast<EntityGroup>(child)); }
    
        std::list<std::shared_ptr<EntityGroup>> groups;
    };
    
    
    

    class Transition : public Object {
    public:
        class Node : public Object {
            glm::vec3 location;
            Transition* parent;
        };

        Transition(Object* parent) : Transition(parent, "untitled transition", "none") {}
        Transition(Object* parent, std::string_view name, std::string_view cell_into) : Object(parent), name(name), cell_into_name(cell_into) {}
        
        bool IsChildrenListable() { return true; }

        std::string name;
        std::string cell_into_name;
        std::list<std::shared_ptr<Node>> nodes;
    };
    
    class TransitionManager : public Object {
    public:
        TransitionManager(Object* parent) : Object(parent) {}
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>>(transitions.begin(), transitions.end()); }
        std::string_view GetName() { return "Transitions"; }
    
        std::list<std::shared_ptr<Transition>> transitions;
    };
    
    
    

    class Path : public Object {
        class Node : public Object {
            uint64_t id;
            uint64_t next_id;
            uint64_t prev_id;
            uint64_t left_id;
            uint64_t right_id;
            glm::vec3 location1;
            glm::vec3 location2;
            glm::vec3 location3;
            glm::vec3 location4;
        };
        
        Path(Object* parent) : Path(parent, "untitled path") {}
        Path(Object* parent, std::string_view name) : Object(parent), name(name) {}

        std::string name;
        std::list<std::shared_ptr<Node>> nodes;
    };
    
    class PathManager : public Object {
    public:
        PathManager(Object* parent) : Object(parent) {}
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>>(paths.begin(), paths.end()); }
        std::string_view GetName() { return "Paths"; }
    
        std::list<std::shared_ptr<Path>> paths;
    };
    
    

    class Navmesh : public Object {
        class Node : public Object {
            uint64_t id;
            uint64_t next_id;
            uint64_t prev_id;
            uint64_t left_id;
            uint64_t right_id;
            glm::vec3 location;
            Navmesh* parent;
        };

        std::string name;
        std::list<std::shared_ptr<Node>> nodes;
    };
    
    class NavmeshManager : public Object {
    public:
        NavmeshManager(Object* parent) : Object(parent) {}
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>>(navmeshes.begin(), navmeshes.end()); }
        std::string_view GetName() { return "Navmeshes"; }
    
        std::list<std::shared_ptr<Navmesh>> navmeshes;
    };





    class WorldCell : public Object {
    public:
        std::shared_ptr<EntityGroupManager> group_manager;
        WorldCell() : Object(nullptr), group_manager(std::make_shared<EntityGroupManager>(this)) { }
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>> { group_manager }; }
        std::string_view GetName() { return name; }
        
        std::string name = "untitled worldcell";
    };
    
    class WorldCellManager : public Object {
    public:
        WorldCellManager(Object* parent) {}
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>>(cells.begin(), cells.end()); }
        std::string_view GetName() { return "Pasaule"; }
    
        std::list<std::shared_ptr<WorldCell>> cells;
    };
    
    extern std::shared_ptr<WorldCellManager> worldcells;
}


namespace Editor {
    namespace WorldTree {
        void Add (Object* object);
        void Remove (Object* object);
        void Rename (Object* object);
        void Rebuild();
        std::shared_ptr<Object> GetObject(void* Id);
    }
    
    namespace PropertyPanel {
        
    }
    
    namespace ObjectList {
        
        
    }
    
    
    
    /*struct WorldCell;
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
            Transition* parent;
        };
        
        Point* NewPoint();
        void DeletePoint(Point* point);
        
        bool is_visible = false;
        std::string name;
        std::string cell_into_name;
        WorldCell* parent;
        std::vector<Point*> points;
        Core::WorldCell::Transition* v_transition = nullptr;
    };
    
    struct EntityGroup {
        std::string name;
        std::vector<Entity*> entities;
        WorldCell* parent;
        
        bool is_visible = false;
        Entity* NewEntity();
        void DeleteEntity(Entity* entity);
    };
    
    struct Path {
        struct Curve {
            uint64_t id;
            uint64_t next_id;
            uint64_t prev_id;
            uint64_t left_id;
            uint64_t right_id;
            glm::vec3 location1;
            glm::vec3 location2;
            glm::vec3 location3;
            glm::vec3 location4;
            Path* parent;
        };
        
        Curve* NewCurve();
        void DeleteCurve(Curve* curve);
        
        bool is_visible = false;
        std::string name;
        WorldCell* parent;
        std::vector<Curve*> curves;
        //Core::WorldCell::Transition* v_transition = nullptr;
    };
    
    struct Navmesh {
        struct Node {
            uint64_t id;
            uint64_t next_id;
            uint64_t prev_id;
            uint64_t left_id;
            uint64_t right_id;
            glm::vec3 location;
            Navmesh* parent;
        };
        
        Node* NewNode();
        void DeleteNode(Node* node);
        bool is_visible = false;
        
        std::string name;
        WorldCell* parent;
        std::vector<Node*> nodes;
    };
    
    struct WorldCell {
        std::string name;
        bool is_interior = false;
        bool is_interior_lighting = false;
        
        bool is_visible = false;
        bool is_deleted = false;
        
        bool is_transitions_visible = false;
        bool is_paths_visible = false;
        bool is_navmeshes_visible = false;
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
            NONE,
            CELL_ITSELF,
            CELL_ENTITIES,
            CELL_ENTITY_GROUPS,
            CELL_TRANSITIONS,
            CELL_PATHS,
            CELL_NAVMESHES,
            ENTITY,
            ENTITY_GROUP,
            TRANSITION,
            PATH,
            NAVMESH,
            TRANSITION_POINT,
            PATH_CURVE,
            NAVMESH_NODE
        } indirection_type = NONE;
        WorldCell* into = nullptr;
        Entity* entity = nullptr;
        Transition* trans = nullptr;
        EntityGroup* group = nullptr;
        Path* path = nullptr;
        Navmesh* navmesh = nullptr;
        Transition::Point* point = nullptr;
        Path::Curve* curve = nullptr;
        Navmesh::Node* node = nullptr;

        
        void Show();
        void Hide();
        void Begonis();
        void Delete(); // delete should return something, just like new does
        bool IsVisible();
        bool IsNewable();
        bool IsEditable();
        bool IsDeletable();
        WorldCellIndirector New();
        WorldCellIndirector Index(long index);
    };
    
    using Selector = WorldCellIndirector;
    
    WorldCell* NewWorldCell();
    
    extern std::vector<WorldCell*> worldCells;
    extern std::unordered_map<std::string, Core::SerializedEntityData* (*)(void)> entityDatas;*/
}

#endif // EDITOR_H