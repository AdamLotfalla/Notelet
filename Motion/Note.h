#pragma once
#include <wx/wx.h>
#include <string>
#include <wx/object.h>
#include <wx/richtext/richtextctrl.h>
#include "Rectangle.h"

struct MainFrame;

class Note : public rectangle {
public:
    int width, height, rotation, x_pos, y_pos;
    wxString text;
    //wxColor fcolor, bcolor;
    wxGridSizer* sizer;
    wxRichTextCtrl* textContent;
    bool disabled;
    int borderWidth;
    wxPoint dragStartPos; // Mouse position when dragging starts
    wxPoint noteStartPos; // Initial position of the Note
    wxPoint offset;
    MainFrame* mainframe = nullptr;



    // Constructor declaration only
    Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxRichTextCtrl* SOURCE, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT, MainFrame* frame);

    void UpdateNote(wxRichTextCtrl* newSOURCE, wxColor newBcolor);
    void MakeActiveNote(wxMouseEvent& evt);
    void ShowEditPanel(wxMouseEvent& evt);
};
