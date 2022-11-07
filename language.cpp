#include <language.h>
#include <editor.h>
#include <unordered_map>

Editor::Language* Editor::Languages[2] = {&Language_LV, &Language_EN};
Editor::Language* Editor::selected_language = &Editor::Language_LV;
std::unordered_map<std::string, std::wstring> rename_map;

void Editor::ResetRename() {
    rename_map.clear();
    
    if (Editor::Settings::INTERFACE_LANGUAGE == Editor::Settings::LANGUAGE_LV) {
        rename_map["New Entity"] = L"Jauna Entītija";
        rename_map["New Entity Group"] = L"Jauna Entītiju Grupa";
        rename_map["Name"] = L"Vārds";
        rename_map["Into"] = L"Ieeja";
        rename_map["Entity"] = L"Entītija";
        rename_map["Action"] = L"Darbība";
        rename_map["Position"] = L"Pozīcija";
        rename_map["Rotation"] = L"Rotācija";
        rename_map["Entity Type"] = L"Entītijas tips";
        rename_map["Entities"] = L"Entītijas";
        rename_map["[default]"] = L"[noklusējuma]";
        rename_map["Entity Group"] = L"Entītiju grupa";
        rename_map["Entity Group Manager"] = L"Entītiju grupu pārvaldnieks";
        rename_map["Transition Node"] = L"Pārejas mezgls";
        rename_map["Transition"] = L"Pāreja";
        rename_map["New Transition"] = L"Jauna pāreja";
        rename_map["Transitions"] = L"Pārejas";
        rename_map["Transition Manager"] = L"Pāreju pārvaldnieks";
        rename_map["Path"] = L"Ceļš";
        rename_map["New Path"] = L"Jauns ceļš";
        rename_map["Paths"] = L"Ceļi";
        rename_map["Path Node"] = L"Ceļa mezgls";
        rename_map["ID"] = L"ID";
        rename_map["Next ID"] = L"Nāk. ID";
        rename_map["Prev ID"] = L"Iepr. ID";
        rename_map["Left ID"] = L"Kr. ID";
        rename_map["Right ID"] = L"Lab. ID";
        rename_map["Position 1"] = L"1. Pozīcija";
        rename_map["Position 2"] = L"2. Pozīcija";
        rename_map["Position 3"] = L"3. Pozīcija";
        rename_map["Position 4"] = L"4. Pozīcija";
        rename_map["Path Manager"] = L"Ceļu pārvaldnieks";
        
        rename_map["Navmesh"] = L"Nav. tīkls";
        rename_map["New Navmesh"] = L"Jauns nav. tīkls";
        rename_map["Navmeshes"] = L"Nav. tīkls";
        rename_map["Navmesh Node"] = L"Nav. tīkla mezgls";
        rename_map["Navmesh Manager"] = L"Nav. tīklu pārvaldnieks";

        rename_map["Worldcell"] = L"Šūna";
        rename_map["New Worldcell"] = L"Jauna šūna";
        rename_map["Worldcells"] = L"Šūnas";
        rename_map["Worldcell Manager"] = L"Šūnu pārvaldnieks";
        rename_map["Interior"] = L"Iekštelpa";
        rename_map["Interior Lighting"] = L"Iekštelpas apgaismojums";

        rename_map["World"] = L"Pasaule";
    }
}

std::wstring Editor::PropertyRename (const std::string& name) {
    auto it = rename_map.find(name);
    
    if (it != rename_map.end()) {
        return it->second;
    } else {
        return std::wstring(name.begin(), name.end());
    }
}