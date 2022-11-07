#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>

namespace Editor {
    struct Language {
        std::wstring title_bar = L"Tramvaju drifta un pagrabu pētīšanas simulatoru izstrādes rīkkopas līmeņu rediģējamā lietotne";
        
        std::wstring file_menu = L"Fails";
        std::wstring file_menu_load = L"Ielādēt šūnas";
        std::wstring file_menu_load_info = L"Ielādē šūnas no diska.";
        std::wstring file_menu_save = L"Saglabāt šūnas";
        std::wstring file_menu_save_info =  L"Saglabā atvērtās šūnas.";
        std::wstring file_menu_quit = L"Beidzēt\tCtrl-Q";
        std::wstring file_menu_quit_info = L"Izbeigt programmu.";
        
        std::wstring edit_menu = L"Rediģēt";
        std::wstring edit_menu_undo = L"Atpakaļināties\tCtrl-Z";
        std::wstring edit_menu_undo_info = L"Atceļ pēdējās izmaiņas.";
        std::wstring edit_menu_redo = L"Atpriekšināties\tCtrl-Y";
        std::wstring edit_menu_redo_info = L"Atkal izpilda atceltās izmaiņas.";
        
        std::wstring settings_menu = L"Iestatījumi";
        std::wstring settings_menu_world_space = L"Pasaules telpa";
        std::wstring settings_menu_world_space_info = L"Rotācijas un relokācijas notiek pasaules telpā.";
        std::wstring settings_menu_entity_space = L"Entītijas telpa";
        std::wstring settings_menu_entity_space_info = L"Rotācijas un relokācijas notiek entītijas telpā.";
        std::wstring settings_menu_entity_group_space = L"Grupas telpa";
        std::wstring settings_menu_entity_group_space_info = L"Rotācijas un relokācijas notiek entītiju grupas telpā.";
        std::wstring settings_menu_radians = L"Radiāni";
        std::wstring settings_menu_radians_info = L"Rotācijas norādītas radiānos.";
        std::wstring settings_menu_degrees = L"Grādi";
        std::wstring settings_menu_degrees_info = L"Rotācijas norādītas grādos.";
        
        std::wstring language_menu = L"Valoda";
        
        std::wstring help_menu = L"Palīdzība";
        std::wstring help_menu_hello = L"Labrīt";
        std::wstring help_menu_hello_info = L"Labrīt.";
        std::wstring help_menu_about = L"Par";
        std::wstring help_menu_about_info = L"Par programmu.";
        
        std::wstring info_ready = L"Gatavs.";
        
        std::wstring world_tree = L"Pasaule";
        std::wstring property_panel = L"Īpašības";
        std::wstring object_list = L"Saturs";
        std::wstring output_text = L"Konsole";
        std::wstring dialog_saving_title = L"Saglabā šūnas";
        std::wstring dialog_saving_info = L"Notiek šūnu saglabāšana";
        std::wstring dialog_saving_cell = L"Saglabā ";

        std::wstring dialog_loading_title = L"Ielādē šūnas";
        std::wstring dialog_loading_info = L"Notiek šūnu ielāde";
        std::wstring dialog_loading_cell = L"Ielādē ";

        std::wstring dialog_finished = L"Pabeigts.";

        std::wstring dialog_data_loss = L"Iespējams datu zudums!";
        std::wstring dialog_save_data = L"Jūs esat veicis izmaiņas. Šīs izmaiņas nav saglabātas. Saglabāt izmaiņas?";
        std::wstring dialog_load_data = L"Jūs esat veicis izmaiņas. Ielādējot šūnas no diska šīs izmaiņas tiks zaudētas. Turpināt?";

        std::wstring dialog_about_name = L"Līmeņu rediģējamā programma";
        std::wstring dialog_description = L"Episkā līmeņu programmma.";
        std::wstring dialog_copyright = L"Autortiesības";
        
