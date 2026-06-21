#ifndef EDITOR_EDITOR_H
#define EDITOR_EDITOR_H

#include <list>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>
#include <unordered_map>
#include <iostream>
#include <framework/math.h>

#include <platform/other.h>

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
    void Deselect();
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
    void Deselect();
    void Refresh();
}

/// Viewport panel.
namespace Viewport {
    void SetCurrentSelection();
    void Refresh();
    void ShowErrorDialog(std::string msg);
    
    extern float CURSOR_X, CURSOR_Y, CURSOR_Z;
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
    PROPERTY_INT,       // int32_t
    PROPERTY_UINT,      // uint32_t
    PROPERTY_ENUM,      // int32_t
    PROPERTY_FLAG,      // int32_t
    PROPERTY_BOOL,      // bool
    PROPERTY_VECTOR,
    PROPERTY_ORIGIN,
    PROPERTY_DIRECTION, // normal vector
    //PROPERTY_POSITION,  // position vector
    PROPERTY_NULL
};

// property name being a string is very inefficient, but it's the easier option
// when working with the wxWidgets property panel widget
/// Definition of a property.
/// These definitions determine how the Object properties will be displayed
/// in the PropertyPanel and the ObjectList.
struct PropertyDefinition {
    std::string name;           /// Name of the property.
    std::string category_name;  /// Category to which this property belongs.
    PropertyType type;          /// Type of the property.
};

/// Enumeration property values.
/// Map that contains lists of possible values for all enumerations.
extern std::unordered_map<std::string, std::vector<std::string>> PROPERTY_ENUMERATIONS;

// maybe add a move constructor to this class?
/// Value of a property.
struct PropertyValue {    
    inline static PropertyValue String(const std::string& value) {
        PropertyValue v; v.type = PROPERTY_STRING; new (&v.str_value) std::string(value); return v;
    }
    
    inline static PropertyValue Float(const float& value) {
        PropertyValue v; v.type = PROPERTY_FLOAT; v.float_value = value; return v;
    }
    
    inline static PropertyValue Int(const int32_t& value) {
        PropertyValue v; v.type = PROPERTY_INT; v.int_value = value; return v;
    }
    
    inline static PropertyValue Enum(const int32_t& value) {
        PropertyValue v; v.type = PROPERTY_ENUM; v.int_value = value; return v;
    }
    
    inline static PropertyValue UInt(const uint32_t& value) {
        PropertyValue v; v.type = PROPERTY_UINT; v.uint_value = value; return v;
    }
    
    inline static PropertyValue Flag(const uint32_t& value) {
        PropertyValue v; v.type = PROPERTY_FLAG; v.uint_value = value; return v;
    }
    
    inline static PropertyValue Bool(const bool& value) {
        PropertyValue v; v.type = PROPERTY_BOOL; v.bool_value = value; return v;
    }
    
    inline static PropertyValue Vector(const tram::vec3& value) {
        PropertyValue v; v.type = PROPERTY_VECTOR; v.vector_value = value; return v;
    }
    
    PropertyValue() { type = PROPERTY_NULL; }
    PropertyValue(const std::string& value) : str_value(value) { type = PROPERTY_STRING; }
    PropertyValue(const float& value) : float_value(value) { type = PROPERTY_FLOAT; }
    PropertyValue(const uint32_t& value) : uint_value(value) { type = PROPERTY_UINT; }
    PropertyValue(const int32_t& value) : int_value(value) { type = PROPERTY_INT; }
    PropertyValue(const bool& value) : bool_value(value) { type = PROPERTY_BOOL; }
    PropertyValue(const tram::vec3& value) : vector_value(value) { type = PROPERTY_VECTOR; }
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
            case PROPERTY_ENUM:
            case PROPERTY_INT:
                int_value = value.int_value;
                break;
            case PROPERTY_FLAG:
            case PROPERTY_UINT:
                uint_value = value.uint_value;
                break;
            case PROPERTY_BOOL:
                bool_value = value.bool_value;
                break;
            case PROPERTY_VECTOR:
            case PROPERTY_ORIGIN:
            case PROPERTY_DIRECTION:
                vector_value = value.vector_value;
                break;
            case PROPERTY_NULL:
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
            case PROPERTY_ENUM:
            case PROPERTY_INT:
                return int_value == other.int_value;
            case PROPERTY_FLAG:
            case PROPERTY_UINT:
                return uint_value == other.uint_value;
            case PROPERTY_BOOL:
                return bool_value == other.bool_value;
            case PROPERTY_VECTOR:
            case PROPERTY_DIRECTION:
            case PROPERTY_ORIGIN:
                return vector_value.x == other.vector_value.x && vector_value.y == other.vector_value.y && vector_value.z == other.vector_value.z;
            case PROPERTY_NULL:
                return true;
        }
        return false;
    }
    
    operator std::string() const { assert(type == PROPERTY_STRING); return str_value; }
    operator float() const { if (type != PROPERTY_FLOAT) tram::Platform::TryDebugging(); assert(type == PROPERTY_FLOAT); return float_value; }
    operator int32_t() const { assert(type == PROPERTY_INT || type == PROPERTY_ENUM); return int_value; }
    operator uint32_t() const { assert(type == PROPERTY_UINT || type == PROPERTY_FLAG); return uint_value; }
    operator bool() const { assert(type == PROPERTY_BOOL); return bool_value; }
    operator tram::vec3() const { assert(type == PROPERTY_VECTOR || type == PROPERTY_DIRECTION || type == PROPERTY_ORIGIN); return vector_value; }
    
    PropertyType type;
    union {
        std::string str_value;
        float float_value;
        int32_t int_value;
        uint32_t uint_value;
        bool bool_value;
        tram::vec3 vector_value;
    };
};

