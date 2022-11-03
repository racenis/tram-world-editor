#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <wx/glcanvas.h>
#include <wx/timer.h>

class ViewportCtrl : public wxGLCanvas {
public:
    ViewportCtrl(wxWindow* parent);
    virtual ~ViewportCtrl();
    ViewportCtrl(const ViewportCtrl& tc) = delete;
    ViewportCtrl(ViewportCtrl&& tc) = delete;
    ViewportCtrl& operator=(const ViewportCtrl& tc) = delete; 
    ViewportCtrl& operator=(ViewportCtrl&& tc) = delete; 
    
    enum ViewportMode : uint8_t {
        MODE_NONE,
        MODE_MOVE,
        MODE_ROTATE,
        MODE_TRANSLATE
    };
    
    enum TransformAxis : uint8_t {
        AXIS_NONE = 0,
        AXIS_X = 1,
        AXIS_Y = 2,
        AXIS_Z = 4
    };
    
    ViewportMode viewport_mode = MODE_NONE;
    uint8_t viewport_axis = AXIS_NONE;
private:
    void OnPaint(wxPaintEvent& event);
    void OnLeftClick(wxMouseEvent& event);
    void OnRightClick(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnSizeChange(wxSizeEvent& event);
    void OnKeydown(wxKeyEvent& event);
    void OnKeyup(wxKeyEvent& event);
    void OnTimer(wxTimerEvent& event);
    
    void CenterMouseCursor();
    
    float mouse_x = 0;
    float mouse_y = 0;
    bool key_forward = false;
    bool key_backward = false;
    bool key_left = false;
    bool key_right = false;
    wxTimer key_timer;
    
    wxGLContext* m_context;
};

#endif // VIEWPORT_H