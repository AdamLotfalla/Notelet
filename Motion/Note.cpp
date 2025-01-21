#include "Note.h"
#include <wx/wx.h>
#include "MainFrame.h"
#include <wx/dcbuffer.h>

Note::Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxRichTextCtrl* SOURCE, wxFont FONT, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT, MainFrame* frame)
    : rectangle(PARENT, wxPoint(X_POS, Y_POS), wxSize(WIDTH, HEIGHT), 2, FCOLOR, BCOLOR, frame)
{
    textContent = new wxRichTextCtrl(this, wxID_ANY, "", wxPoint(5, 5), wxSize(WIDTH - 10, HEIGHT - 10), wxTE_READONLY | wxBORDER_NONE);
    wxRichTextBuffer buffer;
    buffer.Copy(SOURCE->GetBuffer());
    textContent->GetBuffer().Copy(buffer);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(textContent, 1, wxEXPAND | wxALL, 5);

    textContent->SetBackgroundColour(BCOLOR);

    textContent->Disable();
    this->SetSizerAndFit(sizer);
    textContent->SetSize(wxSize(WIDTH, HEIGHT));
    this->SetSize(wxSize(WIDTH, HEIGHT));
    this->sizerKnob->SetPosition(wxPoint(this->GetSize().x - 15, this->GetSize().y - 15));
    this->SetDoubleBuffered(true);
    this->Layout();
    this->Update();
    this->Refresh();
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