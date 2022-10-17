#include <editor.h>
#include <iostream>
#include <fstream>

#include <components/rendercomponent.h>

#include <entities/crate.h>
#include <entities/lamp.h>
#include <entities/staticworldobject.h>
#include <moshkis.h>
#include <pickup.h>

namespace Editor {
    
    std::shared_ptr<Selection> selection;
    std::shared_ptr<WorldCellManager> worldcells;
    std::list<std::unique_ptr<Action>> performed_actions;
    /*
    std::vector<WorldCell*> worldCells;
    std::unordered_map<std::string, Core::SerializedEntityData* (*)(void)> entityDatas;
    
    void BENIS () {
        int i = 0;
        for (auto it = Editor::entityDatas.begin(); it != Editor::entityDatas.end(); it++, i++) {
            std::cout << std::string() + *it->first.c_str();
        }
    }
    
    void WorldCellIndirector::Show() {
        switch (indirection_type) {
            case CELL_ITSELF:
            case CELL_ENTITIES:
                for (auto ent : into->entities) ent->Show();
                into->is_visible = true;
                break;
            case CELL_ENTITY_GROUPS:
                for (auto gr : into->groups) for (auto ent : gr->entities) ent->Show();
                for (auto gr : into->groups) gr->is_visible = true;
                break;
            case CELL_TRANSITIONS:
                for (auto trans : into->transitions) trans->is_visible = true;
                into->is_transitions_visible = true;
                break;
            case CELL_PATHS:
                for (auto path : into->paths) path->is_visible = true;
                into->is_paths_visible = true;
                break;
            case CELL_NAVMESHES:
                for (auto navmesh : into->navmeshes) navmesh->is_visible = true;
                into->is_navmeshes_visible = true;
                break;
            case ENTITY_GROUP:
                for (auto ent : group->entities) ent->Show();
                group->is_visible = true;
                break;
            case TRANSITION:
                trans->is_visible = true;
                break;
            case PATH:
                path->is_visible = true;
                break;
            case NAVMESH:
                navmesh->is_visible = true;
                break;
            case ENTITY:
                entity->Show();
                break;
            default:
                std::cout << "'Show' is not implemented for this type" << std::endl;
        }
    }
    
    void WorldCellIndirector::Hide() {
        switch (indirection_type) {
            case CELL_ITSELF:
                for (auto ent : into->entities) ent->Hide();
                into->is_visible = false;
            case CELL_ENTITIES:

                break;
            case CELL_ENTITY_GROUPS:
                for (auto gr : into->groups) for (auto ent : gr->entities) ent->Hide();
                for (auto gr : into->groups) gr->is_visible = false;
                break;
            case CELL_TRANSITIONS:
                for (auto trans : into->transitions) trans->is_visible = false;
                into->is_transitions_visible = false;
                break;
            case CELL_PATHS:
                for (auto path : into->paths) path->is_visible = false;
                into->is_paths_visible = false;
                break;
            case CELL_NAVMESHES:
                for (auto navmesh : into->navmeshes) navmesh->is_visible = false;
                into->is_navmeshes_visible = false;
                break;
            case ENTITY_GROUP:
                for (auto ent : group->entities) ent->Hide();
                group->is_visible = false;
                break;
            case TRANSITION:
                trans->is_visible = false;
                break;
            case PATH:
                path->is_visible = false;
                break;
            case NAVMESH:
                navmesh->is_visible = false;
                break;
            case ENTITY:
                entity->Hide();
                break;
            default:
                std::cout << "'Hide' is not implemented for this type" << std::endl;
        }
    }
    
    bool WorldCellIndirector::IsVisible() {
        switch (indirection_type) {
            case CELL_ITSELF:
            case CELL_ENTITIES:
            case CELL_ENTITY_GROUPS:
                return into->is_visible;
            case CELL_TRANSITIONS:
                return into->is_transitions_visible;
            case CELL_PATHS:
                return into->is_paths_visible;
            case CELL_NAVMESHES:
                return into->is_navmeshes_visible;
            case ENTITY_GROUP:
                return group->is_visible;
            case TRANSITION:
                return trans->is_visible;
            case PATH:
                return path->is_visible;
            case NAVMESH:
                return navmesh->is_visible;
            case ENTITY:
                return entity->model;
            case TRANSITION_POINT:
                return point->parent->is_visible;
            case PATH_CURVE:
                return curve->parent->is_visible;
            case NAVMESH_NODE:
                return node->parent->is_visible;
            default:
                std::cout << "'IsVisible' is not implemented for this type" << std::endl;
                return false;
        }
    }
    
    bool WorldCellIndirector::IsNewable() {
        switch (indirection_type) {
            case NONE:
            case CELL_ITSELF:
            case CELL_ENTITIES:
            case CELL_ENTITY_GROUPS:
            case CELL_TRANSITIONS:
            case CELL_PATHS:
            case CELL_NAVMESHES:
            case ENTITY_GROUP:
            case TRANSITION:
            case PATH:
            case NAVMESH:
                return true;
            case ENTITY:
            case TRANSITION_POINT:
            case PATH_CURVE:
            case NAVMESH_NODE:
                return false;
        }
        return false;
    }
    
    bool WorldCellIndirector::IsEditable() {
        switch (indirection_type) {
            case NONE:
            case CELL_ENTITIES:
            case CELL_ENTITY_GROUPS:
            case CELL_TRANSITIONS:
            case CELL_PATHS:
            case CELL_NAVMESHES:
                return false;
            case CELL_ITSELF:
            case ENTITY_GROUP:
            case TRANSITION:
            case PATH:
            case NAVMESH:
            case ENTITY:
            case TRANSITION_POINT:
            case PATH_CURVE:
            case NAVMESH_NODE:
                return true;
        }
        return false;
    }
    
    bool WorldCellIndirector::IsDeletable() {
        switch (indirection_type) {
            case NONE:
            case CELL_ENTITIES:
            case CELL_ENTITY_GROUPS:
            case CELL_TRANSITIONS:
            case CELL_PATHS:
            case CELL_NAVMESHES:
                return false;
            case CELL_ITSELF:
            case ENTITY_GROUP:
            case TRANSITION:
            case PATH:
            case NAVMESH:
            case ENTITY:
            case TRANSITION_POINT:
            case PATH_CURVE:
            case NAVMESH_NODE:
                return true;
        }
        return false;
    }
    
    void WorldCellIndirector::Begonis() {
        std::cout << "BEGONISING CELL: " << into->name << std::endl;
    }
    
    WorldCellIndirector WorldCellIndirector::New() {
        WorldCellIndirector new_indirect;
        switch(indirection_type) {
            case NONE:
                new_indirect.indirection_type = CELL_ITSELF;
                new_indirect.into = new WorldCell { "jauns" };
                worldCells.push_back(new_indirect.into);
                break;
            case CELL_ITSELF:
                new_indirect.indirection_type = ENTITY;
                new_indirect.entity = new Entity {.id = 0, .name = "name", .action = "none", .parent = into };
                into->entities.push_back(new_indirect.entity);
                break;
            case CELL_ENTITIES:
                new_indirect.indirection_type = ENTITY_GROUP;
                new_indirect.group = new EntityGroup { .name = "name", .parent = into };
                into->groups.push_back(new_indirect.group);
                break;
            case CELL_ENTITY_GROUPS:
                break;
            case CELL_TRANSITIONS:
                new_indirect.indirection_type = TRANSITION;
                new_indirect.trans = new Transition { .name = "name", .cell_into_name = "none", .parent = into };
                into->transitions.push_back(new_indirect.trans);
                break;
            case CELL_PATHS:
                new_indirect.indirection_type = PATH;
                new_indirect.path = new Path { .name = "name", .parent = into };
                into->paths.push_back(new_indirect.path);
                break;
            case CELL_NAVMESHES:
                new_indirect.indirection_type = NAVMESH;
                new_indirect.navmesh = new Navmesh { .name = "name", .parent = into };
                into->navmeshes.push_back(new_indirect.navmesh);
                break;
            case ENTITY:
                new_indirect.indirection_type = ENTITY;
                new_indirect.entity = new Entity {.id = 0, .name = "name", .action = "none", .parent = entity->parent, .group = entity->group };
                if (!entity->group) entity->parent->entities.push_back(new_indirect.entity);
                if (entity->group) entity->group->entities.push_back(new_indirect.entity);
                break;
            case ENTITY_GROUP:
                new_indirect.indirection_type = ENTITY;
                new_indirect.entity = new Entity {.id = 0, .name = "name", .action = "none", .parent = group->parent, .group = group };
                group->entities.push_back(new_indirect.entity);
                break;
            case TRANSITION:
                new_indirect.indirection_type = TRANSITION_POINT;
                new_indirect.trans = trans;
                new_indirect.point = new Transition::Point { .parent = trans };
                trans->points.push_back(new_indirect.point);
                break;
            case PATH:
                new_indirect.indirection_type = PATH_CURVE;
                new_indirect.path = path;
                new_indirect.curve = new Path::Curve { .parent = path };
                path->curves.push_back(new_indirect.curve);
                break;
            case NAVMESH:
                new_indirect.indirection_type = NAVMESH_NODE;
                new_indirect.navmesh = navmesh;
                new_indirect.node = new Navmesh::Node { .parent = navmesh };
                navmesh->nodes.push_back(new_indirect.node);
                break;
            case TRANSITION_POINT:
                break;
            case PATH_CURVE:
                break;
            case NAVMESH_NODE:
                break;

        }
        return new_indirect;
    }
    

    

    
    void WorldCellIndirector::Delete() {
        switch(indirection_type) {
            case NONE:

                break;
            case CELL_ITSELF:
                worldCells.erase(std::find(worldCells.begin(), worldCells.end(), into));
                into->is_deleted = true;
                break;
            case CELL_ENTITIES:
                break;
            case CELL_ENTITY_GROUPS:
                break;
            case CELL_TRANSITIONS:
                break;
            case CELL_PATHS:
                break;
            case CELL_NAVMESHES:
                break;
            case ENTITY:
                if (entity->group) entity->group->entities.erase(std::find(entity->group->entities.begin(), entity->group->entities.end(), entity));
                entity->parent->entities.erase(std::find(entity->parent->entities.begin(), entity->parent->entities.end(), entity));
                break;
            case ENTITY_GROUP:
                group->parent->groups.erase(std::find(group->parent->groups.begin(), group->parent->groups.end(), group));
                break;
            case TRANSITION:
                trans->parent->transitions.erase(std::find(trans->parent->transitions.begin(), trans->parent->transitions.end(), trans));
                break;
            case PATH:
                path->parent->paths.erase(std::find(path->parent->paths.begin(), path->parent->paths.end(), path));
                break;
            case NAVMESH:
                navmesh->parent->navmeshes.erase(std::find(navmesh->parent->navmeshes.begin(), navmesh->parent->navmeshes.end(), navmesh));
                break;
            case TRANSITION_POINT:
                point->parent->points.erase(std::find(point->parent->points.begin(), point->parent->points.end(), point));
                break;
            case PATH_CURVE:
                curve->parent->curves.erase(std::find(curve->parent->curves.begin(), curve->parent->curves.end(), curve));
                break;
            case NAVMESH_NODE:
                node->parent->nodes.erase(std::find(node->parent->nodes.begin(), node->parent->nodes.end(), node));
                break;

        }
    }
    
    WorldCellIndirector WorldCellIndirector::Index(long index) {
        WorldCellIndirector new_indirect;
        switch(indirection_type) {
            case NONE:
                break;
            case CELL_ITSELF:
                new_indirect.indirection_type = ENTITY;
                new_indirect.entity = into->entities[index];
                break;
            case ENTITY:
                new_indirect.indirection_type = ENTITY;
                if (entity->group) new_indirect.entity = entity->group->entities[index];
                else new_indirect.entity = entity->parent->entities[index];
                break;
            case CELL_ENTITIES:
                break;
            case CELL_ENTITY_GROUPS:
                break;
            case CELL_TRANSITIONS:
                break;
            case CELL_PATHS:
                break;
            case CELL_NAVMESHES:
                break;
            case ENTITY_GROUP:
                new_indirect.indirection_type = ENTITY;
                new_indirect.entity = group->entities[index];
                break;
            case TRANSITION:
                new_indirect.indirection_type = TRANSITION_POINT;
                new_indirect.point = trans->points[index];
                break;
            case PATH:
                new_indirect.indirection_type = PATH_CURVE;
                new_indirect.curve = path->curves[index];
                break;
            case NAVMESH:
                new_indirect.indirection_type = NAVMESH_NODE;
                new_indirect.node = navmesh->nodes[index];
                break;
            case TRANSITION_POINT:
                new_indirect.indirection_type = TRANSITION_POINT;
                new_indirect.point = point->parent->points[index];
                break;
            case PATH_CURVE:
                new_indirect.indirection_type = PATH_CURVE;
                new_indirect.curve = curve->parent->curves[index];
                break;
            case NAVMESH_NODE:
                new_indirect.indirection_type = NAVMESH_NODE;
                new_indirect.node = node->parent->nodes[index];
                break;
        }
        return new_indirect;
    }
    
    WorldCell* NewWorldCell() {
        auto cell = new WorldCell{"jauns"};
        worldCells.push_back(cell);
        return cell;
    }
    
    Entity* WorldCell::NewEntity() {
        auto ent = new Entity {.id = 0, .name = "name", .action = "none", .parent = this };
        entities.push_back(ent);
        return ent;
    }
    
    Transition* WorldCell::NewTransition() {
        auto trans = new Transition { .name = "transition", .parent = this };
        transitions.push_back(trans);
        return trans;
    }
    
    Transition::Point* Transition::NewPoint() {
        auto point = new Point;
        points.push_back(point);
        return point;
    }
    
    EntityGroup* WorldCell::NewEntityGroup() {
        auto group = new EntityGroup;
        groups.push_back(group);
        return group;
    }
    
    Entity* EntityGroup::NewEntity() {
        auto entity = parent->NewEntity();
        entities.push_back(entity);
        entity->group = this;
        return entity;
    }
    
    Path* WorldCell::NewPath() {
        auto path = new Path { .name = "path", .parent = this };
        paths.push_back(path);
        return path;
    }
    
    Path::Curve* Path::NewCurve() {
        auto curve = new Curve;
        curves.push_back(curve);
        return curve;
    }
    
    Navmesh* WorldCell::NewNavmesh() {
        auto navmesh = new Navmesh { .name = "navmesh", .parent = this };
        navmeshes.push_back(navmesh);
        return navmesh;
    }
    
    Navmesh::Node* Navmesh::NewNode() {
        auto node = new Node;
        nodes.push_back(node);
        return node;
    }
    
    void WorldCell::DeleteEntity(size_t id) {
        entities.erase(entities.begin() + id);
    }
    
    void WorldCell::DeleteTransition(Transition* transition) {
        transitions.erase(std::find(transitions.begin(), transitions.end(), transition));
    }
    
    void WorldCell::DeleteEntityGroup(EntityGroup* group) {
        groups.erase(std::find(groups.begin(), groups.end(), group));
    }
    
    void WorldCell::DeletePath(Path* path) {
        paths.erase(std::find(paths.begin(), paths.end(), path));
    }
    
    void WorldCell::DeleteNavmesh(Navmesh* navmesh) {
        navmeshes.erase(std::find(navmeshes.begin(), navmeshes.end(), navmesh));
    }
    
    void Transition::DeletePoint(Transition::Point* point) {
        points.erase(std::find(points.begin(), points.end(), point));
    }
    
    void EntityGroup::DeleteEntity(Entity* entity) {
        entities.erase(std::find(entities.begin(), entities.end(), entity));
        parent->entities.erase(std::find(entities.begin(), entities.end(), entity));
    }
    
    void Path::DeleteCurve(Curve* curve) {
        curves.erase(std::find(curves.begin(), curves.end(), curve));
    }
    
    void Navmesh::DeleteNode(Node* node) {
        nodes.erase(std::find(nodes.begin(), nodes.end(), node));
    }
    
    
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
    }
    
    void WorldCell::Load() {
        std::ifstream file(std::string("data/") + name + ".cell");
        entities.clear();
        groups.clear();
        transitions.clear();
        paths.clear();
        navmeshes.clear();
        EntityGroup* current_group = nullptr;
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                if (line.size() < 3) continue;
                std::string_view str (line);
                std::string ent_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                if (ent_name == "#") {
                    std::string annotation_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                    if (annotation_name == "cell") {
                        is_interior = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                        is_interior_lighting = Core::SerializedEntityData::Field<uint64_t>().FromString(str);
                    } else if (annotation_name == "group") {
                        current_group = new EntityGroup;
                        current_group->name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                        current_group->parent = this;
                        groups.push_back(current_group);
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
        
/*
        entityDatas[(new Core::Crate::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::Crate::Data; d->collmodel = 0; d->model = 0; return d; };
        entityDatas[(new Core::Lamp::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::Lamp::Data; return d; };
        entityDatas[(new Core::StaticWorldObject::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::StaticWorldObject::Data; d->lightmap = 0; d->model = 0; return d; };
        entityDatas[(new Core::Moshkis::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::Moshkis::Data; return d; };
        entityDatas[(new Core::Pickup::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::Pickup::Data; return d; };
        
        std::ifstream file("data/editor_data.txt");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {

                worldCells.push_back(new WorldCell {line});
            }
        } else {
            std::cout << "Can't find the editor config file." << std::endl;
        }*/
    }
}