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
                }
            }
            
            ((Object*)current_group)->AddChild(entity);
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
    
    file.write_name ("CELLv2");
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
            
            file.write_uint64(ent->GetProperty("id"));
            file.write_name(ent->GetProperty("name").str_value);
            
            file.write_float32(ent->GetProperty("position-x"));
            file.write_float32(ent->GetProperty("position-y"));
            file.write_float32(ent->GetProperty("position-z"));
            
            file.write_float32(ent->GetProperty("rotation-x"));
            file.write_float32(ent->GetProperty("rotation-y"));
            file.write_float32(ent->GetProperty("rotation-z"));
            
            auto data_defs = std::dynamic_pointer_cast<Entity>(ent)->GetSerializationPropertyDefinitions();
            
            bool wrote = false;
            
            for (auto& prop : data_defs) {
                switch (prop.type) {
                    default: // TODO: actually implement writers for other types
                    case PROPERTY_STRING:
                        file.write_name(ent->GetProperty(prop.name).str_value); wrote = true; break;
                    case PROPERTY_INT:
                        file.write_int64(ent->GetProperty(prop.name)); wrote = true; break;
                    case PROPERTY_UINT:
                        file.write_uint64(ent->GetProperty(prop.name)); wrote = true; break;
                    case PROPERTY_FLOAT:
                        file.write_float32(ent->GetProperty(prop.name)); wrote = true; break;
                    case PROPERTY_CATEGORY:
                        break;
                }
            }
            
            // workaround
            if (!wrote) {
                file.write_name("IGNORE_THIS_TEXT");
            }
            
            file.write_newline();
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