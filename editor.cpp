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
    struct EntityData {
        Core::SerializedEntityData* data = nullptr;
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
                    case Core::SerializedEntityData::FieldInfo::FIELD_UINT64:
                        entity->SetProperty(prop.data_name, *(uint64_t*) prop.field);
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                        entity->SetProperty(prop.data_name, *(float*) prop.field);
                        break;
                    case Core::SerializedEntityData::FieldInfo::FIELD_NAME:
                        entity->SetProperty(prop.data_name, *(Core::name_t*) prop.field);
                        break;
                }
            }
        }
    };
    
    std::shared_ptr<Selection> selection;
    std::shared_ptr<WorldCellManager> worldcells;
    std::list<std::unique_ptr<Action>> performed_actions;
    std::list<std::unique_ptr<Action>> unperformed_actions;
    
    std::unordered_map<std::string, std::vector<std::string>> property_enumerations = {
        {"entity-type", {"[none]"}}
    };
    
    std::unordered_map<size_t, Core::SerializedEntityData* (*)(void)> entity_data_constructors;
    
    void RegisterEntityType(Core::SerializedEntityData* (*constructor)(void)) {
        auto instance = constructor();
        
        auto index = property_enumerations["entity-type"].size();
        property_enumerations["entity-type"].push_back(instance->GetEditorName());
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
            
            if (property_value.enum_value == 0) {
                if (entity_data) {
                    if (entity_data->render_component) {
                        entity_data->render_component->Uninit();
                        Core::PoolProxy<Core::RenderComponent>::Delete(entity_data->render_component);
                    }
                    
                    if (entity_data->data) {
                        delete entity_data->data;
                    }
                    
                    delete entity_data;
                    entity_data = nullptr;
                }
                return;
            }
            
            if (!entity_data) entity_data = new EntityData;
            if (entity_data->data) delete entity_data->data;
            entity_data->data = entity_data_constructors[property_value.enum_value]();
            goto update_model;
        } else {
            properties[property_name] = property_value;
            goto update_model;
        }
        
        return;
        
        update_model:
        entity_data->PropertiesToSerializedData(this);
        
        /*if (entity_data->render_component) {
            entity_data->render_component->SetModel(entity_data->data->GetEditorModel());
        } else {
            entity_data->render_component = Core::PoolProxy<Core::RenderComponent>::New();
            entity_data->render_component->UpdateRotation(glm::quat(glm::vec3(
                properties["rotation-x"].float_value,
                properties["rotation-y"].float_value,
                properties["rotation-z"].float_value
            )));
            entity_data->render_component->UpdateRotation(glm::vec3(
                properties["position-x"].float_value,
                properties["position-y"].float_value,
                properties["position-z"].float_value
            ));
            entity_data->render_component->SetModel(entity_data->data->GetEditorModel());
            entity_data->render_component->SetLightmap(Core::UID("fullbright"));
            entity_data->render_component->Start();
        }*/
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
            std::cout << "entity_data is!" << std::endl;
        } else {
            std::cout << "entity_data is NOT!" << std::endl;
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
    void LoadCell(WorldCell* cell) {
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
                        // TODO: stuff these where they belong
                        bool is_interior = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        bool is_interior_lighting = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    } else if (annotation_name == "group") {
                        auto new_group = current_group->parent->AddChild();
                        new_group->SetProperty("name", std::string(Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str))));
                        current_group = (EntityGroup*) new_group.get();
                    }
                } else if (ent_name == "transition") {
                    Transition* trans = new Transition;
                    trans->name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                    trans->cell_into_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                    trans->parent = this;
                    uint64_t point_count = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    for (uint64_t i = 0; i < point_count; i++) {
                        Transition::Point* p = new Transition::Point;
                        p->location.x = Core::SerializedEntityData::Field<float>().FromString(str);
                        p->location.y = Core::SerializedEntityData::Field<float>().FromString(str);
                        p->location.z = Core::SerializedEntityData::Field<float>().FromString(str);
                        p->parent = trans;
                        trans->points.push_back(p);
                    }
                    transitions.push_back(trans);
                } else if (ent_name == "path") {
                    Path* path = new Path;
                    path->name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                    path->parent = this;
                    uint64_t curve_count = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    for (uint64_t i = 0; i < curve_count; i++) {
                        Path::Curve* c = new Path::Curve;
                        c->id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        c->next_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        c->prev_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        c->left_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        c->right_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        c->location1.x = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location1.y = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location1.z = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location2.x = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location2.y = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location2.z = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location3.x = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location3.y = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location3.z = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location4.x = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location4.y = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->location4.z = Core::SerializedEntityData::Field<float>().FromString(str);
                        c->parent = path;
                        path->curves.push_back(c);
                    }
                    paths.push_back(path);
                } else if (ent_name == "navmesh") {
                    Navmesh* navmesh = new Navmesh;
                    navmesh->name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                    navmesh->parent = this;
                    uint64_t node_count = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    for (uint64_t i = 0; i < node_count; i++) {
                        Navmesh::Node* n = new Navmesh::Node;
                        n->id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        n->next_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        n->prev_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        n->left_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        n->right_id = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        n->location.x = Core::SerializedEntityData::Field<float>().FromString(str);
                        n->location.y = Core::SerializedEntityData::Field<float>().FromString(str);
                        n->location.z = Core::SerializedEntityData::Field<float>().FromString(str);
                        n->parent = navmesh;
                        navmesh->nodes.push_back(n);
                    }
                    navmeshes.push_back(navmesh);
                } else {
                    auto ent = new Entity {.id = 0, .parent = this, .group = current_group};
                    ent->FromString(str);
                    ent->ent_data = entityDatas[ent_name]();
                    ent->ent_data->FromString(str);
                    if (current_group) {
                        current_group->entities.push_back(ent);
                    } else {
                        entities.push_back(ent);
                    }
                }
            }
        } else {
            std::cout << "Can't find the cell file " << name << ".cell." << std::endl;
        }
    }
    /*
    void WorldCell::Save() { 
        
        std::ofstream file (std::string("data/") + name + ".cell");
        
        if (file.is_open()) {
            std::string output_line = "# cell";
            output_line += " " + std::to_string(is_interior);
            output_line += " " + std::to_string(is_interior_lighting);
            file << output_line << std::endl;
            
            for (auto ent : entities) {
                std::string output_line = ent->ent_data->GetDataName();
                ent->ToString(output_line);
                if (ent->ent_data) ent->ent_data->ToString(output_line);
                file << output_line << std::endl;
            }
            
            for (auto group : groups) {
                std::string output_line = "# group ";
                output_line += group->name;
                file << output_line << std::endl;
                
                for (auto ent : group->entities) {
                    std::string output_line = ent->ent_data->GetDataName();
                    ent->ToString(output_line);
                    if (ent->ent_data) ent->ent_data->ToString(output_line);
                    file << output_line << std::endl;
                }
            }
            
            for (auto trans : transitions) {
                std::string output_line = "transition";
                output_line += " " + trans->name;
                output_line += " " + trans->cell_into_name;
                output_line += " " + std::to_string(trans->points.size());
                for (auto point : trans->points) {
                    output_line += " " + std::to_string(point->location.x);
                    output_line += " " + std::to_string(point->location.y);
                    output_line += " " + std::to_string(point->location.z);
                } 
                file << output_line << std::endl;
            }
            
            for (auto path : paths) {
                std::string output_line = "path";
                output_line += " " + path->name;
                output_line += " " + std::to_string(path->curves.size());
                for (auto curve : path->curves) {
                    output_line += " " + std::to_string(curve->id);
                    output_line += " " + std::to_string(curve->next_id);
                    output_line += " " + std::to_string(curve->prev_id);
                    output_line += " " + std::to_string(curve->left_id);
                    output_line += " " + std::to_string(curve->right_id);
                    output_line += " " + std::to_string(curve->location1.x);
                    output_line += " " + std::to_string(curve->location1.y);
                    output_line += " " + std::to_string(curve->location1.z);
                    output_line += " " + std::to_string(curve->location2.x);
                    output_line += " " + std::to_string(curve->location2.y);
                    output_line += " " + std::to_string(curve->location2.z);
                    output_line += " " + std::to_string(curve->location3.x);
                    output_line += " " + std::to_string(curve->location3.y);
                    output_line += " " + std::to_string(curve->location3.z);
                    output_line += " " + std::to_string(curve->location4.x);
                    output_line += " " + std::to_string(curve->location4.y);
                    output_line += " " + std::to_string(curve->location4.z);
                }
                file << output_line << std::endl;
            }
            
            for (auto navmesh : navmeshes) {
                std::string output_line = "navmesh";
                output_line += " " + navmesh->name;
                output_line += " " + std::to_string(navmesh->nodes.size());
                for (auto node : navmesh->nodes) {
                    output_line += " " + std::to_string(node->id);
                    output_line += " " + std::to_string(node->next_id);
                    output_line += " " + std::to_string(node->prev_id);
                    output_line += " " + std::to_string(node->left_id);
                    output_line += " " + std::to_string(node->right_id);
                    output_line += " " + std::to_string(node->location.x);
                    output_line += " " + std::to_string(node->location.y);
                    output_line += " " + std::to_string(node->location.z);
                }
                file << output_line << std::endl;
            }
            
        } else {
            std::cout << "Can't write to the cell file " << name << ".cell." << std::endl;
        }
    }*/
    
    
    void ProduceTestData() {
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
        
        //worldcells = new WorldCellManager (nullptr);
        worldcells = std::make_shared<WorldCellManager>(nullptr);
        selection = std::make_shared<Selection>();
        
        RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::Crate::Data; d->collmodel = 0; d->model = 0; return d; });
        RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::Lamp::Data; return d; });
        RegisterEntityType([]() -> Core::SerializedEntityData* { auto d = new Core::StaticWorldObject::Data; d->lightmap = 0; d->model = 0; return d; });
        
        std::ifstream file("data/editor_data.txt");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::static_pointer_cast<Object>(worldcells)->AddChild(std::make_shared<WorldCell>(worldcells.get(), line));
            }
        } else {
            std::cout << "Can't find the editor config file." << std::endl;
        }
    }
}