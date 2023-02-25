#include <editor.h>
#include <language.h>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <components/rendercomponent.h>

#include <entities/crate.h>
#include <entities/lamp.h>
#include <entities/staticworldobject.h>
//#include <moshkis.h>
//#include <pickup.h>

namespace Editor {
    // we'll need to move all of this entity handling stuff to a seperate file
    /*struct EntityData {
        Core::SerializedEntityData* data = nullptr; // TODO: rename to serialized_data
        Core::RenderComponent* render_component = nullptr;
        
        void PropertiesToSerializedData (Entity* entity) {
            assert(data);
            
            auto props = data->GetEditorFieldInfo();
            
            for (auto& prop : props) {
                auto prop_val = entity->GetProperty(prop.data_name);
                if (prop_val.type == PROPERTY_NULL) continue;
                
                switch (prop.type) {
                    case Core::SerializedEntityData::FieldInfo::FIELD_UINT64:
                        *(uint64_t*) prop.field = prop_val.uint_value;
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                        *(float*) prop.field = prop_val.float_value;
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_NAME:
                        *(Core::name_t*) prop.field = Core::UID(prop_val.str_value);
                        break;
                }
            }
        }
        
        void SerializedDataToProperties (Entity* entity) {
            assert(data);
            
            auto props = data->GetEditorFieldInfo();
            
            for (auto& prop : props) {
                switch (prop.type) {
                    // we're setting the properties directly, because otherwise
                    case Core::SerializedEntityData::FieldInfo::FIELD_UINT64:
                        entity->properties[prop.data_name] =  *(uint64_t*) prop.field;
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                        entity->properties[prop.data_name] =  *(float*) prop.field;
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_NAME:
                        entity->properties[prop.data_name] = std::string(Core::ReverseUID(*(Core::name_t*) prop.field));
                        break;
                }
            }
        }
        
        void InitRenderComponent (Entity* entity) {
            render_component = Core::PoolProxy<Core::RenderComponent>::New();
            render_component->UpdateRotation(glm::quat(glm::vec3(
                entity->properties["rotation-x"].float_value,
                entity->properties["rotation-y"].float_value,
                entity->properties["rotation-z"].float_value
            )));
            render_component->UpdateLocation(glm::vec3(
                entity->properties["position-x"].float_value,
                entity->properties["position-y"].float_value,
                entity->properties["position-z"].float_value
            ));
            render_component->SetModel(data->GetEditorModel());
            render_component->SetLightmap(Core::UID("fullbright"));
            render_component->SetPose(Core::Render::poseList.begin().ptr);
            render_component->Init();
        }
        
        void UpdateRenderComponent (Entity* entity) {
            if (!render_component) {
                InitRenderComponent (entity);
                return;
            }
            
            if (data->GetEditorModel() != render_component->GetModel()) {
                render_component->SetModel(data->GetEditorModel());
            }
            
            render_component->UpdateRotation(glm::quat(glm::vec3(
                entity->properties["rotation-x"].float_value,
                entity->properties["rotation-y"].float_value,
                entity->properties["rotation-z"].float_value
            )));
            render_component->UpdateLocation(glm::vec3(
                entity->properties["position-x"].float_value,
                entity->properties["position-y"].float_value,
                entity->properties["position-z"].float_value
            ));
        }
        
        void RemoveRenderComponent (Entity* entity) {
            render_component->Uninit();
            Core::PoolProxy<Core::RenderComponent>::Delete(render_component);
            render_component = nullptr;
        }
    };*/
    
    // this is kinda out of place
    std::shared_ptr<Selection> selection;
    std::shared_ptr<WorldCellManager> worldcells;
    std::list<std::unique_ptr<Action>> performed_actions;
    std::list<std::unique_ptr<Action>> unperformed_actions;
    bool data_modified = false;
    
    namespace Settings {
        Space TRANSFORM_SPACE = SPACE_WORLD;
        Rotation ROTATION_UNIT = ROTATION_RADIANS;
        bool CELL_LIST_FROM_FILESYSTEM = true;
        Language INTERFACE_LANGUAGE = LANGUAGE_EN;
    }
    
    // TODO: maybe change this to all-caps?
    std::unordered_map<std::string, std::vector<std::string>> property_enumerations = {
        {"entity-type", {"[none]"}}
    };
    
