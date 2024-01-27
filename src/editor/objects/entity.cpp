#include <editor/objects/entity.h>

#include <components/render.h>

#include <random>
#include <set>

namespace Editor {

using namespace tram;

struct EntityTypeInfo {
    std::string model_name;
    std::vector<PropertyDefinition> definition;
};

static std::unordered_map<int32_t, EntityTypeInfo> entity_type_infos;
static std::unordered_map<std::string, int32_t> entity_name_to_id;
static std::unordered_map<RenderComponent*, Entity*> viewmodel_ptr_to_entity_ptr;

static std::set<uint64_t> used_ids; 

std::shared_ptr<Object> Entity::Duplicate() {
    auto dupe = std::dynamic_pointer_cast<Editor::Entity>(parent->AddChild());
    dupe->properties = properties;
    dupe->signals = signals;
    if ((std::string)dupe->properties["name"] != "none")
        dupe->properties["name"] = (std::string)dupe->properties["name"] + "-duplicate";
    dupe->GenerateNewRandomId();
    return dupe;
}

void Entity::GenerateNewRandomId() {
    std::random_device device;
    std::mt19937 randomizer(device());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(100000000, 999999999);

    this->SetProperty("id", (uint64_t)distribution(randomizer));
}

Entity* GetEntityFromViewmodel(tram::RenderComponent* model) {
    return viewmodel_ptr_to_entity_ptr[model];
}

void Entity::Draw() {
    if (model) {
        Render::AddLineAABB(
            model->GetModel()->GetAABBMin(),
            model->GetModel()->GetAABBMax(),
            model->GetLocation(),
            model->GetRotation(),
            Render::COLOR_GREEN
        );
    }
}

void RegisterEntityType(std::string name, std::string model_name, std::vector<PropertyDefinition> definitions) {
    int32_t type_index = PROPERTY_ENUMERATIONS["entity-type"].size();
    PROPERTY_ENUMERATIONS["entity-type"].push_back(name);
    
    entity_name_to_id[name] = type_index;
    
    entity_type_infos[type_index] = {model_name, definitions};
}

PropertyValue Entity::GetProperty (std::string property_name) {
    return properties[property_name];
}

void Entity::SetProperty (std::string property_name, PropertyValue property_value) {
    properties[property_name] = property_value;
    
    // stupid hack, but better than generating id collisions
    if (property_name=="id" && (uint64_t)property_value) {
        if (used_ids.contains(property_value)) {
             GenerateNewRandomId();
        } else {
            used_ids.insert(property_value);
        }
    }
    
    Entity::CheckModel();
}

void Entity::CheckModel() {
    
    // check if model needs to be hidden
    if (this->is_hidden) {
        if (this->model) {
            PoolProxy<RenderComponent>::Delete(this->model);
            
            viewmodel_ptr_to_entity_ptr[this->model] = nullptr;
            
            this->model = nullptr;
        }
        
        return;
    }
    
    
    // find the name of the model
    EntityTypeInfo& type_info = entity_type_infos[this->GetProperty("entity-type")];
    
    std::string model_name = type_info.model_name;
    
    if (model_name == "none") {
        PropertyValue model_name_value = this->GetProperty("model");
    
        if (model_name_value.type != PROPERTY_STRING) {
            std::cout << "found model name, but its not string" << std::endl;
            return;
        }
        
        model_name = (std::string) model_name_value;
    }
    
    if (model_name == "" || model_name == "none") {
        return;
    }    
    
    if (!this->model) {
        std::cout << "making rendercomp for " << model_name << std::endl;
        this->model = PoolProxy<RenderComponent>::New();
        this->model->SetModel(model_name);
        this->model->SetLightmap("fullbright");
        //this->model->SetPose(Render::poseList.begin().ptr);
        this->model->Init();
        
        viewmodel_ptr_to_entity_ptr[this->model] = this;
    }
    
    if (this->model && model_name != this->model->GetModel()->GetName()) {
        std::cout << "deleting rendercomp for " << model_name << std::endl;
        PoolProxy<RenderComponent>::Delete(this->model);
        this->model = nullptr;
        viewmodel_ptr_to_entity_ptr[this->model] = nullptr;
        CheckModel();
        return;
    }
    
    this->model->SetRotation(quat(vec3(
        this->GetProperty("rotation-x").float_value,
        this->GetProperty("rotation-y").float_value,
        this->GetProperty("rotation-z").float_value
    )));
    
    this->model->SetLocation(vec3(
        this->GetProperty("position-x").float_value,
        this->GetProperty("position-y").float_value,
        this->GetProperty("position-z").float_value
    ));
}

void Entity::SetHidden(bool is_hidden) {
    this->is_hidden = is_hidden;
    
    Entity::CheckModel();
}

std::vector<PropertyDefinition> Entity::GetFullPropertyDefinitions() { 
    auto defs = std::vector<PropertyDefinition> {
        {"group-entity", "Entity", "", PROPERTY_CATEGORY},
        {"id", "ID", "group-entity", PROPERTY_UINT},
        {"name", "Name", "group-entity", PROPERTY_STRING},
        {"group-position", "Position", "", PROPERTY_CATEGORY},
        {"position-x", "X", "group-position", PROPERTY_FLOAT},
        {"position-y", "Y", "group-position", PROPERTY_FLOAT},
        {"position-z", "Z", "group-position", PROPERTY_FLOAT},
        {"group-rotation", "Rotation", "", PROPERTY_CATEGORY},
        {"rotation-x", "X", "group-rotation", PROPERTY_FLOAT},
        {"rotation-y", "Y", "group-rotation", PROPERTY_FLOAT},
        {"rotation-z", "Z", "group-rotation", PROPERTY_FLOAT},
        {"entity-type", "Entity Type", "group-entity", PROPERTY_ENUM}
    };
    
    auto special_defs = entity_type_infos[(int32_t) this->GetProperty("entity-type")].definition;
    
    defs.insert(defs.end(), special_defs.begin(), special_defs.end());
    
    return defs;
}

std::vector<PropertyDefinition> Entity::GetSerializationPropertyDefinitions() {
    return entity_type_infos[(int32_t) this->GetProperty("entity-type")].definition;
}

/// Sets the entity type from registred types.
void Entity::SetEntityType (std::string type) {
    if (entity_name_to_id.find(type) == entity_name_to_id.end()) {
        Editor::Viewport::ShowErrorDialog("Entity type \"" + type + "\" not registered!");
        abort();
    }
    
    this->SetProperty("entity-type", (int32_t) entity_name_to_id[type]);
}

}