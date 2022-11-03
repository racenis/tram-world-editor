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
    class Object;
    
    namespace WorldTree {
        void Add (Object* object);
        void Remove (Object* object);
        void Rename (Object* object);
        void Rebuild();
        std::shared_ptr<Object> GetObject(void* Id);
    }
    
    // I think that we should remove SetCurrentSelection() functions from all of these
    namespace PropertyPanel {
        void SetCurrentSelection();
        void Refresh();
    }
    
    namespace ObjectList {
        void SetCurrentSelection();
        void Refresh();
    }
    
    namespace Viewport {
        void SetCurrentSelection();
        void Refresh();
    }
    
    namespace Settings {
        enum Space {
            SPACE_WORLD,
            SPACE_ENTITY,
            SPACE_ENTITYGROUP
        };
        
        enum Rotation {
            ROTATION_RADIANS,
            ROTATION_DEGREES
        };
        
        extern Space TRANSFORM_SPACE;
        extern Rotation ROTATION_UNIT;
    }
    
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
    extern std::list<std::unique_ptr<Action>> unperformed_actions;
    
    template <typename action, typename ...Args> void PerformAction (Args && ...args) {
        if (performed_actions.size() > 20) {
            std::cout << "Redo/Undo list size exceeded, popping." << std::endl;
            performed_actions.pop_front();
        }
        unperformed_actions.clear();
        performed_actions.push_back(std::make_unique<action>(std::forward<Args>(args)...));
    }
    
    inline void Undo() {
        if (performed_actions.size()) {
            unperformed_actions.push_back(std::move(performed_actions.back()));
            unperformed_actions.back()->Unperform();
            performed_actions.pop_back();
        } else {
            std::cout << "Nothing to UNDO!" << std::endl;
        }
    }
    
    inline void Redo() {
        if (unperformed_actions.size()) {
            performed_actions.push_back(std::move(unperformed_actions.back()));
            performed_actions.back()->Perform();
            unperformed_actions.pop_back();
        } else {
            std::cout << "Nothing to REDO!" << std::endl;
        }
    }
    
    enum PropertyType {
        PROPERTY_STRING,    // char*
        PROPERTY_FLOAT,     // float
        PROPERTY_INT,       // int64_t
        PROPERTY_UINT,      // uint64_t
        PROPERTY_ENUM,      // uint32_t
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
    
    extern std::unordered_map<std::string, std::vector<std::string>> property_enumerations;
    
    // maybe add a move constructor to this class?
    struct PropertyValue {
        PropertyValue() { type = PROPERTY_NULL; }
        PropertyValue(const std::string& value) : str_value(value) { type = PROPERTY_STRING; }
        PropertyValue(const float& value) : float_value(value) { type = PROPERTY_FLOAT; }
        PropertyValue(const int64_t& value) : int_value(value) { type = PROPERTY_INT; }
        PropertyValue(const uint64_t& value) : uint_value(value) { type = PROPERTY_UINT; }
        PropertyValue(const int32_t& value) : enum_value(value) { type = PROPERTY_ENUM; }
        PropertyValue(const PropertyValue& value, PropertyType type) : PropertyValue(value) { this->type = type; }
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
                case PROPERTY_ENUM:
                    enum_value = value.enum_value;
                    break;
                default:
                    break;
            }
            
            type = value.type;
            return *this;
        }
        
        bool operator==(const PropertyValue& other) const {
            if (other.type != type) return false;
            
            switch (type) {
                case PROPERTY_STRING:
                    return str_value == other.str_value;
                case PROPERTY_FLOAT:
                    return float_value == other.float_value;
                case PROPERTY_INT:
                    return int_value == other.int_value;
                case PROPERTY_UINT:
                    return uint_value == other.uint_value;
                case PROPERTY_ENUM:
                    return enum_value == other.enum_value;
                case PROPERTY_CATEGORY:
                    return false;
                case PROPERTY_NULL:
                    return true;
            }
            return false;
        }
        
        PropertyType type;
        union {
            std::string str_value;
            float float_value;
            int64_t int_value;
            uint64_t uint_value;
            int32_t enum_value;
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
        std::shared_ptr<Object> GetCopy() { return GetPointer(); } // TODO: implement copying
        
        auto GetProperties() { return properties; }
        void SetProperties(auto properties) { this->properties = properties; }
        
        virtual std::string_view GetName() { return properties["name"].str_value; }
        
        void AddChild(std::shared_ptr<Object> child) { children.push_back(child); }
        void RemoveChild(std::shared_ptr<Object> child) { children.remove(child); }
        virtual std::list<std::shared_ptr<Object>> GetChildren() { return children; }
    
        bool is_hidden = true;
        Object* parent = nullptr;
        
        virtual bool IsChildrenTreeable() { std::cout << "IsChildrenTreeable() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual bool IsChildrenListable() { std::cout << "IsChildrenListable() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual void SetHidden(bool is_hidden) { for (auto& child : children) child->SetHidden(is_hidden); }
        virtual std::shared_ptr<Object> AddChild() { std::cout << "AddChild(void) not implemented for " << typeid(*this).name() << std::endl; abort(); }

        virtual bool IsAddable() { std::cout << "IsAddable() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual bool IsRemovable() { std::cout << "IsRemovable() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual bool IsEditable() { std::cout << "IsEditable() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual bool IsCopyable() { std::cout << "IsCopyable() not implemented for " << typeid(*this).name() << std::endl; abort(); }
        virtual bool IsHidden() { for (auto& child : children) if (child->IsHidden()) return true; return false; }
        
        // these are the properties that will be shown in the object list
        virtual std::vector<PropertyDefinition> GetListPropertyDefinitions() { std::cout << "GetListPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        
        // these properties will be shown and will be editable from property panel
        virtual std::vector<PropertyDefinition> GetFullPropertyDefinitions() { std::cout << "GetFullPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
        
        // takes in the property name and returns the value by copying it to the void* pointer
        virtual PropertyValue GetProperty (std::string property_name) { return properties[property_name]; }
        
        // takes in the property name and copies the value from the void* pointer into it
        virtual void SetProperty (std::string property_name, PropertyValue property_value) { properties[property_name] = property_value; if (property_name == "name" && parent && parent->IsChildrenTreeable()) WorldTree::Rename(this); }
        
    };
    
    class WorldCell;
    void LoadCell(WorldCell* cell);
    void SaveCell(WorldCell* cell);
    
    class Selection {
    public:
        std::list<std::shared_ptr<Object>> objects;
    };
    
    extern std::shared_ptr<Selection> selection;
}

namespace Editor {
    struct EntityData;
    class Entity : public Object {
    public:
        Entity(Object* parent) : Entity(parent, "New Entity") {}
        Entity(Object* parent, std::string name) : Object(parent) {
            properties["name"] = name;
            properties["action"] = std::string("none");
            properties["position-x"] = 0.0f;
            properties["position-y"] = 0.0f;
            properties["position-z"] = 0.0f;
            properties["rotation-x"] = 0.0f;
            properties["rotation-y"] = 0.0f;
            properties["rotation-z"] = 0.0f;
            properties["entity-type"] = (int32_t) 0;
        }
    
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return false; }
        bool IsAddable() { return false; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        bool IsHidden() { return is_hidden; }
        
        void SetHidden(bool is_hidden);
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions();
        
        PropertyValue GetProperty (std::string property_name);
        
        void SetProperty (std::string property_name, PropertyValue property_value);
        
        EntityData* entity_data = nullptr;
    };
    
    class EntityGroup : public Object {
    public:
        EntityGroup(Object* parent) : EntityGroup(parent, std::string("Untitled EntityGroup ") + std::to_string(parent->GetChildren().size())) {}
        EntityGroup(Object* parent, std::string name) : Object(parent) {
            properties["name"] = name;
        }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return true; }
        bool IsAddable() { return true; }
        bool IsRemovable() { return properties["name"].str_value != "[default]"; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
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
        bool IsAddable() { return true; }
        bool IsRemovable() { return false; }
        bool IsEditable() { return false; }
        bool IsCopyable() { return false; }
        
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
        public:
            Node (Object* parent) : Object(parent) {
                properties["position-x"] = 0.0f;
                properties["position-y"] = 0.0f;
                properties["position-z"] = 0.0f;
            }
            
            std::string_view GetName() { return "Transition Node"; }
            
            bool IsChildrenTreeable() { return false; }
            bool IsChildrenListable() { return false; }
            bool IsAddable() { return false; }
            bool IsRemovable() { return true; }
            bool IsEditable() { return true; }
            bool IsCopyable() { return true; }
            
            /*std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
                return std::vector<PropertyDefinition> {
                    {"name", "Name", "", PROPERTY_STRING},
                    {"position-x", "X", "", PROPERTY_FLOAT},
                    {"position-y", "Y", "", PROPERTY_FLOAT},
                    {"position-z", "Z", "", PROPERTY_FLOAT}
                };
            }*/
            
            std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
                return std::vector<PropertyDefinition> {
                    {"group-transition-node", "Transition Node", "", PROPERTY_CATEGORY},
                    {"position-x", "X", "group-transition-node", PROPERTY_FLOAT},
                    {"position-y", "Y", "group-transition-node", PROPERTY_FLOAT},
                    {"position-z", "Z", "group-transition-node", PROPERTY_FLOAT}
                };
            }
        };

        Transition(Object* parent) : Object(parent) {
            properties["name"] = std::string("Transition");
        }
        
        bool IsChildrenTreeable() { return false; }
        bool IsChildrenListable() { return true; }
        bool IsAddable() { return true; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return true; }
        bool IsCopyable() { return true; }
        
        std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"position-x", "X", "", PROPERTY_FLOAT},
                {"position-y", "Y", "", PROPERTY_FLOAT},
                {"position-z", "Z", "", PROPERTY_FLOAT}
            };
        }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-transition", "Transition", "", PROPERTY_CATEGORY},
                {"name", "Name", "group-transition", PROPERTY_STRING}
            };
        }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Node>(this); children.push_back(child); return child; }
    };
    
    class TransitionManager : public Object {
    public:
        TransitionManager(Object* parent) : Object(parent) {}
        
        std::string_view GetName() { return "Transitions"; }
        
        bool IsChildrenTreeable() { return true; }
        bool IsChildrenListable() { return true; }
        bool IsAddable() { return true; }
        bool IsRemovable() { return true; }
        bool IsEditable() { return false; }
        bool IsCopyable() { return false; }
        
        std::vector<PropertyDefinition> GetListPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"name", "Name", "", PROPERTY_STRING}
            };
        }
        
        std::vector<PropertyDefinition> GetFullPropertyDefinitions() { 
            return std::vector<PropertyDefinition> {
                {"group-transition-manager", "Transition Manager", "", PROPERTY_CATEGORY}
            };
        }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<Transition>(this); children.push_back(child); return child; }
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
        WorldCell(Object* parent, std::string name) : Object(parent), group_manager(std::make_shared<EntityGroupManager>(this)), 
            transition_manager(std::make_shared<TransitionManager>(this)) {
            properties["name"] = name;
            
            children.push_back(group_manager);
            children.push_back(transition_manager);
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
                {"name", "Name", "group-worldcell", PROPERTY_STRING}
            };
        }
        
        //std::list<std::shared_ptr<Object>> GetChildren() { return std::list<std::shared_ptr<Object>> { group_manager }; }
        
        std::shared_ptr<EntityGroupManager> group_manager;
        std::shared_ptr<TransitionManager> transition_manager;
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
        bool IsAddable() { return true; }
        bool IsRemovable() { return false; }
        bool IsEditable() { return false; }
        bool IsCopyable() { return false; }
        
        std::shared_ptr<Object> AddChild() { auto child = std::make_shared<WorldCell>(this); children.push_back(child); return child; }
    };
    
    extern std::shared_ptr<WorldCellManager> worldcells;
}


namespace Editor {

    
    
    
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