    //std::unordered_map<size_t, Core::SerializedEntityData* (*)(void)> entity_data_constructors;
    //std::vector<std::string> entity_data_names = {"none"};
    
    /// Instances of SerializedEntityData for all registered Entity types.
    /// For determining which specialized fields each Entity type has.
    std::vector<Core::SerializedEntityData*> ENTITY_DATA_INSTANCES = { nullptr };
    
    /// Registers an Entity type.
    /// Pass in a blank instance of a SerializedEntityData for each Entity type
    /// that you want to be editable in the editor. It needs to be allocated
    /// with new and needs to live until the termination of the program.
    void RegisterEntityType(Core::SerializedEntityData* instance) {
        //auto instance = constructor();
        
        //auto index = property_enumerations["entity-type"].size();
        //property_enumerations["entity-type"].push_back(instance->GetEditorName());
        //entity_data_names.push_back(instance->GetDataName());
        //entity_data_constructors[index] = constructor;
        
        //delete instance;
        
        /*auto index = ENTITY_DATA_INSTANCES.size();*/
        
        ENTITY_DATA_INSTANCES.push_back(instance);
        property_enumerations["entity-type"].push_back(instance->GetType());
    }
    
    PropertyValue Entity::GetProperty (std::string property_name) { return properties[property_name]; }
    
    void Entity::SetProperty (std::string property_name, PropertyValue property_value) {
        properties[property_name] = property_value;
        
        Entity::CheckModel();
        /*
        if (property_name == "name") {
            properties["name"] = property_value;
        } else if (property_name == "position-x") {
            properties["position-x"] = property_value;
        } else if (property_name == "position-y") {
            properties["position-y"] = property_value;
        } else if (property_name == "position-z") {
            properties["position-z"] = property_value;
        } else if (property_name == "rotation-x") {
            properties["rotation-x"] = property_value;
        } else if (property_name == "rotation-y") {
            properties["rotation-y"] = property_value;
        } else if (property_name == "rotation-z") {
            properties["rotation-z"] = property_value;
        } else if (property_name == "entity-type") {
            properties["entity-type"] = property_value;
            
            // if you set the entity type to "none"
            if (property_value.enum_value == 0) {
                if (entity_data) {
                    // TODO: move these checks into the destructor of the entity_data
                    if (entity_data->render_component) {
                        entity_data->RemoveRenderComponent(this);
                    }
                    
                    if (entity_data->data) {
                        delete entity_data->data;
                    }
                    
                    delete entity_data;
                    entity_data = nullptr;
                }
                return;
            }
            
            // if you set the entity type to an actual type
            if (!entity_data) entity_data = new EntityData;
            if (entity_data->data) delete entity_data->data;
            entity_data->data = entity_data_constructors[property_value.enum_value]();
        } else {
            properties[property_name] = property_value;
        }
        

        if (entity_data && !is_hidden) {
            entity_data->PropertiesToSerializedData(this);
            entity_data->UpdateRenderComponent(this);
        }*/
    }
    
