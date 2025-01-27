#pragma once
#include <wx/wx.h>
#include "MainFrame.h"

struct ToDoList;


struct RenameDialog : public wxDialog
{
	RenameDialog(wxWindow* parent, ToDoList* todolist);

	wxBoxSizer* verticalSizer;
	wxBoxSizer* buttonSizer;
	wxTextCtrl* textInput;
	wxButton* RenameButton;
	wxButton* CancelButton;

	ToDoList* todolistptr = nullptr;

	void onRenameButton(wxCommandEvent& evt);
	void onCancelButton(wxCommandEvent& evt);
	void onKeyEnter(wxKeyEvent& evt);
};

