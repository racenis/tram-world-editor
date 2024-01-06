#include <widgets/dialogs.h>

#include <editor/editor.h>
#include <editor/language.h>
#include <editor/settings.h>
#include <editor/objects/world.h>

#include <widgets/mainframe.h>

#include <wx/progdlg.h>

static auto& lang = Editor::selected_language;

// Shows "loading cells" progress bar and loads cells.
void LoadCells() {
    if (Editor::data_modified) {
        wxMessageDialog confirmation (main_frame, lang->dialog_load_data, lang->dialog_data_loss, wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_EXCLAMATION);
        if(confirmation.ShowModal() != wxID_YES) {
            return;
        }
    }
    
    Editor::Reset();
        
    wxProgressDialog progress_dialog(lang->dialog_loading_title, lang->dialog_loading_info, 100, main_frame);
    
    auto paths = Editor::WORLD->path_manager->GetChildren();
    auto navmeshes = Editor::WORLD->navmesh_manager->GetChildren();
    auto cells = Editor::WORLD->worldcell_manager->GetChildren();
    
    float progress = 0;
    float progress_increment = ((float) 100 / (float) (paths.size() + navmeshes.size() + cells.size()));
    
    for (auto& wpath : paths) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wpath->GetName()));
        std::dynamic_pointer_cast<Editor::Path>(wpath)->LoadFromDisk();
        progress += progress_increment;
    }
    
    for (auto& wnavmesh : navmeshes) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wnavmesh->GetName()));
        std::dynamic_pointer_cast<Editor::Navmesh>(wnavmesh)->LoadFromDisk();
        progress += progress_increment;
    }
    
    for (auto& wcell : cells) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wcell->GetName()));
        std::dynamic_pointer_cast<Editor::WorldCell>(wcell)->LoadFromDisk();
        progress += progress_increment;
    }
    
    Editor::WorldTree::Rebuild();
    
    progress_dialog.Update(100, lang->dialog_finished);
    
    std::cout << lang->dialog_finished << std::endl;
}

// Shows "saving cells" progress bar and saves cells.
void SaveCells() {
    wxProgressDialog progress_dialog (lang->dialog_saving_title, lang->dialog_saving_info, 100, main_frame);
    
    auto paths = Editor::WORLD->path_manager->GetChildren();
    auto navmeshes = Editor::WORLD->navmesh_manager->GetChildren();
    auto cells = Editor::WORLD->worldcell_manager->GetChildren();
    
    float progress = 0;
    float progress_increment = ((float) 100 / (float) (paths.size() + navmeshes.size() + cells.size()));
    
    for (auto& wpath : paths) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wpath->GetName()));
        std::dynamic_pointer_cast<Editor::Path>(wpath)->SaveToDisk();
        progress += progress_increment;
    }
    
    for (auto& wnavmesh : navmeshes) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wnavmesh->GetName()));
        std::dynamic_pointer_cast<Editor::Navmesh>(wnavmesh)->SaveToDisk();
        progress += progress_increment;
    }
    
    for (auto& wcell : cells) {
        progress_dialog.Update(progress, lang->dialog_loading_cell + std::string(wcell->GetName()));
        std::dynamic_pointer_cast<Editor::WorldCell>(wcell)->SaveToDisk();
        progress += progress_increment;
    }
    
    Editor::Settings::Save();
    
    progress_dialog.Update(100, lang->dialog_finished);
    
    std::cout << lang->dialog_finished << std::endl;
}
