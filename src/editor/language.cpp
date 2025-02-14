#include <editor/language.h>
#include <editor/settings.h>

#include <unordered_map>

#include <iostream>

static std::unordered_map<std::string, std::wstring> l;

void Editor::ResetLanguage() {
    l.clear();
    
    switch (Editor::Settings::INTERFACE_LANGUAGE) {
        
        case Editor::Settings::LANGUAGE_EN:
            l["world_tree"] = L"World";
            l["property_panel"] = L"Properties";
            l["object_list"] = L"Objects";
            l["output_text"] = L"Console";
            
            l["world"] = L"World";
            l["worldcells"] = L"WorldCells";
            l["entity-manager"] = L"Entity Manager";
            l["transitions"] = L"Transitions";
            l["paths"] = L"Paths";
            l["navmeshes"] = L"Navmeshes";
            
            l["dialog_loading_info"] = L"Loading cell...";
            l["dialog_loading_title"] = L"Loading cells...";
            l["dialog_loading_cell"] = L"Currently processing cell: ";
            l["dialog_finished"] = L"Finished!";
            
            l["title_bar"] = L"Tramway SDK Level Editing App";
            
            l["file_menu_load"] = L"Load";
            l["file_menu_load_info"] = L"Loads all worldcells and other data files.";
            l["file_menu_save"] = L"Save\tCtrl-S";
            l["file_menu_save_info"] = L"Saves all worldcells and other data files.";
            l["file_menu_quit"] = L"Quit\tF1";
            l["file_menu_quit_info"] = L"Exits the program.";
            
            l["edit_menu_undo"] = L"Undo\tCtrl-Z";
            l["edit_menu_undo_info"] = L"Reverses the previously committed action.";
            l["edit_menu_redo"] = L"Redo\tCtrl-Y";
            l["edit_menu_undo_info"] = L"Performs again a previously reversed action.";

            l["edit_menu_duplicate"] = L"Duplicate";
            l["edit_menu_duplicate_info"] = L"Duplicates the selected objects.";
            l["edit_menu_signals"] = L"Signals";
            l["edit_menu_signals_info"] = L"Opens the signal editor for the selected entity.";
            l["edit_menu_center_aabb"] = L"Center Origin on AABB";
            l["edit_menu_center_aabb_info"] = L"Centers the origin of an entity on its 3D model's AABB.";
            l["edit_menu_worldspawn_offset"] = L"Set Worldspawn Offset";
            l["edit_menu_worldspawn_offset_info"] = L"TODO: fill in";
            l["edit_menu_stash_selection"] = L"Stash Selection\tCtrl-X";
            l["edit_menu_stash_selection_info"] = L"TODO: fill in";
            l["edit_menu_reparent_stash"] = L"Reparent Objects\tCtrl-V";
            l["edit_menu_reparent_stash_info"] = L"TODO: fill in";
            
            l["view_menu_center_selection"] = L"Center View on Selection";
            l["view_menu_center_selection_info"] = L"Centers the vieport on the selected object.";
            
            l["settings_menu_world_space"] = L"World Space";
            l["settings_menu_world_space_info"] = L"Performs translations and rotations in world space.";
            l["settings_menu_entity_space"] = L"Entity Space";
            l["settings_menu_entity_space_info"] = L"Performs translations and rotations in entity space.";
            l["settings_menu_entity_group_space"] = L"Group Space";
            l["settings_menu_entity_group_space_info"] = L"Performs translations and rotations in group space.";
            
            l["settings_menu_radians"] = L"Radians";
            l["settings_menu_radians_info"] = L"Displays rotations in radians.";
            l["settings_menu_degrees"] = L"Degrees";
            l["settings_menu_degrees_info"] = L"Displays rotations in degrees.";

            l["snap"] = L"Snap";
            
            l["snap_to_0_01"] = L"Snap to 0.01m";
            l["snap_to_0_10"] = L"Snap to 0.10m";
            l["snap_to_0_25"] = L"Snap to 0.25m";
            l["snap_to_0_50"] = L"Snap to 0.50m";
            l["snap_to_1_00"] = L"Snap to 1.00m";
            
            l["snap_to_15"] = L"Snap to 15°";
            l["snap_to_30"] = L"Snap to 30°";
            l["snap_to_45"] = L"Snap to 45°";
            l["snap_to_90"] = L"Snap to 90°";
            
            l["help_menu_help"] = L"Documentation";
            l["help_menu_help_info"] = L"Get help.";
            l["help_menu_about"] = L"About";
            l["dialog_about_name"] = L"Tramway SDK Level Editing Program";
            l["dialog_description"] = L"This program edits levels.";
            l["dialog_copyright"] = L"Tramway Editor";
            l["dialog_data_loss"] = L"Warning! Possible data loss!";
            l["dialog_save_data"] = L"There are some unsaved datas. Do you want to save the datas before exiting?";
            l["dialog_load_data"] = L"There are some unsaved datas. Do you want to yeet them permanently?";
            
            l["dialog_show"] = L"Show";
            l["dialog_add_new"] = L"Add New";
            l["dialog_edit"] = L"Edit";
            l["dialog_duplicate"] = L"Duplicate";
            l["dialog_signals"] = L"Signals";
            l["dialog_delete"] = L"Delete";
            
            l["info_ready"] = L"Ready.";
            
            l["file_menu"] = L"File";
            l["edit_menu"] = L"Edit";
            l["view_menu"] = L"View";
            l["settings_menu"] = L"Settings";
            l["language_menu"] = L"Language";
            l["help_menu"] = L"Help";

            
            
            l["interior"] = L"Interior";
            l["interior_lighting"] = L"Interior Lighting";
            l["name"] = L"Name";
            l["x"] = L"X";
            l["y"] = L"Y";
            l["z"] = L"Z";
            l["entity_group"] = L"Entity Group";
            l["entity_group_manager"] = L"Entity Groups";
            l["worldcell_manager"] = L"WorldCells";
            l["transition_manager"] = L"Transitions";
            l["path_manager"] = L"Paths";
            l["navmesh_manager"] = L"Navmeshes";
            l["path"] = L"Path";
            l["transition"] = L"Transition";
            l["entity_group"] = L"Entity Group";
            l["color-r"] = L"Color R";
            l["color-g"] = L"Color G";
            l["color-b"] = L"Color B";
            l["distance"] = L"Distance";
            l["direction-x"] = L"Direction X";
            l["direction-y"] = L"Direction Y";
            l["direction-z"] = L"Direction Z";
            l["exponents"] = L"Exponents";
            
            break;
            
        case Editor::Settings::LANGUAGE_LV:

 
            break;
    }

}

std::wstring Editor::Get(const std::string& name) {
    auto it = l.find(name);
    
    if (it != l.end()) {
        return it->second;
    } else {
        return std::wstring(name.begin(), name.end());
    }
}