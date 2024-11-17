#pragma once
#include <wx/wx.h>
#include <string>
#include "MainFrame.h"


class Note : public wxPanel {
public:
    int width, height, rotation, x_pos, y_pos;
    wxString text;
    wxColor fcolor, bcolor;
    wxPanel* stickynote;
    wxGridSizer* sizer;
    wxStaticText* textContent;
    bool mHover;
    bool isActive;


    // Constructor declaration only
    Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxString TEXT, wxFont FONT, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT);

    void makeActive(wxMouseEvent& evt);
    void onMouseEnter(wxMouseEvent& evt);
    void onMouseLeave(wxMouseEvent& evt);
};
