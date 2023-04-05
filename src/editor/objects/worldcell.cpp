#include <editor/objects/worldcell.h>

#include <framework/file.h>

namespace Editor {

void WorldCell::LoadFromDisk() {
    using namespace tram;
    WorldCell* cell = this;
    
    File file ((std::string("data/worldcells/") + std::string(cell->GetName()) + ".cell").c_str(), MODE_READ);
    EntityGroup* current_group = (EntityGroup*) cell->group_manager->GetChildren().front().get();
    
    if (!file.is_open()) {
        std::wcout << "FILE NOT FAUND" << std::wstring(cell->GetName().begin(), cell->GetName().end()) << ".cell." << std::endl;
        return;
    }
    
    name_t header = file.read_name();
    
    assert(header == "CELLv2");
    
    file.read_name(); // skip cell name 
    
    cell->SetProperty("is-interior", (bool) file.read_uint32());
    cell->SetProperty("is-interior-lighting", (bool) file.read_uint32());
    
    while (file.is_continue()) {
        name_t ent_type = file.read_name();
        
        if (ent_type == "group") {
            auto new_group = current_group->parent->AddChild();
            new_group->SetProperty("name", std::string(file.read_name()));
            current_group = (EntityGroup*) new_group.get();
        } else if (ent_type == "transition") {
            auto trans = std::make_shared<Transition>(cell->transition_manager.get());
            trans->SetProperty("name", std::string(file.read_name()));
            trans->SetProperty("cell-into", std::string(file.read_name()));
            uint64_t point_count = file.read_uint32();
            for (uint64_t i = 0; i < point_count; i++) {
                auto p = trans->AddChild();
                p->SetProperty("position-x", file.read_float32());
                p->SetProperty("position-y", file.read_float32());
                p->SetProperty("position-z", file.read_float32());
            }
            static_cast<Object*>(cell->transition_manager.get())->AddChild(trans);
        } else if (ent_type == "path") {
            /*
            auto path = std::make_shared<Path>(cell->path_manager.get());
            path->SetProperty("name", std::string(tram::SerializedEntityData::Field<tram::name_t>().FromString(str)));
            uint64_t curve_count = tram::SerializedEntityData::Field<uint64_t>().FromString(str);
            for (uint64_t i = 0; i < curve_count; i++) {
                auto c = path->AddChild();
                c->SetProperty("id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                c->SetProperty("next-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                c->SetProperty("prev-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                c->SetProperty("left-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                c->SetProperty("right-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                c->SetProperty("position-x", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-y", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-z", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-x-2", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-y-2", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-z-2", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-x-3", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-y-3", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-z-3", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-x-4", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-y-4", tram::SerializedEntityData::Field<float>().FromString(str));
                c->SetProperty("position-z-4", tram::SerializedEntityData::Field<float>().FromString(str));
            }
            static_cast<Object*>(cell->path_manager.get())->AddChild(path);*/
        } else if (ent_type == "navmesh") {
            /*
            auto navmesh = std::make_shared<Path>(cell->navmesh_manager.get());
            navmesh->SetProperty("name", std::string(tram::SerializedEntityData::Field<tram::name_t>().FromString(str)));
            uint64_t node_count = tram::SerializedEntityData::Field<uint64_t>().FromString(str);
            for (uint64_t i = 0; i < node_count; i++) {
                auto n = navmesh->AddChild();
                n->SetProperty("id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                n->SetProperty("next-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                n->SetProperty("prev-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                n->SetProperty("left-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                n->SetProperty("right-id", tram::SerializedEntityData::Field<uint64_t>().FromString(str));
                n->SetProperty("position-x", tram::SerializedEntityData::Field<float>().FromString(str));
                n->SetProperty("position-y", tram::SerializedEntityData::Field<float>().FromString(str));
                n->SetProperty("position-z", tram::SerializedEntityData::Field<float>().FromString(str));
            }
            static_cast<Object*>(cell->navmesh_manager.get())->AddChild(navmesh);*/
        } else {
            auto entity = std::make_shared<Entity>(current_group);

            entity->SetEntityType(ent_type);
            
            entity->properties["id"] = file.read_uint64();
            entity->properties["name"] = std::string(file.read_name());

            entity->properties["position-x"] = file.read_float32();
            entity->properties["position-y"] = file.read_float32();
            entity->properties["position-z"] = file.read_float32();

            entity->properties["rotation-x"] = file.read_float32();
            entity->properties["rotation-y"] = file.read_float32();
            entity->properties["rotation-z"] = file.read_float32();
            
            auto data = file.read_line();
            
            // TODO: figure out how to change SerializedEntityData functions to regular File functions
            
            // this is going to be tricky, since we have to keep track of newlines
            // so that bad SerializedEntityData definitions don't destroy the file reader alignment with lines
            
            auto entity_definitions = entity->GetSerializedEntityDataDefinitions();
            
            for (auto& prop : entity_definitions) {
                switch (prop.type) {
                    default: // TODO: actually implement writers for other types
                    case PROPERTY_STRING:
                        entity->SetProperty(prop.name, std::string(SerializedEntityData::Field<name_t>().FromString(data))); break;
                    case PROPERTY_INT:
                        entity->SetProperty(prop.name, SerializedEntityData::Field<int64_t>().FromString(data)); break;
                    case PROPERTY_UINT:
                        entity->SetProperty(prop.name, SerializedEntityData::Field<uint64_t>().FromString(data)); break;
                    case PROPERTY_FLOAT:
                        entity->SetProperty(prop.name, SerializedEntityData::Field<float>().FromString(data)); break;
                    case PROPERTY_CATEGORY:
                        break;
                }
            }
            
            ((Object*)current_group)->AddChild(entity);
        }
    }
}

void WorldCell::SaveToDisk() {
    //std::ofstream file (std::string("data/worldcells/") + std::string(cell->GetName()) + ".cell");
    /*
    if (file.is_open()) {
        std::string output_line = "# cell";
        output_line += " " + std::to_string(cell->GetProperty("is-interior").bool_value);
        output_line += " " + std::to_string(cell->GetProperty("is-interior-lighting").bool_value);
        file << output_line << std::endl;
                    
        for (auto& group : cell->group_manager->GetChildren()) {
            if (group->GetName() != "[default]") {
                std::string output_line = "group ";
                output_line += group->GetName();
                file << output_line << std::endl;
            }

            for (auto& ent : group->GetChildren()) {
                using namespace tram;
                int32_t entity_type_index = ent->GetProperty("entity-type");
                std::string str = ENTITY_DATA_INSTANCES[entity_type_index]->GetType();
                
                SerializedEntityData::Field<float>(ent->GetProperty("is").int_value).ToString(str);
                str += " " + std::string(ent->GetName());
                SerializedEntityData::Field<float>(ent->GetProperty("position-x").float_value).ToString(str);
                SerializedEntityData::Field<float>(ent->GetProperty("position-y").float_value).ToString(str);
                SerializedEntityData::Field<float>(ent->GetProperty("position-z").float_value).ToString(str);
                
                SerializedEntityData::Field<float>(ent->GetProperty("rotation-x").float_value).ToString(str);
                SerializedEntityData::Field<float>(ent->GetProperty("rotation-y").float_value).ToString(str);
                SerializedEntityData::Field<float>(ent->GetProperty("rotation-z").float_value).ToString(str);
                
                str += " " + ent->GetProperty("action").str_value;
                
                for (auto& prop : ENTITY_DATA_INSTANCES[entity_type_index]->GetFieldInfo()) {
                    switch (prop.type) {
                        case tram::SerializedEntityData::FieldInfo::FIELD_STRING:
                        case tram::SerializedEntityData::FieldInfo::FIELD_MODELNAME:
                            SerializedEntityData::Field<name_t>(ent->GetProperty(prop.name).str_value).ToString(str); break;
                        case tram::SerializedEntityData::FieldInfo::FIELD_INT:
                            SerializedEntityData::Field<int64_t>(ent->GetProperty(prop.name).int_value).ToString(str); break;
                        case tram::SerializedEntityData::FieldInfo::FIELD_UINT:
                            SerializedEntityData::Field<uint64_t>(ent->GetProperty(prop.name).uint_value).ToString(str); break;
                        case tram::SerializedEntityData::FieldInfo::FIELD_FLOAT:
                            SerializedEntityData::Field<float>(ent->GetProperty(prop.name).float_value).ToString(str); break;
                    }
                }
                
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
    }*/
}
}