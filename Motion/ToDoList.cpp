#include "ToDoList.h"
#include "MainFrame.h"
#include <wx/wx.h>

ToDoList::ToDoList(int X_POS, int Y_POS, int WIDTH, int HEIGHT, wxPanel* PARENT, wxString TITLE, MainFrame* frame) {
    parent = PARENT;
    x_pos = X_POS;
    y_pos = Y_POS;
    title = TITLE;
    width = WIDTH;
    height = HEIGHT;
    mainFrame = frame;

    CreateControls();
    BindEventHandlers();
    SetupSizers();
}

// Methods
void ToDoList::CreateControls()
{
    wxFont headlineFont(wxFontInfo(wxSize(0, 14)).Bold());
    wxFont mainFont(wxFontInfo(wxSize(0, 12)));

    panel = new wxPanel(parent, wxID_ANY, wxPoint(x_pos, x_pos), wxSize(width, height));
    panel->SetBackgroundColour(wxColor("#cfcfcf"));
    panel->SetFont(mainFont);

    headlineText = new wxStaticText(panel, wxID_ANY, title, wxPoint(-1, -1), wxSize(-1, -1), wxALIGN_CENTER_HORIZONTAL);
    headlineText->SetFont(headlineFont);

    inputField = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition /*wxPoint(100, 80)*/, wxSize(-1, 25), wxTE_PROCESS_ENTER);
    addButton = new wxButton(panel, wxID_ANY, "Add", wxDefaultPosition /*wxPoint(600, 80)*/, wxSize(-1, 25));
    checkListBox = new wxCheckListBox(panel, wxID_ANY, wxDefaultPosition /*wxPoint(100, 120)*/, wxSize(-1, 250), tasks, wxTE_MULTILINE | wxTE_NO_VSCROLL);
    //clearButton = new wxButton(panel, wxID_ANY, "Clear", wxDefaultPosition /*wxPoint(100, 525)*/, wxSize(-1, 25));
}

void ToDoList::SetupSizers() {
    mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->AddSpacer(10);
    mainSizer->Add(headlineText, wxSizerFlags().CenterHorizontal());
    mainSizer->AddSpacer(10);

    inputSizer = new wxBoxSizer(wxHORIZONTAL);
    inputSizer->AddSpacer(5);
    inputSizer->Add(inputField, wxSizerFlags().Proportion(1));
    inputSizer->AddSpacer(1);
    inputSizer->Add(addButton);
    inputSizer->AddSpacer(5);

    mainSizer->Add(inputSizer, wxSizerFlags().Expand());
    mainSizer->Add(checkListBox, wxSizerFlags().Proportion(1).Expand().Border(wxALL, 5));
    //mainSizer->Add(clearButton, 0, wxLEFT | wxDOWN, 5);

    //wxGridSizer* outerSizer = new wxGridSizer(1);
    //outerSizer->Add(mainSizer, wxSizerFlags().Border(wxALL, 25).Expand());

    //panel->SetSizer(outerSizer);
    //outerSizer->SetSizeHints(this);

    panel->SetSizerAndFit(mainSizer);
}

void ToDoList::BindEventHandlers()
{
    addButton->Bind(wxEVT_BUTTON, &ToDoList::OnAddButtonClicked, this);
    inputField->Bind(wxEVT_TEXT_ENTER, &ToDoList::OnInputEnter, this);
    //checkListBox->Bind(wxEVT_KEY_DOWN, &ToDoList::OnListKeyDown, this);
    //clearButton->Bind(wxEVT_BUTTON, &ToDoList::OnClearButtonClicked, this);
    //this->Bind(wxEVT_CLOSE_WINDOW, &ToDoList::OnWindowClosed, this);
}

void ToDoList::AddTaskFromInput()
{
    wxString description = inputField->GetValue();
    if (!description.IsEmpty()) {
        checkListBox->Insert(description, checkListBox->GetCount());
        inputField->Clear();
    }
    inputField->SetFocus();
}


// Event handlers
void ToDoList::OnAddButtonClicked(wxCommandEvent& evt)
{
    AddTaskFromInput();
}

void ToDoList::OnInputEnter(wxCommandEvent& evt)
{
    AddTaskFromInput();
}