        std::wstring dialog_cell_not_found = L"Can't find the cell file ";
        std::wstring dialog_cell_not_write = L"Can't write to the cell file ";
        std::wstring dialog_settings_not_found = L"Settings file was not found!";

        std::wstring dialog_show = L"Rādīt";
        std::wstring dialog_add_new = L"Pievienot jaunu";
        std::wstring dialog_edit = L"Rediģēt";
        std::wstring dialog_delete = L"Dzēst";
        
        std::wstring dialog_change_language_title = L"Valodas maiņa";
        std::wstring dialog_change_language = L"Daži grafiskās saskarnes elementi nomainīs savu valodu tikai pēc programmas pārstartēšanas.";
    };
    
    extern Language* selected_language;
    
    static Language Language_LV;
    static Language Language_EN = {
        .title_bar = L"Tramway SDK Level Editing App",
        
        .file_menu = L"File",
        .file_menu_load = L"Load cells",
        .file_menu_load_info = L"Loads cells from disk.",
        .file_menu_save = L"Save cells",
        .file_menu_save_info =  L"Saves open cells.",
        .file_menu_quit = L"Quit\tCtrl-Q",
        .file_menu_quit_info = L"Stops the program.",
        
        .edit_menu = L"Edit",
        .edit_menu_undo = L"Undo\tCtrl-Z",
        .edit_menu_undo_info = L"Reverts the last change.",
        .edit_menu_redo = L"Redo\tCtrl-Y",
        .edit_menu_redo_info = L"Reverts the last reverted change.",
        
        .settings_menu = L"Settings",
        .settings_menu_world_space = L"World space",
        .settings_menu_world_space_info = L"Transforms happen in world space.",
        .settings_menu_entity_space = L"Entity space",
        .settings_menu_entity_space_info = L"Transforms happen in entity space.",
        .settings_menu_entity_group_space = L"Group space",
        .settings_menu_entity_group_space_info = L"Transforms happen in entity space.",
        .settings_menu_radians = L"Radians",
        .settings_menu_radians_info = L"Rotations are displayed in radians.",
        .settings_menu_degrees = L"Degrees",
        .settings_menu_degrees_info = L"Rotations are displayed in degrees.",
        
        .language_menu = L"Language",
        
        .help_menu = L"Help",
        .help_menu_hello = L"Hello",
        .help_menu_hello_info = L"Hello.",
        .help_menu_about = L"About",
        .help_menu_about_info = L"About the program.",
        
        .info_ready = L"Ready.",
        
        .world_tree = L"World",
        .property_panel = L"Properties",
        .object_list = L"Objects",
        .output_text = L"Console",
        .dialog_saving_title = L"Saving cells",
        .dialog_saving_info = L"The cells are being saved",
        .dialog_saving_cell = L"Saving ",

        .dialog_loading_title = L"Loading cells",
        .dialog_loading_info = L"The cells are being loaded",
        .dialog_loading_cell = L"Loading ",

        .dialog_finished = L"Done.",

        .dialog_data_loss = L"Possible data loss!",
        .dialog_save_data = L"Your changes have not been saved. Save the changes before quitting?",
        .dialog_load_data = L"Your changes have not been saved. By loading the cells, your changes will be lost. Continue?",

        .dialog_about_name = L"Level editing program",
        .dialog_description = L"Epic level editing program.",
        .dialog_copyright = L"Copyright",
        
        .dialog_cell_not_found = L"Can't find the cell file ",
        .dialog_cell_not_write = L"Can't write to the cell file ",
        .dialog_settings_not_found = L"Settings file was not found!",

        .dialog_show = L"Show",
        .dialog_add_new = L"Add new",
        .dialog_edit = L"Edit",
        .dialog_delete = L"Remove",
        
        .dialog_change_language_title = L"Language change",
        .dialog_change_language = L"Some graphical interface elements will change their language only after restarting the program."
    };
    
    extern Language* Languages[2];
    
    void ResetRename();
    std::wstring PropertyRename (const std::string& name);
}

#endif // LANGUAGE_H