#include "Note.h"
#include <wx/wx.h>
#include "MainFrame.h"
#include <wx/dcbuffer.h>
#include "NoteDialog.h"

Note::Note(int WIDTH, int HEIGHT, int ROTATION, int X_POS, int Y_POS, wxRichTextCtrl* SOURCE, wxColor FCOLOR, wxColor BCOLOR, wxWindow* PARENT, MainFrame* frame)
    : rectangle(PARENT, wxPoint(X_POS, Y_POS), wxSize(WIDTH, HEIGHT), 2, FCOLOR, BCOLOR, frame)
{
    textContent = new wxRichTextCtrl(this, wxID_ANY, "", wxPoint(5, 5), wxSize(WIDTH - 10, HEIGHT - 10), wxTE_READONLY | wxBORDER_NONE);
    mainframe = frame;
    wxRichTextBuffer buffer;
    buffer.Copy(SOURCE->GetBuffer());
    textContent->GetBuffer().Copy(buffer);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(textContent, 1, wxEXPAND | wxALL, 5);

    textContent->SetBackgroundColour(BCOLOR);

    textContent->Disable();
    this->Bind(wxEVT_LEFT_DOWN, &Note::MakeActiveNote, this);
    this->Bind(wxEVT_LEFT_DCLICK, &Note::ShowEditPanel, this);
    this->SetSizerAndFit(sizer);
    this->sizerKnob->SetPosition(wxPoint(this->GetSize().x - 15, this->GetSize().y - 15));
    this->SetDoubleBuffered(true);
    this->Layout();
    this->Update();
    this->Refresh();
}

void Note::UpdateNote(wxRichTextCtrl* newSOURCE, wxColor newBcolor)
{
    textContent->Freeze();  // Prevent flickering

    textContent->GetBuffer().ResetAndClearCommands(); // Clear previous content properly

    wxRichTextBuffer buffer;
    buffer.Copy(newSOURCE->GetBuffer());  // Copy formatted content
    textContent->GetBuffer().Copy(buffer);

    textContent->LayoutContent(); // Force layout recalculation
    textContent->Refresh();
    textContent->Update();

    textContent->Thaw();  // Re-enable UI updates

    this->bcolor = newBcolor;
    textContent->SetBackgroundColour(newBcolor);

    this->Layout();  // Ensure sizer updates
    this->Refresh();
    this->Update();

    wxLogStatus("Updated!");
}


void Note::MakeActiveNote(wxMouseEvent& evt)
{
    mainframe->activeNote = this;

    evt.Skip();
}

void Note::ShowEditPanel(wxMouseEvent& evt)
{
    NoteDialog* dialog = new NoteDialog(mainframe->notSidePanel, "Edit Note", mainframe->isDark, mainframe);
    dialog->TextInput->SetFocus();

    
    
    dialog->TextInput->Freeze();
    dialog->TextInput->GetBuffer().ResetAndClearCommands();

    wxRichTextBuffer buffer;
    buffer.Copy(textContent->GetBuffer());
    dialog->TextInput->GetBuffer().Copy(buffer);

    // Invalidate all content to force measurement of trailing empty paragraphs
    dialog->TextInput->GetBuffer().Invalidate(wxRICHTEXT_ALL);

    // Force layout to recalc line breaks & paragraph heights
    dialog->TextInput->LayoutContent();

    // Place caret at start (or where you want it)
    dialog->TextInput->SetInsertionPoint(0);

    // Redraw everything
    dialog->TextInput->Refresh();
    dialog->TextInput->Update();

    dialog->TextInput->Thaw();



    dialog->TextInput->SetCaretPosition(0);
    dialog->TextInput->SelectAll();
    if (dialog->TextInput->HasSelection()){
        long selectionEndIndex = dialog->TextInput->GetSelectionRange().GetEnd() - 1;
        dialog->TextInput->SetCaretPosition(selectionEndIndex);
        dialog->TextInput->SelectNone();
        dialog->TextInput->AppendText(" temp");
        dialog->TextInput->SetSelection(selectionEndIndex, selectionEndIndex + 5);
        dialog->TextInput->DeleteSelection();
        dialog->TextInput->WriteText(" ");
        dialog->TextInput->Remove(selectionEndIndex, selectionEndIndex + 1); // remove space
    }
    else {
        dialog->TextInput->WriteText(" ");
        dialog->TextInput->Remove(0, 1);
    }
    



    dialog->ShowModal();
}
