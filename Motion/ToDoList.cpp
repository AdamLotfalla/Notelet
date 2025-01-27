#include "ToDoList.h"
#include "MainFrame.h"
#include "RenameDialog.h"
#include <wx/wx.h>

ToDoList::ToDoList(int X_POS, int Y_POS, int WIDTH, int HEIGHT, wxPanel* PARENT, wxString TITLE, MainFrame* frame) 
    : rectangle(PARENT, wxPoint(X_POS, Y_POS), wxSize(WIDTH, HEIGHT), 2, *wxBLACK, wxColor("#cfcfcf"), frame)  {
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

    this->sizerKnob->SetPosition(wxPoint(this->GetSize().x - 15, this->GetSize().y - 15));
    this->Refresh();
}

// Methods
void ToDoList::CreateControls()
{
    wxFont headlineFont(wxFontInfo(wxSize(0, 14)).Bold());
    wxFont mainFont(wxFontInfo(wxSize(0, 12)));

    headlineText = new wxStaticText(this, wxID_ANY, title, wxPoint(-1, -1), wxSize(-1, -1), wxALIGN_CENTER_HORIZONTAL);
    headlineText->SetFont(headlineFont);
    headlineText->SetBackgroundColour(this->bcolor);

    inputField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 25), wxTE_PROCESS_ENTER);
    addButton = new wxButton(this, wxID_ANY, "Add", wxDefaultPosition, wxSize(-1, 25));
    checkListBox = new wxCheckListBox(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 250), tasks, wxTE_MULTILINE | wxTE_NO_VSCROLL);
    deleteButton = new wxButton(this, wxID_ANY, "Delete", wxDefaultPosition, wxSize(-1, 25));
    clearButton = new wxButton(this, wxID_ANY, "Clear", wxDefaultPosition, wxSize(-1, 25));
    wxFont buttonFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial Unicode MS");
    moveUpButton = new wxButton(this, wxID_ANY, L"\u21e7", wxDefaultPosition, wxSize(25, 25));
    moveUpButton->SetFont(buttonFont);
    moveDownButton = new wxButton(this, wxID_ANY, L"\u21e9", wxDefaultPosition, wxSize(25, 25));
    moveDownButton->SetFont(buttonFont);
}

void ToDoList::OnClearButtonClicked(wxCommandEvent& evt)
{
    if (checkListBox->IsEmpty()) {
        return;
    }
    wxMessageDialog dialog(this, "Are you sure you want to clear your To-Do list?", "Clear", wxYES_NO);
    int result = dialog.ShowModal();
    if (result == wxID_YES) {
        checkListBox->Clear();
    }
}

void ToDoList::OnDeleteTask(wxCommandEvent& evt)
{
    int selection = checkListBox->GetSelection();
    if (selection == wxNOT_FOUND) {
        return;
    }
    checkListBox->Delete(selection);
}

void ToDoList::SetupSizers() {
    mainSizer = new wxBoxSizer(wxVERTICAL);
    buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
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
    mainSizer->Add(checkListBox, 1, wxEXPAND | wxRIGHT | wxLEFT, 5);
    mainSizer->Add(buttonsSizer, 0, wxEXPAND | wxALL, 5);


    buttonsSizer->Add(deleteButton, 1, wxEXPAND);
    buttonsSizer->Add(clearButton, 1, wxEXPAND);
    buttonsSizer->Add(moveUpButton, 0, wxEXPAND);
    buttonsSizer->Add(moveDownButton, 0, wxEXPAND);

    this->SetSizerAndFit(mainSizer);
}

void ToDoList::BindEventHandlers()
{
    addButton->Bind(wxEVT_BUTTON, &ToDoList::OnAddButtonClicked, this);
    inputField->Bind(wxEVT_TEXT_ENTER, &ToDoList::OnInputEnter, this);
    //checkListBox->Bind(wxEVT_KEY_DOWN, &ToDoList::OnListKeyDown, this);
    deleteButton->Bind(wxEVT_BUTTON, &ToDoList::OnDeleteTask, this);
    clearButton->Bind(wxEVT_BUTTON, &ToDoList::OnClearButtonClicked, this);
    moveUpButton->Bind(wxEVT_BUTTON, &ToDoList::moveUp, this);
    moveDownButton->Bind(wxEVT_BUTTON, &ToDoList::moveDown, this);
    headlineText->Bind(wxEVT_LEFT_DCLICK, &ToDoList::showRenameDialog, this);
    //this->Bind(wxEVT_CLOSE_WINDOW, &ToDoList::OnWindowClosed, this);
}

void ToDoList::AddTaskFromInput()
{
    wxString description = inputField->GetValue();
    tasks.push_back(description);
    if (!description.IsEmpty()) {
        checkListBox->Insert(description, checkListBox->GetCount());
        inputField->Clear();
    }
    inputField->SetFocus();
}

void ToDoList::MoveSelectedTask(int offset) {
    int selectedIndex = checkListBox->GetSelection();
    if (selectedIndex == wxNOT_FOUND) {
        return;
    }
    int newIndex = selectedIndex + offset;
    if (newIndex >= 0 && newIndex < checkListBox->GetCount()) {
        SwapTasks(selectedIndex, newIndex);
        checkListBox->SetSelection(newIndex, true);
    }
}

void ToDoList::SwapTasks(int i, int j) {
    auto task1 = checkListBox->GetString(i);
    auto task2 = checkListBox->GetString(j);
    bool task1state = checkListBox->IsChecked(i);
    bool task2state = checkListBox->IsChecked(j);
    checkListBox->SetString(j, task1);
    checkListBox->Check(j, task1state);
    checkListBox->SetString(i, task2);
    checkListBox->Check(i, task2state);
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

void ToDoList::moveDown(wxCommandEvent& evt)
{
    MoveSelectedTask(1);
}

void ToDoList::moveUp(wxCommandEvent& evt)
{
    MoveSelectedTask(-1);
}

void ToDoList::showRenameDialog(wxMouseEvent& evt)
{
    RenameDialog* dialog = new RenameDialog(parent, this);
    dialog->ShowModal();
}
