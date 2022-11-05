#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>

namespace Editor {
    struct Language {
        std::wstring title_bar = L"Tramvaju drifta un pagrabu pētīšanas simulatoru izstrādes rīkkopas līmeņu rediģējamā lietotne";
        
        std::wstring file_menu = L"Fails";
        std::wstring file_menu_load = L"Ielādēt šūnas";
        std::wstring file_menu_load_info = L"Ielādē šūnas kas atrodas konfigurācijas failā.";
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
    };
    
    extern Language* selected_language;
    
    static Language Language_LV;
    static Language Language_EN = {
        .title_bar = L"Tramway SDK Level Editing App"
    };
    
    static Language* Languages[2] = {&Language_LV, &Language_EN};
}

#endif // LANGUAGE_H