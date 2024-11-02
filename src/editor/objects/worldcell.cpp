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
    
    assert(header == "CELLv3");
    
    file.read_name(); // skip cell name 
    
    cell->SetProperty("is-interior", (bool) file.read_uint32());
    cell->SetProperty("is-interior-lighting", (bool) file.read_uint32());
    
    // this will be used, so that we can attach signals to entities
    std::unordered_map<uint32_t, Entity*> loaded_entities;
    
    while (file.is_continue()) {
        name_t ent_type = file.read_name();
        
        //std::cout << "entity type: " << ent_type << std::endl; 
        
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
        } else if (ent_type == "signal") {
            
            uint32_t source = file.read_uint64();
            
            Signal signal;
            
            signal.type = (const char*)file.read_name();
            signal.target = (const char*)file.read_name();
            signal.delay = file.read_float32();
            signal.limit = file.read_int32();
            signal.message = (const char*)file.read_name();
            
            auto param_type = file.read_name();
            
            if (param_type == "none") {
                signal.param_type = "none";
                signal.param = "";
            } else if (param_type == "int") {
                signal.param_type = "int";
                signal.param = (const char*)file.read_name(); // stupid
            } else if (param_type == "float") {
                signal.param_type = "float";
                signal.param = (const char*)file.read_name(); // dumb
            } else if (param_type == "name") {
                signal.param_type = "name";
                signal.param = (const char*)file.read_name(); // yes, good
            } else if (param_type == "vec3") {
                vec3 vec = {file.read_float32(), file.read_float32(), file.read_float32()};
                
                signal.param_type = "vec3";
                signal.param = std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);
            }
            
            loaded_entities[source]->signals.push_back(signal);
            
        } else {
            auto entity = std::make_shared<Entity>(current_group);

            entity->SetEntityType(ent_type);
            
            entity->SetProperty("id", file.read_uint64());
            entity->properties["name"] = std::string(file.read_name());
            entity->properties["entity-flags"] = PropertyValue::Flag(file.read_uint32());
            
            entity->properties["position-x"] = file.read_float32();
            entity->properties["position-y"] = file.read_float32();
            entity->properties["position-z"] = file.read_float32();

            entity->properties["rotation-x"] = file.read_float32();
            entity->properties["rotation-y"] = file.read_float32();
            entity->properties["rotation-z"] = file.read_float32();
            
            auto entity_definitions = entity->GetSerializationPropertyDefinitions();
            
            for (auto& prop : entity_definitions) {
                switch (prop.type) {
                    default: // TODO: actually implement writers for other types
                        std::cout << "idk what this is" << std::endl;
                    case PROPERTY_STRING:
                        entity->SetProperty(prop.name, std::string(file.read_name())); break;
                    case PROPERTY_INT:
                        entity->SetProperty(prop.name, file.read_int64()); break;
                    case PROPERTY_UINT:
                        entity->SetProperty(prop.name, file.read_uint64()); break;
                    case PROPERTY_FLOAT:
                        entity->SetProperty(prop.name, file.read_float32()); break;
                    case PROPERTY_ENUM:
                        entity->SetProperty(prop.name, file.read_int32()); break;
                    case PROPERTY_VECTOR:
                    case PROPERTY_ORIGIN:
                    case PROPERTY_DIRECTION:
                        //Editor::PropertyValue new_prop = Editor::PropertyValue::Vector {file.read_float32(), file.read_float32(), file.read_float32()};
                        Editor::PropertyValue new_prop = vec3{file.read_float32(), file.read_float32(), file.read_float32()};
                        new_prop.type = prop.type;
                        entity->SetProperty(prop.name, new_prop);
                        break;
                }
            }
            
            // attach entity to parent group
            ((Object*)current_group)->AddChild(entity);
            
            // save the pointer, in case we get some signals for the entity
            loaded_entities[entity->properties["id"]] = entity.get();
        }
    }
}

