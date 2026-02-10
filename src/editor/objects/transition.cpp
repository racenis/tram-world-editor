#include <editor/objects/transition.h>

namespace Editor {

using namespace tram;

std::vector<WidgetDefinition> Transition:: GetWidgetDefinitions() {
    std::vector<WidgetDefinition> widgets;
    
    for (auto& child_from : children) {
        std::shared_ptr<Transition::Node> node_from = std::dynamic_pointer_cast<Transition::Node>(child_from);
        
        vec3 from = {
            node_from->GetProperty("position-x"),
            node_from->GetProperty("position-y"),
            node_from->GetProperty("position-z")
        };
        
        for (auto& child_to : children) {
            std::shared_ptr<Transition::Node> node_to = std::dynamic_pointer_cast<Transition::Node>(child_to);
            
            vec3 to = {
                node_to->GetProperty("position-x"),
                node_to->GetProperty("position-y"),
                node_to->GetProperty("position-z")
            };
            
            widgets.push_back(WidgetDefinition::Line(from, to, WidgetDefinition::WIDGET_CYAN));
            
           // Render::AddLine(from, to, Render::COLOR_CYAN);
        }
        
        //Render::AddLineAABB({-0.1f, -0.1f, -0.1f}, {0.1f, 0.1f, 0.1f}, from, vec3(0.0f, 0.0f, 0.0f), Render::COLOR_GREEN);
    }
    
    return widgets;
}

void Transition::Draw() {
    /*for (auto& child_from : children) {
        std::shared_ptr<Transition::Node> node_from = std::dynamic_pointer_cast<Transition::Node>(child_from);
        
        vec3 from = {
            node_from->GetProperty("position-x"),
            node_from->GetProperty("position-y"),
            node_from->GetProperty("position-z")
        };
        
        for (auto& child_to : children) {
            std::shared_ptr<Transition::Node> node_to = std::dynamic_pointer_cast<Transition::Node>(child_to);
            
            vec3 to = {
                node_to->GetProperty("position-x"),
                node_to->GetProperty("position-y"),
                node_to->GetProperty("position-z")
            };
            
            Render::AddLine(from, to, Render::COLOR_CYAN);
        }
        
        Render::AddLineAABB({-0.1f, -0.1f, -0.1f}, {0.1f, 0.1f, 0.1f}, from, vec3(0.0f, 0.0f, 0.0f), Render::COLOR_GREEN);
    }*/
    
}

std::vector<WidgetDefinition> Transition::Node::GetWidgetDefinitions() {
    return {WidgetDefinition::SelectionBox({0.1f, 0.1f, 0.1f}, WidgetDefinition::WIDGET_GREEN)};
}

void Transition::Node::Draw() {
    /*parent->Draw();
    
    vec3 pos = {
        this->GetProperty("position-x"),
        this->GetProperty("position-y"),
        this->GetProperty("position-z")
    };
    
    Render::AddLineAABB({-0.2f, -0.2f, -0.2f}, {0.2f, 0.2f, 0.2f}, pos, vec3(0.0f, 0.0f, 0.0f), Render::COLOR_RED);*/
}

}