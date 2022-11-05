#include <editor.h>
#include <iostream>
#include <fstream>

#include <components/rendercomponent.h>

#include <entities/crate.h>
#include <entities/lamp.h>
#include <entities/staticworldobject.h>
//#include <moshkis.h>
//#include <pickup.h>

namespace Editor {
    // we'll need to move all of this entity handling stuff to a seperate file
    struct EntityData {
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
    };
    
    // this is kinda out of place
    std::shared_ptr<Selection> selection;
    std::shared_ptr<WorldCellManager> worldcells;
    std::list<std::unique_ptr<Action>> performed_actions;
    std::list<std::unique_ptr<Action>> unperformed_actions;
    bool data_modified = false;
    
    namespace Settings {
        Space TRANSFORM_SPACE = SPACE_WORLD;
        Rotation ROTATION_UNIT = ROTATION_RADIANS;
    }
    
    std::unordered_map<std::string, std::vector<std::string>> property_enumerations = {
        {"entity-type", {"[none]"}}
    };
    
    std::unordered_map<size_t, Core::SerializedEntityData* (*)(void)> entity_data_constructors;
    std::vector<std::string> entity_data_names = {"none"};
    
    void RegisterEntityType(Core::SerializedEntityData* (*constructor)(void)) {
        auto instance = constructor();
        
        auto index = property_enumerations["entity-type"].size();
        property_enumerations["entity-type"].push_back(instance->GetEditorName());
        entity_data_names.push_back(instance->GetDataName());
        entity_data_constructors[index] = constructor;
        
        std::cout << "enum size: " << property_enumerations["entity-type"].size() << std::endl;
        std::cout << "map size: " << entity_data_constructors.size() << std::endl;
        
        delete instance;
    }
    
    PropertyValue Entity::GetProperty (std::string property_name) { return properties[property_name]; }
    
