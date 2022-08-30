#include <editor.h>
#include <iostream>
#include <fstream>

#include <components/rendercomponent.h>

#include <entities/crate.h>
#include <entities/lamp.h>
#include <entities/staticworldobject.h>

namespace Editor {
    std::vector<WorldCell*> worldCells;
    //std::vector<std::pair<std::string, Core::SerializedEntityData* (*)(void)>> entityDatasSorted;
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
            default:
                std::cout << "'Show' is not implemented for this type" << std::endl;
        }
    }
    
    void WorldCellIndirector::Hide() {
        switch (indirection_type) {
            case CELL_ITSELF:
            case CELL_ENTITIES:
                for (auto ent : into->entities) ent->Hide();
                into->is_visible = false;
                break;
            default:
                std::cout << "'Hide' is not implemented for this type" << std::endl;
        }
    }
    
    bool WorldCellIndirector::IsVisible() {
        switch (indirection_type) {
            case CELL_ITSELF:
            case CELL_ENTITIES:
                return into->is_visible;
                break;
            default:
                std::cout << "'IsVisible' is not implemented for this type" << std::endl;
                return false;
        }
    }
    
    void WorldCellIndirector::Begonis() {
        std::cout << "BEGONISING CELL: " << into->name << std::endl;
    }
    
    void WorldCellIndirector::Delete() {
        std::cout << "YEETING CELL: " << into->name << std::endl;
        for (auto it = worldCells.begin(); it < worldCells.end(); it++) {
            if (*it == into) {
                worldCells.erase(it);
                return;
            }
        }
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
        model->SetPose(Core::Render::poseList.begin());
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
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::string_view str (line);
                std::string ent_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                auto ent = new Entity {.id = 0, .parent = this};
                ent->FromString(str);
                ent->ent_data = entityDatas[ent_name]();
                ent->ent_data->FromString(str);
                entities.push_back(ent);
            }
        } else {
            std::cout << "Can't find the cell file " << name << ".cell." << std::endl;
        }
    }
    
    void WorldCell::Save() { 
        
        std::ofstream file (std::string("data/") + name + ".cell");
        
        if (file.is_open()) {
            std::string line;
            for (auto ent : entities) {
                std::string output_line = ent->ent_data->GetDataName();
                ent->ToString(output_line);
                if (ent->ent_data) ent->ent_data->ToString(output_line);
                file << output_line << std::endl;
            }
        } else {
            std::cout << "Can't write to the cell file " << name << ".cell." << std::endl;
        }
    }
    
    void Init(){
        Core::Init();
        //auto cell1 = new WorldCell{"chongon"};
        //auto cell2 = new WorldCell{"benigon"};
        //auto cell3 = new WorldCell{"dongon"};
        //auto cell4 = new WorldCell{"dingodongo"};
        
        //worldCells.push_back(cell1);
        //worldCells.push_back(cell2);
        //worldCells.push_back(cell3);
        //worldCells.push_back(cell4);
        
        //cell1->entities.push_back(new Entity{0, "benis"});
        //cell1->entities.push_back(new Entity{4, "impressive"});
        //cell1->entities.push_back(new Entity{5, "nice"});
        
        //cell2->entities.push_back(new Entity{1, "benisoner"});
        
        entityDatas[(new Core::Crate::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::Crate::Data; d->collmodel = 0; d->model = 0; return d; };
        entityDatas[(new Core::Lamp::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::Lamp::Data; return d; };
        entityDatas[(new Core::StaticWorldObject::Data)->GetDataName()] = []() -> Core::SerializedEntityData* { auto d = new Core::StaticWorldObject::Data; d->lightmap = 0; d->model = 0; return d; };
        
        //entityDatasSorted.push_back(std::pair(Core::Crate::data_name, []() -> Core::SerializedEntityData* { auto d = new Core::Crate::Data; d->collmodel = 0; d->model = 0; return d; }));
        //entityDatasSorted.push_back(std::pair(Core::StaticWorldObject::data_name, []() -> Core::SerializedEntityData* { auto d = new Core::StaticWorldObject::Data; d->lightmap = 0; d->model = 0; return d; }));
        
        std::ifstream file("data/editor_data.txt");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                //std::string_view str (line);
                //std::string ent_name = Core::ReverseUID(Core::SerializedEntityData::Field<Core::name_t>().FromStringAsName(str));
                worldCells.push_back(new WorldCell {line});
            }
        } else {
            std::cout << "Can't find the editor config file." << std::endl;
        }
    }
}