struct WidgetDefinition {
    enum Color {
        WIDGET_WHITE,
        WIDGET_CYAN,
        WIDGET_GREEN,
    };
    
    enum Type {
        WIDGET_POINT,
        WIDGET_NORMAL,
        WIDGET_SELECTION_BOX,
        WIDGET_SELECTION_AABB,
        
        WIDGET_LINE,
    };
    
    inline static WidgetDefinition Line(tram::vec3 from, tram::vec3 to, Color color) {
        return {.color = color, .type = WIDGET_LINE, .value1 = from, .value2 = to};
    }
    
    inline static WidgetDefinition SelectionBox(tram::vec3 size, Color color) {
        return {.color = color, .type = WIDGET_SELECTION_BOX, .value1 = size};
    }
    
    inline static WidgetDefinition SelectionAABB(tram::vec3 min, tram::vec3 max, Color color) {
        return {.color = color, .type = WIDGET_SELECTION_AABB, .value1 = min, .value2 = max};
    }
    
    Color color;
    Type type;
    
    std::string property;
    
    PropertyValue value1;
    PropertyValue value2;
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
    std::shared_ptr<Object> GetParent() { return parent ? parent->GetPointer() : nullptr; }
    std::shared_ptr<Object> GetCopy() { return GetPointer(); } // TODO: implement copying
    
    auto GetProperties() { return properties; }
    void SetProperties(auto properties) { this->properties = properties; }
    
    virtual std::string_view GetName() { return properties["name"].str_value; }
    
    // object hierarchy operations
    virtual void AddChild(std::shared_ptr<Object> child) { children.push_back(child); }
    virtual void RemoveChild(std::shared_ptr<Object> child) { children.remove(child); }
    virtual bool IsChildAddable(std::shared_ptr<Object> child) { std::cout << "IsChildAddable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual std::list<std::shared_ptr<Object>> GetChildren() { return children; }

    virtual bool IsChildrenTreeable() { std::cout << "IsChildrenTreeable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual bool IsChildrenListable() { std::cout << "IsChildrenListable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual void SetHidden(bool is_hidden) { for (auto& child : children) child->SetHidden(is_hidden); }
    virtual std::shared_ptr<Object> AddChild() { std::cout << "AddChild(void) not implemented for " << typeid(*this).name() << std::endl; return std::shared_ptr<Object>(nullptr); }

    // graph object operations
    virtual std::shared_ptr<Object> Extrude() { std::cout << "Extrude() not implemented for " << typeid(*this).name() << std::endl; return std::shared_ptr<Object>(nullptr); }
    virtual void Connect(std::shared_ptr<Object> other) { std::cout << "Connect() not implemented for " << typeid(*this).name() << std::endl; }
    virtual void Disconnect(std::shared_ptr<Object> other) { std::cout << "Disconnect() not implemented for " << typeid(*this).name() << std::endl; }
    virtual bool IsConnected(std::shared_ptr<Object> other) { std::cout << "IsConnected() not implemented for " << typeid(*this).name() << std::endl; return false; }

    bool is_hidden = true;
    Object* parent = nullptr;
    
    // operation checks
    virtual bool IsAddable() { std::cout << "IsAddable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual bool IsRemovable() { std::cout << "IsRemovable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual bool IsEditable() { std::cout << "IsEditable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual bool IsCopyable() { std::cout << "IsCopyable() not implemented for " << typeid(*this).name() << std::endl; return false; }
    virtual bool IsHidden() { for (auto& child : children) if (child->IsHidden()) return true; return false; }
    
    virtual std::shared_ptr<Object> Duplicate() { std::cout << "Duplicate(void) not implemented for " << typeid(*this).name() << std::endl; return std::shared_ptr<Object>(nullptr); }
    
    virtual bool IsWidgetedWithChildren() { return false; }
    virtual std::vector<WidgetDefinition> GetWidgetDefinitions() { return {}; }
    
    // these are the properties that will be shown in the object list
    virtual std::vector<PropertyDefinition> GetListPropertyDefinitions() { std::cout << "GetListPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; return {}; }
    
    // these properties will be shown and will be editable from property panel
    virtual std::vector<PropertyDefinition> GetFullPropertyDefinitions() { std::cout << "GetFullPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; return {}; }
    
    // these properties will be used for serialization
    virtual std::vector<PropertyDefinition> GetSerializationPropertyDefinitions() { std::cout << "GetSerializationPropertyDefinitions() not implemented for " << typeid(*this).name() <<  std::endl; return {}; }
    
    virtual PropertyValue GetProperty (std::string property_name) { return properties[property_name]; }
    virtual void SetProperty (std::string property_name, PropertyValue property_value) { properties[property_name] = property_value; if (property_name == "name" && parent && parent->IsChildrenTreeable()) WorldTree::Rename(this); }
    virtual bool HasProperty (std::string property_name) { return properties.contains(property_name); }
};

struct EntityDefinition {
    uint32_t version;
    
    std::string name;
    std::string model_name;
    
    std::vector<PropertyDefinition> definitions;
    std::vector<WidgetDefinition> widgets;
    std::vector<std::pair<std::string, PropertyValue>> default_properties;
};

void RegisterEntityType(EntityDefinition definition);

class WorldCell;
void LoadCell(WorldCell* cell);
void SaveCell(WorldCell* cell);

struct Selection {
    std::list<std::shared_ptr<Object>> objects;
};

extern bool data_modified;
extern std::shared_ptr<Selection> SELECTION;
extern std::shared_ptr<Selection> STASH;

}

#endif // EDITOR_H