#include <editor/objects/entity.h>

#include <components/rendercomponent.h>

namespace Editor {

/// Instances of SerializedEntityData for all registered Entity types.
/// For determining which specialized fields each Entity type has.
static std::vector<tram::SerializedEntityData*> entity_data_instances = { nullptr };

/// Registers an Entity type.
/// Pass in a blank instance of a SerializedEntityData for each Entity type
/// that you want to be editable in the editor. It needs to be allocated
/// with new and needs to live until the termination of the program.
void RegisterEntityType(tram::SerializedEntityData* instance) {        
    entity_data_instances.push_back(instance);
    PROPERTY_ENUMERATIONS["entity-type"].push_back(instance->GetType());
}

PropertyValue Entity::GetProperty (std::string property_name) { return properties[property_name]; }

void Entity::SetProperty (std::string property_name, PropertyValue property_value) {
    properties[property_name] = property_value;
    
    Entity::CheckModel();
}

void Entity::CheckModel() {
    if (this->is_hidden) {
        return;
    }
    
    using namespace tram;
    
    std::string model_field_name = "";
    
    int32_t entity_type = this->GetProperty("entity-type");
    for (auto& prop : entity_data_instances[entity_type]->GetFieldInfo()) {
        if (prop.type == tram::SerializedEntityData::FieldInfo::FIELD_MODELNAME) {
            model_field_name = prop.name;
        }
    }
    
    if (model_field_name == "") {
        return;
    }
    
    PropertyValue model_name_value = this->GetProperty(model_field_name);
    
    if (model_name_value.type != PROPERTY_STRING) {
        return;
    }
    
    std::string model_name = model_name_value;
    
    if (!this->model) {
        std::cout << "making rendercomp for " << model_name << std::endl;
        this->model = PoolProxy<RenderComponent>::New();
        this->model->SetModel(model_name);
        this->model->SetLightmap("fullbright");
        //this->model->SetPose(Render::poseList.begin().ptr);
        this->model->Init();
    }
    
    if (this->model && model_name != this->model->GetModel()) {
        std::cout << "deleting rendercomp for " << model_name << std::endl;
        PoolProxy<RenderComponent>::Delete(this->model);
        this->model = nullptr;
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
    
    auto special_defs = GetSerializedEntityDataDefinitions();
    
    defs.insert(defs.end(), special_defs.begin(), special_defs.end());
    
    return defs;
}

/// Gets the SerializedEntityData definitions.
/// Converts the SerializedEntityData definitions from the framework format into
/// editor format. If there is no definition available for the, this method will
/// return an empty definition vector.
std::vector<PropertyDefinition> Entity::GetSerializedEntityDataDefinitions() {
    std::vector<PropertyDefinition> defs;
    size_t entity_type_index = (int32_t) this->GetProperty("entity-type");
    
    assert(entity_type_index < entity_data_instances.size());
    
    if (entity_type_index) {
        defs.push_back({"group-entity-special", entity_data_instances[entity_type_index]->GetType(), "", PROPERTY_CATEGORY});
        
        for (auto& prop : entity_data_instances[entity_type_index]->GetFieldInfo()) {
            switch (prop.type) {
                case tram::SerializedEntityData::FieldInfo::FIELD_INT:
                    defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_INT});
                    break;
                case tram::SerializedEntityData::FieldInfo::FIELD_UINT:
                    defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_UINT});
                    break;
                case tram::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                    defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_FLOAT});
                    break;
                case tram::SerializedEntityData::FieldInfo::FIELD_STRING:
                case tram::SerializedEntityData::FieldInfo::FIELD_MODELNAME:
                    defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_STRING});
                    break;
            }
        }
    }
    
    return defs;
}

/// Sets the entity type from registred types.
void Entity::SetEntityType (std::string type) {
    // need to find the index of the entity type
    size_t entity_type_index = 0;
    for (size_t i = 1; i < entity_data_instances.size(); i++) {
        //std::cout << "comp: " << entity_data_instances[i]->GetType() << " " << type << " " << (entity_data_instances[i]->GetType() == type) << std::endl;
        if (entity_data_instances[i]->GetType() == type) {
            entity_type_index = i;
            break;
        }
    }

    // TODO: find a way to nicely abort the program
    assert(entity_type_index);
    assert(entity_type_index < entity_data_instances.size());
    
    // need the int32_t cast so that PropertyValue gets initialized to an enum type
    this->SetProperty("entity-type", (int32_t) entity_type_index);
}

}