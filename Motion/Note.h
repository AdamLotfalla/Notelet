#pragma once
#include <wx/wx.h>
#include <string>

class MainFrame;

class Note : public wxPanel {
public:
    int width, height, rotation, x_pos, y_pos;
    wxString text;
    wxColor fcolor, bcolor;
    wxPanel* stickynote;
    wxGridSizer* sizer;
    wxStaticText* textContent;
    bool mHover;
    bool mHold;
    bool isActive;
    bool mDrag;
    bool disabled;
    int borderWidth;
    wxPoint dragStartPos; // Mouse position when dragging starts
    wxPoint noteStartPos; // Initial position of the Note
    wxPoint offset;
    wxFont font;
    //wxWindow* active;
    MainFrame* mainFrame = nullptr;


    // Constructor declaration only
    Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxString TEXT, wxFont FONT, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT, MainFrame* frame);

    void UpdateNote(int newWidth, int newHeight, wxString newText, wxFont newFont, wxColor newFcolor, wxColor newBcolor);

    void makeActive(wxMouseEvent& evt);
    void onMouseEnter(wxMouseEvent& evt);
    void onMouseLeave(wxMouseEvent& evt);
    void NotePaintBorder(wxPaintEvent& evt);
    void OnLeftMouseDown(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    void OnLeftMouseUp(wxMouseEvent& evt);
};