void WorldCell::SaveToDisk() {
    using namespace tram;
    File file ((std::string("data/worldcells/") + std::string(this->GetName()) + ".cell").c_str(), MODE_WRITE);
    
    if (!file.is_open()) {
        std::wcout << "FILE NOT OPENABLE" << std::wstring(this->GetName().begin(), this->GetName().end()) << ".cell." << std::endl;
        return;
    }
    
    file.write_name ("CELLv3");
    file.write_name (GetName().data());
    
    file.write_uint32((bool) GetProperty("is-interior"));
    file.write_uint32((bool) GetProperty("is-interior-lighting"));

    file.write_newline();
              
    for (auto& group : group_manager->GetChildren()) {
        if (group->GetName() != "[default]") {
            file.write_name("group");
            file.write_name(group->GetName().data());
            file.write_newline();
        }

        for (auto& ent : group->GetChildren()) {
            int32_t entity_type_index = ent->GetProperty("entity-type");
            
            file.write_name(PROPERTY_ENUMERATIONS["entity-type"][entity_type_index]);
            
            file.write_uint32(ent->GetProperty("id"));
            file.write_name(ent->GetProperty("name").str_value);
            file.write_uint32(ent->GetProperty("entity-flags"));
            
            file.write_float32(ent->GetProperty("position-x"));
            file.write_float32(ent->GetProperty("position-y"));
            file.write_float32(ent->GetProperty("position-z"));
            
            file.write_float32(ent->GetProperty("rotation-x"));
            file.write_float32(ent->GetProperty("rotation-y"));
            file.write_float32(ent->GetProperty("rotation-z"));
            
            auto data_defs = std::dynamic_pointer_cast<Entity>(ent)->GetSerializationPropertyDefinitions();
            
            for (auto& prop : data_defs) {
                switch (prop.type) {
                    default: // TODO: actually implement writers for other types
                    case PROPERTY_STRING:
                        file.write_name(ent->GetProperty(prop.name).str_value); break;
                    case PROPERTY_INT:
                        file.write_int32(ent->GetProperty(prop.name)); break;
                    case PROPERTY_UINT:
                        file.write_uint32(ent->GetProperty(prop.name)); break;
                    case PROPERTY_FLOAT:
                        file.write_float32(ent->GetProperty(prop.name)); break;
                     case PROPERTY_ENUM:
                        file.write_int32(ent->GetProperty(prop.name)); break;
                    case PROPERTY_VECTOR:
                    case PROPERTY_ORIGIN:
                    case PROPERTY_DIRECTION:
                        file.write_float32(ent->GetProperty(prop.name).vector_value.x);
                        file.write_float32(ent->GetProperty(prop.name).vector_value.y);
                        file.write_float32(ent->GetProperty(prop.name).vector_value.z);
                        break;
                    case PROPERTY_CATEGORY:
                        break;
                }
            }
            
            file.write_newline();
            
            if (auto ptr = std::dynamic_pointer_cast<Entity>(ent); ptr->signals.size()) {
                for (const auto& s : ptr->signals) {
                    file.write_name("signal");
                    file.write_uint32(ent->GetProperty("id"));
                    
                    file.write_name(s.type);
                    file.write_name(s.target);
                    file.write_float32(s.delay);
                    file.write_int32(s.limit);
                    file.write_name(s.message);

                    if (s.param_type == "none") {
                        file.write_name("none");
                    } else if (s.param_type == "int") {
                        file.write_name("int");
                        file.write_name(s.param);
                    } else if (s.param_type == "float") {
                        file.write_name("float");
                        file.write_name(s.param);
                    } else if (s.param_type == "name") {
                        file.write_name("name");
                        file.write_name(s.param);
                    } else if (s.param_type == "vec3") {
                        file.write_name("vec3");
                        file.write_name(s.param);
                    }
                    
                    file.write_newline();
                }
            }
        }
    }
    
    for (auto& trans : transition_manager->GetChildren()) {
        file.write_name("transition");
        
        file.write_name(trans->GetProperty("name").str_value);
        file.write_name(trans->GetProperty("cell-into").str_value);
        
        file.write_uint64(trans->GetChildren().size());
        
        for (auto& point : trans->GetChildren()) {
            file.write_float32(point->GetProperty("position-x"));
            file.write_float32(point->GetProperty("position-y"));
            file.write_float32(point->GetProperty("position-z"));
        } 
        
        file.write_newline();
    }
}

}