    void Entity::CheckModel() {
        if (this->is_hidden) {
            return;
        }
        
        using namespace Core;
        
        std::string model_field_name = "";
        
        int32_t entity_type = this->GetProperty("entity-type");
        for (auto& prop : ENTITY_DATA_INSTANCES[entity_type]->GetFieldInfo()) {
            if (prop.type == Core::SerializedEntityData::FieldInfo::FIELD_MODELNAME) {
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
            this->model->SetPose(Render::poseList.begin().ptr);
            this->model->Init();
        }
        
        if (this->model && model_name != this->model->GetModel()) {
            std::cout << "deleting rendercomp for " << model_name << std::endl;
            PoolProxy<RenderComponent>::Delete(this->model);
            this->model = nullptr;
            CheckModel();
            return;
        }
        
        this->model->UpdateRotation(quat(vec3(
            this->GetProperty("rotation-x").float_value,
            this->GetProperty("rotation-y").float_value,
            this->GetProperty("rotation-z").float_value
        )));
        
        this->model->UpdateLocation(vec3(
            this->GetProperty("position-x").float_value,
            this->GetProperty("position-y").float_value,
            this->GetProperty("position-z").float_value
        ));
    }
    
    void Entity::SetHidden(bool is_hidden) {
        this->is_hidden = is_hidden;
        
        Entity::CheckModel();
        
        /*
        if (is_hidden && entity_data && entity_data->render_component) {
            entity_data->RemoveRenderComponent(this);
        }
        
        if (!is_hidden && entity_data && !entity_data->render_component) {
            entity_data->PropertiesToSerializedData(this);
            entity_data->UpdateRenderComponent(this);
        }*/
    }
    
    std::vector<PropertyDefinition> Entity::GetFullPropertyDefinitions() { 
        auto defs = std::vector<PropertyDefinition> {
            {"group-entity", "Entity", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-entity", PROPERTY_STRING},
            {"action", "Action", "group-entity", PROPERTY_STRING},
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
        
        int32_t entity_type_index = this->GetProperty("entity-type");
        
        if (entity_type_index) {
            defs.push_back({"group-entity-special", ENTITY_DATA_INSTANCES[entity_type_index]->GetType(), "", PROPERTY_CATEGORY});
            
            for (auto& prop : ENTITY_DATA_INSTANCES[entity_type_index]->GetFieldInfo()) {
                switch (prop.type) {
                    case Core::SerializedEntityData::FieldInfo::FIELD_INT:
                        defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_INT});
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_UINT:
                        defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_UINT});
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                        defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_FLOAT});
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_STRING:
                    case Core::SerializedEntityData::FieldInfo::FIELD_MODELNAME:
                        defs.push_back({prop.name, prop.name, "group-entity-special", PROPERTY_STRING});
                        break;
                }
            }
        }
        
        return defs;
    }
    
    /// Definitions of all Settings.
    /// They're only used for the Settings::Load() and Settings::Save() functions.
    std::unordered_map<std::string, std::pair<PropertyType, void*>> setting_map = {
        {"TRANSFORM_SPACE", {PROPERTY_ENUM, &Settings::TRANSFORM_SPACE}},
        {"ROTATION_UNIT", {PROPERTY_ENUM, &Settings::ROTATION_UNIT}},
        {"CELL_LIST_FROM_FILESYSTEM", {PROPERTY_BOOL, &Settings::CELL_LIST_FROM_FILESYSTEM}},
        {"INTERFACE_LANGUAGE", {PROPERTY_ENUM, &Settings::INTERFACE_LANGUAGE}}
    };
    
    /// Reads the Settings from disk.
    void Settings::Load() {
        std::ifstream file ("data/editor_settings.ini");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                size_t eq_pos = line.find('=');
                if (eq_pos == std::string::npos) continue;
                
                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(++eq_pos);
                
                auto& bepis = setting_map[std::string(key)];
                if (!bepis.second) continue;
                switch (bepis.first) {
                    case PROPERTY_ENUM:
                        *(uint32_t*) bepis.second = std::stoull(value);
                        break;
                    case PROPERTY_BOOL:
                        *(bool*) bepis.second = std::stoull(value);;
                        break;
                    default:
                        break;
                }
            }
        } else {
            std::wcout << selected_language->dialog_settings_not_found << std::endl;
        }
    }
    
    /// Writes the Settings to disk.
    void Settings::Save() {
        std::ofstream file ("data/editor_settings.ini");
        
        if (file.is_open()) {
            for (auto& entry : setting_map) {
                file << entry.first << "=";
                switch (entry.second.first) {
                    case PROPERTY_ENUM:
                        file << *(uint32_t*) entry.second.second;
                    break;
                    case PROPERTY_BOOL:
                        file << *(bool*) entry.second.second;
                    break;
                    default:
                        file << "0";
                }
                file << std::endl;
            }
        }
    }
    
    
    // this function is absolutely terrible!
    // TODO: wrap Core::SerializedEntityData::Field<TTT>().FromString(STR) function
    void LoadCell(WorldCell* cell) {
        using namespace Core;
        std::ifstream file(std::string("data/") + std::string(cell->GetName()) + ".cell");
        EntityGroup* current_group = (EntityGroup*)cell->group_manager->GetChildren().front().get();
        //assert(current_group->GetName() == "[default]");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (line.size() < 3) continue;
                std::string_view str (line);
                std::string ent_name = Core::SerializedEntityData::Field<Core::name_t>().FromString(str);
                if (ent_name == "#") {
                    std::string annotation_name = Core::SerializedEntityData::Field<Core::name_t>().FromString(str);
                    if (annotation_name == "cell") {
                        cell->SetProperty("is-interior", (bool) Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        cell->SetProperty("is-interior-lighting", (bool) Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                    } else if (annotation_name == "group") {
                        auto new_group = current_group->parent->AddChild();
                        new_group->SetProperty("name", std::string(Core::SerializedEntityData::Field<Core::name_t>().FromString(str)));
                        current_group = (EntityGroup*) new_group.get();
                    }
                } else if (ent_name == "transition") {
                    auto trans = std::make_shared<Transition>(cell->transition_manager.get());
                    trans->SetProperty("name", std::string(Core::SerializedEntityData::Field<Core::name_t>().FromString(str)));
                    trans->SetProperty("cell-into", std::string(Core::SerializedEntityData::Field<Core::name_t>().FromString(str)));
                    uint64_t point_count = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    for (uint64_t i = 0; i < point_count; i++) {
                        auto p = trans->AddChild();
                        p->SetProperty("position-x", Core::SerializedEntityData::Field<float>().FromString(str));
                        p->SetProperty("position-y", Core::SerializedEntityData::Field<float>().FromString(str));
                        p->SetProperty("position-z", Core::SerializedEntityData::Field<float>().FromString(str));
                    }
                    static_cast<Object*>(cell->transition_manager.get())->AddChild(trans);
                } else if (ent_name == "path") {
                    auto path = std::make_shared<Path>(cell->path_manager.get());
                    path->SetProperty("name", std::string(Core::SerializedEntityData::Field<Core::name_t>().FromString(str)));
                    uint64_t curve_count = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    for (uint64_t i = 0; i < curve_count; i++) {
                        auto c = path->AddChild();
                        c->SetProperty("id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        c->SetProperty("next-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        c->SetProperty("prev-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        c->SetProperty("left-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        c->SetProperty("right-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        c->SetProperty("position-x", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-y", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-z", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-x-2", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-y-2", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-z-2", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-x-3", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-y-3", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-z-3", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-x-4", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-y-4", Core::SerializedEntityData::Field<float>().FromString(str));
                        c->SetProperty("position-z-4", Core::SerializedEntityData::Field<float>().FromString(str));
                    }
                    static_cast<Object*>(cell->path_manager.get())->AddChild(path);
                } else if (ent_name == "navmesh") {
                    auto navmesh = std::make_shared<Path>(cell->navmesh_manager.get());
                    navmesh->SetProperty("name", std::string(Core::SerializedEntityData::Field<Core::name_t>().FromString(str)));
                    uint64_t node_count = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    for (uint64_t i = 0; i < node_count; i++) {
                        auto n = navmesh->AddChild();
                        n->SetProperty("id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        n->SetProperty("next-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        n->SetProperty("prev-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        n->SetProperty("left-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        n->SetProperty("right-id", Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        n->SetProperty("position-x", Core::SerializedEntityData::Field<float>().FromString(str));
                        n->SetProperty("position-y", Core::SerializedEntityData::Field<float>().FromString(str));
                        n->SetProperty("position-z", Core::SerializedEntityData::Field<float>().FromString(str));
                    }
                    static_cast<Object*>(cell->navmesh_manager.get())->AddChild(navmesh);
                } else {
                    auto entity = std::make_shared<Entity>(current_group);
                    
                    // need to find the index of the entity type
                    size_t entity_type_index = 0;
                    for (size_t i = 1; i < ENTITY_DATA_INSTANCES.size(); i++) {
                        if (ENTITY_DATA_INSTANCES[i]->GetType() == ent_name) {
                            entity_type_index = i;
                            break;
                        }
                    }
                    // TODO: find a way to nicely abort the program
                    //std::cout << ent_name << std::endl;
                    assert(entity_type_index);
                    assert(entity_type_index < ENTITY_DATA_INSTANCES.size());
                    
                    entity->properties["name"] = std::string(SerializedEntityData::Field<name_t>().FromString(str));

                    entity->properties["position-x"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["position-y"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["position-z"] = SerializedEntityData::Field<float>().FromString(str);

                    entity->properties["rotation-x"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["rotation-y"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["rotation-z"] = SerializedEntityData::Field<float>().FromString(str);

                    entity->properties["action"] = std::string(SerializedEntityData::Field<name_t>().FromString(str));
                    
                    // need the int32_t cast so that PropertyValue gets initialized to an enum type
                    entity->SetProperty("entity-type", (int32_t) entity_type_index);
                    //assert(entity->entity_data && entity->entity_data->data);
                    
                    //entity->entity_data->data->FromString(str);
                    //entity->entity_data->SerializedDataToProperties(entity.get());
                    
                    // TODO: reimplement this stuff
                    
                    for (auto& prop : ENTITY_DATA_INSTANCES[entity_type_index]->GetFieldInfo()) {
                        switch (prop.type) {
                            case Core::SerializedEntityData::FieldInfo::FIELD_STRING:
                            case Core::SerializedEntityData::FieldInfo::FIELD_MODELNAME:
                                entity->SetProperty(prop.name, std::string(SerializedEntityData::Field<name_t>().FromString(str))); break;
                            case Core::SerializedEntityData::FieldInfo::FIELD_INT:
                                entity->SetProperty(prop.name, SerializedEntityData::Field<int64_t>().FromString(str)); break;
                            case Core::SerializedEntityData::FieldInfo::FIELD_UINT:
                                entity->SetProperty(prop.name, SerializedEntityData::Field<uint64_t>().FromString(str)); break;
                            case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                                entity->SetProperty(prop.name, SerializedEntityData::Field<float>().FromString(str)); break;
                        }
                    }
                    
                    ((Object*)current_group)->AddChild(entity);
                }
            }
        } else {
            std::wcout << selected_language->dialog_cell_not_found << std::wstring(cell->GetName().begin(), cell->GetName().end()) << ".cell." << std::endl;
        }
    }
    
    void SaveCell(WorldCell* cell) { 
        std::ofstream file (std::string("data/") + std::string(cell->GetName()) + ".cell");
        
        if (file.is_open()) {
            std::string output_line = "# cell";
            output_line += " " + std::to_string(cell->GetProperty("is-interior").bool_value);
            output_line += " " + std::to_string(cell->GetProperty("is-interior-lighting").bool_value);
            file << output_line << std::endl;
                        
            for (auto& group : cell->group_manager->GetChildren()) {
                if (group->GetName() != "[default]") {
                    std::string output_line = "# group ";
                    output_line += group->GetName();
                    file << output_line << std::endl;
                }

                for (auto& ent : group->GetChildren()) {
                    using namespace Core;
                    //auto entity = (Entity*)ent.get();
                    //std::string str = entity->entity_data->data->GetDataName();
                    /*auto entity = dynamic_cast<Entity*>(ent.get());*/
                    int32_t entity_type_index = ent->GetProperty("entity-type");
                    std::string str = ENTITY_DATA_INSTANCES[entity_type_index]->GetType();
                    
                    str += " " + std::string(ent->GetName());
                    SerializedEntityData::Field<float>(ent->GetProperty("position-x").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("position-y").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("position-z").float_value).ToString(str);
                    
                    SerializedEntityData::Field<float>(ent->GetProperty("rotation-x").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("rotation-y").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("rotation-z").float_value).ToString(str);
                    
                    str += " " + ent->GetProperty("action").str_value;
                    
                    // TODO: rewrite
                    
                    for (auto& prop : ENTITY_DATA_INSTANCES[entity_type_index]->GetFieldInfo()) {
                        switch (prop.type) {
                            case Core::SerializedEntityData::FieldInfo::FIELD_STRING:
                            case Core::SerializedEntityData::FieldInfo::FIELD_MODELNAME:
                                SerializedEntityData::Field<name_t>(ent->GetProperty(prop.name).str_value).ToString(str); break;
                            case Core::SerializedEntityData::FieldInfo::FIELD_INT:
                                SerializedEntityData::Field<int64_t>(ent->GetProperty(prop.name).int_value).ToString(str); break;
                            case Core::SerializedEntityData::FieldInfo::FIELD_UINT:
                                SerializedEntityData::Field<uint64_t>(ent->GetProperty(prop.name).uint_value).ToString(str); break;
                            case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                                SerializedEntityData::Field<float>(ent->GetProperty(prop.name).float_value).ToString(str); break;
                        }
                    }
                    
                    //if (entity->entity_data->data) entity->entity_data->data->ToString(str);
                    file << str << std::endl;
                }
            }
            
            for (auto& trans : cell->transition_manager->GetChildren()) {
                std::string output_line = "transition";
                output_line += " " + trans->GetProperty("name").str_value;
                output_line += " " + trans->GetProperty("cell-into").str_value;
                output_line += " " + std::to_string(trans->GetChildren().size());
                for (auto& point : trans->GetChildren()) {
                    output_line += " " + std::to_string(point->GetProperty("position-x").float_value);
                    output_line += " " + std::to_string(point->GetProperty("position-y").float_value);
                    output_line += " " + std::to_string(point->GetProperty("position-z").float_value);
                } 
                file << output_line << std::endl;
            }
            
            for (auto& path : cell->path_manager->GetChildren()) {
                std::string output_line = "path";
                output_line += " " + path->GetProperty("name").str_value;
                output_line += " " + std::to_string(path->GetChildren().size());
                for (auto& curve : path->GetChildren()) {
                    output_line += " " + std::to_string(curve->GetProperty("id").uint_value);
                    output_line += " " + std::to_string(curve->GetProperty("next-id").uint_value);
                    output_line += " " + std::to_string(curve->GetProperty("prev-id").uint_value);
                    output_line += " " + std::to_string(curve->GetProperty("left-id").uint_value);
                    output_line += " " + std::to_string(curve->GetProperty("right-id").uint_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-x").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-y").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-z").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-x-2").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-y-2").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-z-2").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-x-3").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-y-3").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-z-3").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-x-4").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-y-4").float_value);
                    output_line += " " + std::to_string(curve->GetProperty("position-z-4").float_value);
                }
                file << output_line << std::endl;
            }
            
            for (auto& navmesh : cell->navmesh_manager->GetChildren()) {
                std::string output_line = "navmesh";
                output_line += " " + navmesh->GetProperty("name").str_value;
                output_line += " " + std::to_string(navmesh->GetChildren().size());
                for (auto& node : navmesh->GetChildren()) {
                    output_line += " " + std::to_string(node->GetProperty("id").uint_value);
                    output_line += " " + std::to_string(node->GetProperty("next-id").uint_value);
                    output_line += " " + std::to_string(node->GetProperty("prev-id").uint_value);
                    output_line += " " + std::to_string(node->GetProperty("left-id").uint_value);
                    output_line += " " + std::to_string(node->GetProperty("right-id").uint_value);
                    output_line += " " + std::to_string(node->GetProperty("position-x").float_value);
                    output_line += " " + std::to_string(node->GetProperty("position-y").float_value);
                    output_line += " " + std::to_string(node->GetProperty("position-z").float_value);
                }
                file << output_line << std::endl;
            }
            
        } else {
            std::wcout << selected_language->dialog_cell_not_write << std::wstring(cell->GetName().begin(), cell->GetName().end()) << ".cell." << std::endl;
        }
    }
    
    // TODO: find from where this is being called right now and make it stop!
    void ProduceTestData() {
        return;
        std::cout << "Creating some test data..." << std::endl;
        try {
            //std::shared_ptr<WorldCell> wcell1 = std::make_shared<WorldCell>();
            //worldcells->children.push_back(wcell1);
            worldcells->AddChild();
            
            std::cout << "Finished creating some test data!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Exception occured while creating some test data: \n" << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception occured while creating some test data!";
        }
    }
    
    void Reset() {
        if (Settings::CELL_LIST_FROM_FILESYSTEM || worldcells == nullptr) {
            worldcells = std::make_shared<WorldCellManager>(nullptr);
        }
        
        selection = std::make_shared<Selection>();
        
        Editor::performed_actions.clear();
        Editor::unperformed_actions.clear();
        Editor::data_modified = false;
        
        if (Settings::CELL_LIST_FROM_FILESYSTEM) {
            auto dir = std::filesystem::directory_iterator("data/");
            for (auto& cell : dir) {
                if (cell.is_regular_file() && cell.path().extension() == ".cell") {
                    auto cell_name = cell.path().stem().string();
                    auto worldcell = std::make_shared<WorldCell>(worldcells.get(), cell_name);
                    std::static_pointer_cast<Object>(worldcells)->AddChild(worldcell);
                }
            }
        }
    }
    
    void Init() {
        //Core::Init();
                
        //RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::Crate::Data; d->collmodel = 0; d->model = 0; return d; });
        //RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::Lamp::Data; return d; });
        //RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::StaticWorldObject::Data; d->lightmap = 0; d->model = 0; return d; });
        
        RegisterEntityType (dynamic_cast<Core::SerializedEntityData*>(new Core::Crate::Data));
        RegisterEntityType (dynamic_cast<Core::SerializedEntityData*>(new Core::Lamp::Data));
        RegisterEntityType (dynamic_cast<Core::SerializedEntityData*>(new Core::StaticWorldObject::Data));
    }
}