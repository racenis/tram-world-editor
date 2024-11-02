#include <editor/language.h>
#include <editor/settings.h>

#include <unordered_map>

static std::unordered_map<std::string, std::wstring> l;

void Editor::ResetLanguage() {
    l.clear();
    
    switch (Editor::Settings::INTERFACE_LANGUAGE) {
        
        case Editor::Settings::LANGUAGE_EN:
            l["edit_menu_undo"] = L"Undo\tCtrl-Z";
            l["edit_menu_redo"] = L"Redo\tCtrl-Y";
            l["edit_menu_duplicate"] = L"Duplicate\tShift-D";
            l["edit_menu_signals"] = L"Signals\tShift-S";
            
            
            
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