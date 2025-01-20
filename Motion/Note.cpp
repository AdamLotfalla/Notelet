#include "Note.h"
#include <wx/wx.h>
#include "MainFrame.h"
#include <wx/dcbuffer.h>

Note::Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxString TEXT, wxFont FONT, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT, MainFrame* frame) 
    : wxPanel(PARENT, wxID_ANY, wxPoint(X_POS, Y_POS), wxSize(WIDTH, HEIGHT))
{
    width = WIDTH;
    height = HEIGHT;
    x_pos = X_POS;
    y_pos = Y_POS;
    text = TEXT;
    font = FONT;
    fcolor = FCOLOR;
    bcolor = BCOLOR;
    mHold = false;
    mainframe = frame;

    borderWidth = 3;
    mHover = false;

    this->SetBackgroundColour(bcolor);
    textContent = new wxStaticText(this, wxID_ANY, text, wxPoint(5,5), wxSize(width-10, height-10), wxST_NO_AUTORESIZE | GCP_JUSTIFY);
    //textContent = new wxRichTextCtrl(this, wxID_ANY, text, wxPoint(5,5), wxSize(width - 10, height - 10), wxST_NO_AUTORESIZE | GCP_JUSTIFY | wxTE_READONLY);
    textContent->SetFont(font);
    textContent->SetForegroundColour(fcolor);

    textContent->Bind(wxEVT_LEFT_DOWN, &Note::OnLeftMouseDown, this);
    textContent->Bind(wxEVT_MOTION, &Note::OnMouseMove, this);
    textContent->Bind(wxEVT_LEFT_UP, &Note::OnLeftMouseUp, this);    
    
    this->Bind(wxEVT_LEFT_DOWN, &Note::OnLeftMouseDown, this);
    this->Bind(wxEVT_LEFT_UP, &Note::OnLeftMouseUp, this);
    this->Bind(wxEVT_MOTION, &Note::OnMouseMove, this);
    this->Bind(wxEVT_PAINT, &Note::NotePaintBorder, this);
    this->Bind(wxEVT_KEY_DOWN, &Note::Shortcuts, this);

    this->SetDoubleBuffered(true);
    textContent->SetDoubleBuffered(true);

    Refresh();
}

void Note::UpdateNote(int newWidth, int newHeight, wxString newText, wxFont newFont, wxColor newFcolor, wxColor newBcolor)
{
    this->width = newWidth;
    this->height = newHeight;
    textContent->SetLabel(newText);
    textContent->SetFont(newFont);
    stickynote->SetBackgroundColour(newBcolor);
    textContent->SetForegroundColour(newFcolor);

    fcolor = newFcolor;
    bcolor = newBcolor;
    font = newFont;

    Refresh();
    wxLogStatus("Updated; supposed to be");
}

void Note::NotePaintBorder(wxPaintEvent& evt) {
    //wxPaintDC dc(this);
    wxBufferedPaintDC dc(this);
    dc.Clear();
    
    dc.SetPen(wxPen(wxColour("#7f7f7f"), borderWidth));
    
    wxRect rect = GetClientRect();
    dc.DrawLine(wxPoint(0, 0), wxPoint(0, height));
    dc.DrawLine(wxPoint(width, 0), wxPoint(width, height));
    dc.DrawLine(wxPoint(0, 0), wxPoint(width, 0));
    dc.DrawLine(wxPoint(0, height), wxPoint(width, height));
}

void Note::Shortcuts(wxKeyEvent& evt)
{
    if (evt.GetKeyCode() == WXK_DELETE && mainframe->activeNote == this) {
        mainframe->activeNote = nullptr;
        this->Destroy();
    }
    else if (evt.GetKeyCode() == WXK_PAGEDOWN) {
        this->Lower();
    }
    else if (evt.GetKeyCode() == WXK_PAGEUP) {
        this->Raise();
    }
}

void Note::OnMouseMove(wxMouseEvent& evt) {
    if (mHold && evt.LeftIsDown()  && evt.Dragging()) {
        textContent->SetCursor(wxCURSOR_CLOSED_HAND);
        auto mPos = wxGetMousePosition();
        mainframe->activeNote->SetPosition(mPos - offset);
    }
}

void Note::OnLeftMouseDown(wxMouseEvent& evt) {
    mHold = true;
    offset = wxGetMousePosition() - this->GetPosition();
    mainframe->SetActive(this);
    mainframe->activeRectangle = nullptr;
    
    this->CaptureMouse();
    this->SetFocus();
}

void Note::OnLeftMouseUp(wxMouseEvent& evt) {
    mHold = false;
    this->ReleaseMouse();
}
