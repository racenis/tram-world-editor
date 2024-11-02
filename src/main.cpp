#include <wx/wx.h>

#include <editor/editor.h>
#include <editor/language.h>
#include <editor/settings.h>

#include <widgets/mainframe.h>
#include <widgets/dialogs.h>

#include <platform/api.h>

class TramEditor : public wxApp {
public:
    bool OnInit() {
        Editor::Settings::Load();
        //Editor::selected_language = Editor::Languages[Editor::Settings::INTERFACE_LANGUAGE];
        Editor::ResetLanguage();
        //Editor::Init();
        Editor::Reset();
        
        main_frame = new MainFrameCtrl();
        main_frame->Show(true);
        
        Editor::Init();
        
        LoadCells();
        
        
        return true;
    }
};

double tram::Platform::Window::GetTime() {
    return 0;
}

wxIMPLEMENT_APP(TramEditor);


