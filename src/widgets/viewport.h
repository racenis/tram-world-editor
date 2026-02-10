#ifndef WIDGETS_VIEWPORT_H
#define WIDGETS_VIEWPORT_H

#include <wx/glcanvas.h>
#include <wx/timer.h>

class ViewportTool;

class ViewportCtrl : public wxGLCanvas {
public:
    ViewportCtrl(wxWindow* parent);
    virtual ~ViewportCtrl();
    ViewportCtrl(const ViewportCtrl& tc) = delete;
    ViewportCtrl(ViewportCtrl&& tc) = delete;
    ViewportCtrl& operator=(const ViewportCtrl& tc) = delete; 
    ViewportCtrl& operator=(ViewportCtrl&& tc) = delete; 
    
    void ViewCenterOnSelection();
    void CenterMouseCursor();
    void StartTimer();
    void StopTimer();
    
    ViewportTool* viewport_tool = nullptr;
private:
    void OnPaint(wxPaintEvent& event);
    void OnLeftClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnSizeChange(wxSizeEvent& event);
    void OnKeydown(wxKeyEvent& event);
    void OnKeyup(wxKeyEvent& event);
    void OnTimer(wxTimerEvent& event);
    
    void CancelViewportOperation();
    
    wxTimer key_timer;
    
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    wxGLContext* m_context;
};

extern ViewportCtrl* viewport;

#endif // WIDGETS_VIEWPORT_H