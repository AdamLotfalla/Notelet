#include "Note.h"
#include <wx/wx.h>
#include "MainFrame.h"

Note::Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxString TEXT, wxFont FONT, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT) : wxPanel(PARENT, wxID_ANY, wxPoint(X_POS, Y_POS), wxSize(WIDTH, HEIGHT))
{
    width = WIDTH;
    height = HEIGHT;
    rotation = ROTATION;
    x_pos = X_POS;
    y_pos = Y_POS;
    text = TEXT;
    fcolor = FCOLOR;
    bcolor = BCOLOR;

    mHover = false;
    isActive = false;

    //this->SetBackgroundColour(bcolor);

    stickynote = new wxPanel(this, wxID_ANY);
    stickynote->SetBackgroundColour(bcolor);
    stickynote->SetForegroundColour(fcolor);

    textContent = new wxStaticText(stickynote, wxID_ANY, text, wxPoint(x_pos, y_pos) , wxSize(width, height) , BS_MULTILINE );
    textContent->SetFont(FONT);
    sizer = new wxGridSizer(1,1, wxSize(0,0));
    auto flag = wxSizerFlags().Center().Border(wxALL, 10);
    sizer->Add(textContent, flag);

    stickynote->Bind(wxEVT_ENTER_WINDOW, &Note::onMouseEnter, this);
    stickynote->Bind(wxEVT_LEAVE_WINDOW, &Note::onMouseLeave, this);
    stickynote->Bind(wxEVT_LEFT_DOWN, &Note::makeActive, this);

    stickynote->SetSizerAndFit(sizer);
    
}

void Note::onMouseEnter(wxMouseEvent& event) {
    mHover = true;
    wxLogStatus("Entered");
}

void Note::onMouseLeave(wxMouseEvent& event) {
    mHover = false;
    wxLogStatus("Exited");
}

void Note::makeActive(wxMouseEvent& event) {
    if (mHover) {
        //MainFrame::activePtr = this;
        //wxLogMessage("note active");
        //wxLogStatus("Activated");
        wxLogStatus(wxString::Format("Width: %d, Height: %d\n actual W: %d, actual H, %d", width, height, stickynote->GetSize().GetWidth(), stickynote->GetSize().GetHeight()));
        Refresh();
    }
    event.Skip();
}


