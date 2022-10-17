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
        virtual ~Action() = default;
    };
    
    extern std::list<std::unique_ptr<Action>> performed_actions;
    
    template <typename action, typename ...Args> void PerformAction (Args && ...args) {
        // TODO: check if performed_actions list isn't too full!
        performed_actions.push_back(std::make_unique<action>(std::forward<Args>(args)...));
    }
    
    enum PropertyType {
        PROPERTY_STRING,    // char*
        PROPERTY_FLOAT,     // float
        PROPERTY_INT,       // int64_t
        PROPERTY_UINT,      // uint64_t
        PROPERTY_CATEGORY,
        PROPERTY_NULL
    };
    
    // property name being a string is very inefficient, but it's the easier option
    // when working with the wxWidgets property panel widget
    struct PropertyDefinition {
        std::string name;
        std::string display_name;
        std::string category_name;
        PropertyType type;
    };
    
    struct PropertyValue {
        PropertyValue() { type = PROPERTY_NULL; }
        PropertyValue(const std::string& value) : str_value(value) { type = PROPERTY_STRING; }
        PropertyValue(const float& value) : float_value(value) { type = PROPERTY_FLOAT; }
        PropertyValue(const int64_t& value) : int_value(value) { type = PROPERTY_INT; }
        PropertyValue(const uint64_t& value) : uint_value(value) { type = PROPERTY_UINT; }
        ~PropertyValue() { if (type == PROPERTY_STRING) str_value.~basic_string(); }
        PropertyValue (const PropertyValue& value) : PropertyValue() { *this = value; }
        PropertyValue& operator=(const PropertyValue& value) {
            this->~PropertyValue();
            switch (value.type) {
                case PROPERTY_STRING:
                    new (&str_value) std::string(value.str_value);
                    //str_value = value.str_value;
                    break;
                case PROPERTY_FLOAT:
                    float_value = value.float_value;
                    break;
                case PROPERTY_INT:
                    int_value = value.int_value;
                    break;
                case PROPERTY_UINT:
                    uint_value = value.uint_value;
                    break;
                default:
                    break;
            }
            
            type = value.type;
            return *this;
        }
        
        PropertyType type;
        union {
            std::string str_value;
            float float_value;
            int64_t int_value;
            uint64_t uint_value;
        };
    };
    
    class Object : public std::enable_shared_from_this<Object> {
    public:
        Object() = default;
        Object(Object* parent) : parent(parent) {};
        virtual ~Object() {}
        
        std::list<std::shared_ptr<Object>> children;
        std::unordered_map<std::string, PropertyValue> properties;
        
        std::shared_ptr<Object> GetPointer() { return shared_from_this(); }
        std::shared_ptr<Object> GetParent() { return parent->GetPointer(); }
        std::shared_ptr<Object> GetCopy() { return GetPointer(); } // placeholder
        
        virtual std::string_view GetName() { return properties["name"].str_value; }
        
        void AddChild(std::shared_ptr<Object> child) { children.push_back(child); }
        void RemoveChild(std::shared_ptr<Object> child) { children.remove(child); }
        // TODO: add a SwapChild() method
        virtual std::list<std::shared_ptr<Object>> GetChildren() { return children; }
    
        bool hidden = true;
        Object* parent = nullptr;
        
        //virtual std::list<std::shared_ptr<Object>> GetChildren() { std::cout << "GetChildren() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        //virtual std::string_view GetName() { std::cout << "GetName() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsChildrenTreeable() { std::cout << "IsChildrenTreeable() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsChildrenListable() { std::cout << "IsChildrenListable() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual void SetHidden(bool is_hidden) { std::cout << "SetHidden() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual std::shared_ptr<Object> AddChild() { std::cout << "AddChild(void) not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        //virtual void AddChild(std::shared_ptr<Object>) { std::cout << "AddChild(std::shared_ptr) not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        //virtual void RemoveChild(std::shared_ptr<Object>) { std::cout << "RemoveChild() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsRemovable() { std::cout << "IsRemovable() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsRemovableChildren() { std::cout << "IsRemovableChildren() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsAddableChildren() { std::cout << "IsAddableChildren() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        virtual bool IsHidden() { std::cout << "IsHidden() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        
        // these are the properties that will be shown in the object list
        virtual std::vector<PropertyDefinition> GetListPropertyDefinitions() { std::cout << "GetListPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        
        // these properties will be shown and will be editable from property panel
        virtual std::vector<PropertyDefinition> GetFullPropertyDefinitions() { std::cout << "GetFullPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        
        // takes in the property name and returns the value by copying it to the void* pointer
        //virtual PropertyValue GetProperty (std::string property_name) { std::cout << "GetProperty() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        PropertyValue GetProperty (std::string property_name) { return properties[property_name]; }
        
        // takes in the property name and copies the value from the void* pointer into it
        //virtual void SetProperty (std::string property_name, PropertyValue property_value) { std::cout << "SetProperty() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        void SetProperty (std::string property_name, PropertyValue property_value) { properties[property_name] = property_value; }
        
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
        Entity(Object* parent) : Entity(parent, "Unnamed entity") {}
        Entity(Object* parent, std::string name) : Object(parent) {
            properties["name"] = name;
        }
    
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-entity", "Entity", "", PROPERTY_CATEGORY},
                {"name", "Name", "group-entity", PROPERTY_STRING},
                {"group-position", "Location", "", PROPERTY_CATEGORY},
                {"position-x", "X", "", PROPERTY_FLOAT},
                {"position-y", "Y", "", PROPERTY_FLOAT},
                {"position-z", "Z", "", PROPERTY_FLOAT}
            };
        }
        
    };
    
    class EntityGroup : public Object {
    public:
        EntityGroup(Object* parent) : EntityGroup(parent, std::string("Untitled EntityGroup ") + std::to_string(parent->GetChildren().size())) {}
        EntityGroup(Object* parent, std::string name) : Object(parent) {
            properties["name"] = name;
        }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return true; }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Entity>(this); children.push_back(child); return child; }

        
        std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"name", "Name", "", PROPERTY_STRING},
                {"position-x", "X", "", PROPERTY_FLOAT},
                {"position-y", "Y", "", PROPERTY_FLOAT},
                {"position-z", "Z", "", PROPERTY_FLOAT}
            };
        }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-entity-group", "Entity Group", "", PROPERTY_CATEGORY},
                {"name", "Name", "group-entity-group", PROPERTY_STRING}
            };
        }
     };
    
    class EntityGroupManager : public Object {
    public:
        EntityGroupManager(Object* parent) : Object(parent) {
            auto default_group = std::make_shared<EntityGroup>(this, "[default]");
            children.push_back(default_group);
            
            properties["name"] = std::string("Entities");
        }
        
        bool IsChildrenTreeable() { return true; }
        bool IsChildrenListable() { return true; }
        
        std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"name", "Name", "", PROPERTY_STRING}
            };
        }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-entity-group-manager", "Entity Group Manager", "", PROPERTY_CATEGORY}
            };
        }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<EntityGroup>(this); children.push_back(child); return child; }
    };
    
    
    

    class Transition : public Object {
    public:
        class Node : public Object {
            glm::vec3 location;
            Transition* parent;
        };

        Transition(Object* parent) : Object(parent) {}
        //Transition(Object* parent) : Transition(parent, "untitled transition", "none") {}
        //Transition(Object* parent, std::string_view name, std::string_view cell_into) : Object(parent){}
        
        bool IsChildrenTreeable() { return true; }
        bool IsChildrenListable() { return true; }
    };
    
    class TransitionManager : public Object {
    public:
        TransitionManager(Object* parent) : Object(parent) {}
        
        std::string_view GetName() { return "Transitions"; }
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
        
        Path(Object* parent) : Object(parent) {}
        //Path(Object* parent, std::string_view name) : Object(parent), name(name) {}
    };
    
    class PathManager : public Object {
    public:
        PathManager(Object* parent) : Object(parent) {}
        
        std::string_view GetName() { return "Paths"; }
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
    };
    
    class NavmeshManager : public Object {
    public:
        NavmeshManager(Object* parent) : Object(parent) {}
        
        std::string_view GetName() { return "Navmeshes"; }

    };





    class WorldCell : public Object {
    public:
        WorldCell(Object* parent) : WorldCell(parent, "WorldCell") {}
        WorldCell(Object* parent, std::string name) : Object(parent), group_manager(std::make_shared<EntityGroupManager>(this)) {
            properties["name"] = name;
        }
        
                bool IsChildrenTreeable() { return true; }
        bool IsChildrenListable() { return true; }
        
        std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"name", "Name", "", PROPERTY_STRING}
            };
        }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-worldcell", "Worldcell", "", PROPERTY_CATEGORY},
                {"name", "Name", "group-worldcell", PROPERTY_STRING}
            };
        }
        
        std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>> { group_manager }; }
        
        std::shared_ptr<EntityGroupManager> group_manager;
    };
    
    class WorldCellManager : public Object {
    public:
        WorldCellManager(Object* parent) {
            properties["name"] = std::string("Pasaule");
        }
        
        std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"name", "Name", "", PROPERTY_STRING}
            };
        }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-world-cell-manager", "WorldCell Manager", "", PROPERTY_CATEGORY}
            };
        }
        
                bool IsChildrenTreeable() { return true; }
        bool IsChildrenListable() { return true; }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<WorldCell>(this); children.push_back(child); return child; }
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
        void SetCurrentSelection();
    }
    
    namespace ObjectList {
        void SetCurrentSelection();
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