#ifndef EDITOR_EDITOR_H
#define EDITOR_EDITOR_H

#include <list>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <iostream>
#include <framework/serialization.h>

namespace tram {
    class RenderComponent;
    class SerializedEntityData;
}

namespace Editor {

typedef void* worldTreeID_t;
class Object;

/// World tree panel.
namespace WorldTree {
    void Add (Object* object);
    void Remove (Object* object);
    void Rename (Object* object);
    void Rebuild();
    std::shared_ptr<Object> GetObject(void* Id);
}

// I think that we should remove SetCurrentSelection() functions from all of these
/// Property panel.
namespace PropertyPanel {
    void SetCurrentSelection();
    void Refresh();
}

/// Object list panel.
namespace ObjectList {
    void SetCurrentSelection();
    void Refresh();
}

/// Viewport panel.
namespace Viewport {
    void SetCurrentSelection();
    void Refresh();
}

void Init();
void Reset();
void Yeet();

/// Editor action.
class Action {
public:
    virtual void Perform() = 0;
    virtual void Unperform() = 0;
    virtual ~Action() = default;
};

// TODO: all caps, maybe?
extern std::list<std::unique_ptr<Action>> PERFORMED_ACTIONS;
extern std::list<std::unique_ptr<Action>> UNPERFORMED_ACTIONS;

/// Performs an Action and adds it to the list of performed actions.
template <typename action, typename ...Args> void PerformAction (Args && ...args) {
    if (PERFORMED_ACTIONS.size() > 20) {
        PERFORMED_ACTIONS.pop_front();
    }
    UNPERFORMED_ACTIONS.clear();
    PERFORMED_ACTIONS.push_back(std::make_unique<action>(std::forward<Args>(args)...));
}

/// Unperforms a single action.
inline void Undo() {
    if (PERFORMED_ACTIONS.size()) {
        UNPERFORMED_ACTIONS.push_back(std::move(PERFORMED_ACTIONS.back()));
        UNPERFORMED_ACTIONS.back()->Unperform();
        PERFORMED_ACTIONS.pop_back();
    }
}

/// Reperforms a single action that was previously unperformed.
inline void Redo() {
    if (UNPERFORMED_ACTIONS.size()) {
        PERFORMED_ACTIONS.push_back(std::move(UNPERFORMED_ACTIONS.back()));
        PERFORMED_ACTIONS.back()->Perform();
        UNPERFORMED_ACTIONS.pop_back();
    }
}

/// Property types for objects.
enum PropertyType {
    PROPERTY_STRING,
    PROPERTY_FLOAT,     // float
    PROPERTY_INT,       // int64_t
    PROPERTY_UINT,      // uint64_t
    PROPERTY_ENUM,      // uint32_t
    PROPERTY_BOOL,      // bool
    PROPERTY_CATEGORY,
    PROPERTY_NULL
};

// property name being a string is very inefficient, but it's the easier option
// when working with the wxWidgets property panel widget
/// Definition of a property.
/// These definitions determine how the Object properties will be displayed
/// in the PropertyPanel and the ObjectList.
struct PropertyDefinition {
    std::string name;           /// Name of the property.
    std::string display_name; // TODO: yeet this
    std::string category_name;  /// Category to which this property belongs.
    PropertyType type;          /// Type of the property.
};

/// Enumeration property values.
/// Map that contains lists of possible values for all enumerations.
extern std::unordered_map<std::string, std::vector<std::string>> PROPERTY_ENUMERATIONS;

// maybe add a move constructor to this class?
/// Value of a property.
struct PropertyValue {
    PropertyValue() { type = PROPERTY_NULL; }
    PropertyValue(const std::string& value) : str_value(value) { type = PROPERTY_STRING; }
    PropertyValue(const float& value) : float_value(value) { type = PROPERTY_FLOAT; }
    PropertyValue(const int64_t& value) : int_value(value) { type = PROPERTY_INT; }
    PropertyValue(const uint64_t& value) : uint_value(value) { type = PROPERTY_UINT; }
    PropertyValue(const int32_t& value) : enum_value(value) { type = PROPERTY_ENUM; }
    PropertyValue(const bool& value) : bool_value(value) { type = PROPERTY_BOOL; }
    PropertyValue(const PropertyValue& value, PropertyType type) : PropertyValue(value) { this->type = type; }
    ~PropertyValue() { if (type == PROPERTY_STRING) str_value.~basic_string(); }
    PropertyValue (const PropertyValue& value) : PropertyValue() { *this = value; }
    PropertyValue& operator=(const PropertyValue& value) {
        this->~PropertyValue();
        switch (value.type) {
            case PROPERTY_STRING:
                new (&str_value) std::string(value.str_value);
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
            case PROPERTY_BOOL:
                bool_value = value.bool_value;
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
            case PROPERTY_BOOL:
                return bool_value == other.bool_value;
            case PROPERTY_CATEGORY:
                return false;
            case PROPERTY_NULL:
                return true;
        }
        return false;
    }
    
    operator std::string() { assert(type == PROPERTY_STRING); return str_value; }
    operator float() { assert(type == PROPERTY_FLOAT); return float_value; }
    operator int64_t() { assert(type == PROPERTY_INT); return int_value; }
    operator uint64_t() { assert(type == PROPERTY_UINT); return uint_value; }
    operator int32_t() { assert(type == PROPERTY_ENUM); return enum_value; }
    operator bool() { assert(type == PROPERTY_BOOL); return bool_value; }
    
    PropertyType type;
    union {
        std::string str_value;
        float float_value;
        int64_t int_value;
        uint64_t uint_value;
        int32_t enum_value;
        bool bool_value;
    };
};

/// Editor object base class.
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
    
    virtual std::shared_ptr<Object> Duplicate() { std::cout << "Duplicate(void) not implemented for " << typeid(*this).name() << std::endl; abort(); }
    
    virtual void Draw() {}
    
    // these are the properties that will be shown in the object list
    virtual std::vector<PropertyDefinition> GetListPropertyDefinitions() { std::cout << "GetListPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
    
    // these properties will be shown and will be editable from property panel
    virtual std::vector<PropertyDefinition> GetFullPropertyDefinitions() { std::cout << "GetFullPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
    
    // these properties will be used for serialization
    virtual std::vector<PropertyDefinition> GetSerializationPropertyDefinitions() { std::cout << "GetSerializationPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; abort(); }
    
    // takes in the property name and returns the value by copying it to the void* pointer
    virtual PropertyValue GetProperty (std::string property_name) { return properties[property_name]; }
    
    // takes in the property name and copies the value from the void* pointer into it
    virtual void SetProperty (std::string property_name, PropertyValue property_value) { properties[property_name] = property_value; if (property_name == "name" && parent && parent->IsChildrenTreeable()) WorldTree::Rename(this); }
};

void RegisterEntityType(std::string name, std::string model_name, std::vector<PropertyDefinition> definitions);

class WorldCell;
void LoadCell(WorldCell* cell);
void SaveCell(WorldCell* cell);

struct Selection {
    std::list<std::shared_ptr<Object>> objects;
};

extern bool data_modified;
extern std::shared_ptr<Selection> SELECTION;

}

namespace Editor {

































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