    void Entity::SetProperty (std::string property_name, PropertyValue property_value) {
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
        }
    }
    
    void Entity::SetHidden(bool is_hidden) {
        std::cout << "got asked to be hidden: " << is_hidden << std::endl; 
        this->is_hidden = is_hidden;
        
        if (is_hidden && entity_data && entity_data->render_component) {
            entity_data->RemoveRenderComponent(this);
        }
        
        if (!is_hidden && entity_data && !entity_data->render_component) {
            entity_data->PropertiesToSerializedData(this);
            entity_data->UpdateRenderComponent(this);
        }
    }
    
    std::vector<PropertyDefinition> Entity::GetFullPropertyDefinitions() { 
        auto defs = std::vector<PropertyDefinition> {
            {"group-entity", "Entity", "", PROPERTY_CATEGORY},
            {"name", "Name", "group-entity", PROPERTY_STRING},
            {"group-position", "Location", "", PROPERTY_CATEGORY},
            {"position-x", "X", "group-position", PROPERTY_FLOAT},
            {"position-y", "Y", "group-position", PROPERTY_FLOAT},
            {"position-z", "Z", "group-position", PROPERTY_FLOAT},
            {"group-rotation", "Rotation", "", PROPERTY_CATEGORY},
            {"rotation-x", "X", "group-rotation", PROPERTY_FLOAT},
            {"rotation-y", "Y", "group-rotation", PROPERTY_FLOAT},
            {"rotation-z", "Z", "group-rotation", PROPERTY_FLOAT},
            {"entity-type", "Entity Type", "group-entity", PROPERTY_ENUM}
        };
        
        if (entity_data) {
            defs.push_back({"group-entity-special", entity_data->data->GetEditorName(), "", PROPERTY_CATEGORY});
            
            // this will translate all of the engine entity properties into editor properties
            auto props = entity_data->data->GetEditorFieldInfo();
            for (auto& prop : props) {
                switch (prop.type) {
                    case Core::SerializedEntityData::FieldInfo::FIELD_UINT64:
                        defs.push_back({prop.data_name, prop.display_name, "group-entity-special", PROPERTY_UINT});
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                        defs.push_back({prop.data_name, prop.display_name, "group-entity-special", PROPERTY_FLOAT});
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_NAME:
                        defs.push_back({prop.data_name, prop.display_name, "group-entity-special", PROPERTY_STRING});
                        break;
                }
            }
            //std::cout << "entity_data is!" << std::endl;
        } else {
            //std::cout << "entity_data is NOT!" << std::endl;
        }
        
        return defs;
    }
    
    /*
    
    
    
    void Entity::Show () {
        if (model || !ent_data) return;
        model = Core::PoolProxy<Core::RenderComponent>::New();
        model->SetModel(ent_data->GetEditorModel());
        model->SetLightmap(Core::UID("fullbright"));
        model->SetPose(Core::Render::poseList.begin().ptr);
        model->UpdateLocation(location);
        model->UpdateRotation(glm::quat(rotation));
        model->Init();
    }
    
    void Entity::Hide () {
        if (!model) return;
        model->Uninit();
        Core::PoolProxy<Core::RenderComponent>::Delete(model);
        model = nullptr;
    }
    
    void Entity::ModelUpdate () {
        if (!model && parent->is_visible) Show();
        if (!model) return;
        model->UpdateLocation(location);
        model->UpdateRotation(glm::quat(rotation));
        if (model->GetModel() != ent_data->GetEditorModel()) {
            std::cout << "owo model changed" << std::endl;
            //std::cout << model->GetModel() << "<- rn model | next model ->" << ent_data->GetEditorModel() << std::endl;
            std::cout << Core::ReverseUID(model->GetModel()) << "  " << Core::ReverseUID(ent_data->GetEditorModel()) << std::endl;
            model->Uninit();
            model->SetModel(ent_data->GetEditorModel());
            model->Init();
        }
    }
    
    void Entity::FromString (std::string_view& str) {
        using namespace Core;
        name = ReverseUID(SerializedEntityData::Field<name_t>().FromStringAsName(str));

        location.x = SerializedEntityData::Field<float>().FromString(str);
        location.y = SerializedEntityData::Field<float>().FromString(str);
        location.z = SerializedEntityData::Field<float>().FromString(str);

        rotation.x = SerializedEntityData::Field<float>().FromString(str);
        rotation.y = SerializedEntityData::Field<float>().FromString(str);
        rotation.z = SerializedEntityData::Field<float>().FromString(str);

        action = ReverseUID(SerializedEntityData::Field<name_t>().FromStringAsName(str));
    }
    
    void Entity::ToString (std::string& str) {
        using namespace Core;
        str += " " + name;
        SerializedEntityData::Field<float>(location.x).ToString(str);
        SerializedEntityData::Field<float>(location.y).ToString(str);
        SerializedEntityData::Field<float>(location.z).ToString(str);
        
        SerializedEntityData::Field<float>(rotation.x).ToString(str);
        SerializedEntityData::Field<float>(rotation.y).ToString(str);
        SerializedEntityData::Field<float>(rotation.z).ToString(str);
        
        str += " " + action;
    }*/
    
    // this function is absolutely terrible!
    // TODO: wrap Core::SerializedEntityData::Field<TTT>().FromString(STR) function
    void LoadCell(WorldCell* cell) {
        using namespace Core;
        std::ifstream file(std::string("data/") + std::string(cell->GetName()) + ".cell");
        EntityGroup* current_group = (EntityGroup*)cell->group_manager->GetChildren().front().get();
        assert(current_group->GetName() == "[default]");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (line.size() < 3) continue;
                std::string_view str (line);
                std::string ent_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                if (ent_name == "#") {
                    std::string annotation_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                    if (annotation_name == "cell") {
                        cell->SetProperty("is-interior", (bool) Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                        cell->SetProperty("is-interior-lighting", (bool) Core::SerializedEntityData::Field<uint64_t>().FromString(str));
                    } else if (annotation_name == "group") {
                        auto new_group = current_group->parent->AddChild();
                        new_group->SetProperty("name", std::string(Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str))));
                        current_group = (EntityGroup*) new_group.get();
                    }
                } else if (ent_name == "transition") {
                    auto trans = std::make_shared<Transition>(cell->transition_manager.get());
                    trans->SetProperty("name", std::string(Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str))));
                    trans->SetProperty("cell-into", std::string(Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str))));
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
                    path->SetProperty("name", std::string(Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str))));
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
                    navmesh->SetProperty("name", std::string(Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str))));
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
                    for (size_t i = 0; i < entity_data_names.size(); i++) {
                        if (entity_data_names[i] == ent_name) {
                            entity_type_index = i;
                            break;
                        }
                    }
                    // TODO: find a way to nicely abort the program
                    //std::cout << ent_name << std::endl;
                    assert(entity_type_index);
                    
                    entity->properties["name"] = std::string(ReverseUID(SerializedEntityData::Field<name_t>().FromStringAsName(str)));

                    entity->properties["position-x"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["position-y"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["position-z"] = SerializedEntityData::Field<float>().FromString(str);

                    entity->properties["rotation-x"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["rotation-y"] = SerializedEntityData::Field<float>().FromString(str);
                    entity->properties["rotation-z"] = SerializedEntityData::Field<float>().FromString(str);

                    /*entity->properties["action"] =*/ ReverseUID(SerializedEntityData::Field<name_t>().FromStringAsName(str));
                    
                    // need the int32_t cast so that PropertyValue gets initialized to an enum type
                    entity->SetProperty("entity-type", (int32_t) entity_type_index);
                    assert(entity->entity_data && entity->entity_data->data);
                    
                    entity->entity_data->data->FromString(str);
                    entity->entity_data->SerializedDataToProperties(entity.get());
                    
                    ((Object*)current_group)->AddChild(entity);
                }
            }
        } else {
            std::cout << "Can't find the cell file " << cell->GetName() << ".cell." << std::endl;
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
                    auto entity = (Entity*)ent.get();
                    std::string str = entity->entity_data->data->GetDataName();
                    
                    str += " " + std::string(ent->GetName());
                    SerializedEntityData::Field<float>(ent->GetProperty("position-x").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("position-y").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("position-z").float_value).ToString(str);
                    
                    SerializedEntityData::Field<float>(ent->GetProperty("rotation-x").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("rotation-y").float_value).ToString(str);
                    SerializedEntityData::Field<float>(ent->GetProperty("rotation-z").float_value).ToString(str);
                    
                    str += " " + ent->GetProperty("action").str_value;
                    
                    if (entity->entity_data->data) entity->entity_data->data->ToString(str);
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
            std::cout << "Can't write to the cell file " << std::string(cell->GetName()) << ".cell." << std::endl;
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
    
    void Init(){
        Core::Init();
        
        worldcells = std::make_shared<WorldCellManager>(nullptr);
        selection = std::make_shared<Selection>();
        
        RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::Crate::Data; d->collmodel = 0; d->model = 0; return d; });
        RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::Lamp::Data; return d; });
        RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::StaticWorldObject::Data; d->lightmap = 0; d->model = 0; return d; });
        
        std::ifstream file("data/editor_data.txt");
        
        // TODO: clean this up a bit
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                auto worldcell = std::make_shared<WorldCell>(worldcells.get(), line);
                std::static_pointer_cast<Object>(worldcells)->AddChild(worldcell);
            }
        } else {
            std::cout << "Can't find the editor config file." << std::endl;
        }
    }
}