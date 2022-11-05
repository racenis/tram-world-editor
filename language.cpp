#include <language.h>
#include <unordered_map>

Editor::Language* Editor::Languages[2] = {&Language_LV, &Language_EN};
Editor::Language* Editor::selected_language = &Editor::Language_LV;
std::unordered_map<std::string, std::wstring> rename_map;

void ResetRename() {
    rename_map.clear();
    
    
}

std::wstring Editor::Rename (const std::string& name) {
    return L"FUCK";
    
}