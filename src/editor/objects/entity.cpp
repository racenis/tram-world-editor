#include <editor/objects/entity.h>

#include <components/render.h>

#include <random>
#include <set>

namespace Editor {

using namespace tram;

static std::unordered_map<int32_t, std::vector<EntityDefinition>> entity_type_infos;
static std::unordered_map<std::string, int32_t> entity_name_to_id;
static std::unordered_map<RenderComponent*, Entity*> viewmodel_ptr_to_entity_ptr;

static std::set<uint32_t> used_ids; 

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

void Entity::CenterOrigin() {
    //PropertyValue::Vector vec;
    vec3 vec;
    
    if (!model) {
        vec.x = GetProperty("position-x");
        vec.y = GetProperty("position-y");
        vec.z = GetProperty("position-z");
    } else {
        vec3 origin_a = model->GetModel()->GetAABBMin();
        vec3 origin_b = model->GetModel()->GetAABBMax();
        
        vec3 origin = glm::mix(origin_a, origin_b, 0.5f);
        
        vec.x = origin.x;
        vec.y = origin.y;
        vec.z = origin.z;
    }
    
    SetProperty("origin", vec);
}

void Entity::WorldspawnOffset(Entity* worldspawn) {
    if (!model) {
        std::cout << "Entity has no model to base worldspawn offset off of!" << std::endl;
        return;
    }
    
    std::cout << "settings!!!" << std::endl;
    std::cout << "ee " << model->GetModel()->GetOrigin().x<< std::endl;
    std::cout << "ee " << model->GetModel()->GetOrigin().y<< std::endl;
    std::cout << "ee " << model->GetModel()->GetOrigin().z<< std::endl;
    
    vec3 worldspawn_pos = vec3 {worldspawn->GetProperty("position-x"),
                                worldspawn->GetProperty("position-y"),
                                worldspawn->GetProperty("position-z")};
    quat worldspawn_rot = vec3 {worldspawn->GetProperty("rotation-x"),
                                worldspawn->GetProperty("rotation-y"),
                                worldspawn->GetProperty("rotation-z")};
                                
    vec3 new_position = worldspawn_pos + worldspawn_rot * model->GetModel()->GetOrigin();
                       
    SetProperty("position-x", new_position.x);
    SetProperty("position-y", new_position.y);
    SetProperty("position-z", new_position.z);
    
    SetProperty("rotation-x", worldspawn->GetProperty("rotation-x"));
    SetProperty("rotation-y", worldspawn->GetProperty("rotation-y"));
    SetProperty("rotation-z", worldspawn->GetProperty("rotation-z"));
}

uint32_t GetEntityTypeVersion(int32_t type_id) {
    uint32_t highest_version = 0;
    
    for (auto& definition : entity_type_infos[type_id]) {
        if (definition.version > highest_version)  {
            highest_version = definition.version;
        }
    }
    
    return highest_version;
}

EntityDefinition* FindEntityDefinition(uint32_t version, int32_t type_id) {
    for (auto& definition : entity_type_infos[type_id]) {
        if (definition.version != version) continue;
        return &definition;
    }
    
    return nullptr;
}

EntityDefinition* FindEntityDefinition(int32_t type_id) {
    return FindEntityDefinition(GetEntityTypeVersion(type_id), type_id);
}

void RegisterEntityType(EntityDefinition definition) {
    int32_t type_index = PROPERTY_ENUMERATIONS["entity-type"].size();
    PROPERTY_ENUMERATIONS["entity-type"].push_back(definition.name);
    
    entity_name_to_id[definition.name] = type_index;
    
    entity_type_infos[type_index].push_back(definition);
}

PropertyValue Entity::GetProperty (std::string property_name) {
    return properties[property_name];
}

void Entity::SetProperty (std::string property_name, PropertyValue property_value) {
    properties[property_name] = property_value;
    
    // stupid hack, but better than generating id collisions
    if (property_name=="id" && (uint32_t)property_value) {
        if (used_ids.contains(property_value)) {
             GenerateNewRandomId();
        } else {
            used_ids.insert(property_value);
        }
    }
    
    if (property_name == "entity-type") {
        InitDefaultPropertyValues();
        Editor::PropertyPanel::Refresh();
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
    //EntityDefinition& type_info = entity_type_infos[this->GetProperty("entity-type")];
    EntityDefinition* type_def = FindEntityDefinition(this->GetProperty("entity-type"));
    if (!type_def) return;
    
    EntityDefinition& type_info = *type_def;
    
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
        //std::cout << "making rendercomp for " << model_name << std::endl;
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

std::vector<WidgetDefinition> Entity::GetWidgetDefinitions() {
    auto entity_defs = FindEntityDefinition((int32_t) this->GetProperty("entity-type"));
    
    if (entity_defs) return entity_defs->widgets;
    return std::vector<WidgetDefinition>();
}

std::vector<PropertyDefinition> Entity::GetFullPropertyDefinitions() { 
    auto defs = std::vector<PropertyDefinition> {
        {"group-entity", "Entity", "", PROPERTY_CATEGORY},
        {"id", "ID", "group-entity", PROPERTY_UINT},
        {"name", "Name", "group-entity", PROPERTY_STRING},
        {"entity-flags", "Flags", "group-entity", PROPERTY_FLAG},
        {"group-position", "Position", "", PROPERTY_CATEGORY},
        {"position-x", "X", "group-position", PROPERTY_FLOAT},
        {"position-y", "Y", "group-position", PROPERTY_FLOAT},
        {"position-z", "Z", "group-position", PROPERTY_FLOAT},
        {"group-rotation", "Rotation", "", PROPERTY_CATEGORY},
        {"rotation-x", "X", "group-rotation", PROPERTY_FLOAT},
        {"rotation-y", "Y", "group-rotation", PROPERTY_FLOAT},
        {"rotation-z", "Z", "group-rotation", PROPERTY_FLOAT},
        {"entity-type", "Entity Type", "group-entity", PROPERTY_ENUM},
        {"group-entity-specific", "Type", "", PROPERTY_CATEGORY},
    };
    
    // add entity type specific definitions, it it has any
    auto entity_def = FindEntityDefinition((int32_t) this->GetProperty("entity-type"));
    if (entity_def) {
        defs.insert(defs.end(), entity_def->definitions.begin(), entity_def->definitions.end());
    }
    
    return defs;
}

std::vector<PropertyDefinition> Entity::GetSerializationPropertyDefinitions() {
    return FindEntityDefinition((int32_t) this->GetProperty("entity-type"))->definitions;
}

std::vector<PropertyDefinition> Entity::GetSerializationPropertyDefinitions(uint32_t version) {
    return FindEntityDefinition(version, (int32_t) this->GetProperty("entity-type"))->definitions;
}

/// Sets the entity type from registred types.
void Entity::SetEntityType (std::string type) {
    if (entity_name_to_id.find(type) == entity_name_to_id.end()) {
        Editor::Viewport::ShowErrorDialog("Entity type \"" + type + "\" not registered!");
        abort();
    }
    
    this->SetProperty("entity-type", (int32_t) entity_name_to_id[type]);
}

void Entity::InitDefaultPropertyValues() {
    auto definition = FindEntityDefinition((int32_t) this->GetProperty("entity-type"));
    
    for (size_t i = 0; i < definition->definitions.size(); i++) {
        assert(definition->definitions[i].name == definition->default_properties[i].first);
        
        auto property_name = definition->definitions[i].name;
        auto property_value = definition->default_properties[i].second;
        
        auto current = this->GetProperty(property_name);
        
        if (current.type != PROPERTY_NULL) continue;
        
        this->SetProperty(property_name, property_value);
    } 
}

}