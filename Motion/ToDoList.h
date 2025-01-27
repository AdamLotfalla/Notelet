#pragma once
#include <wx/wx.h>
#include "Rectangle.h"

class MainFrame;

class ToDoList: public rectangle
{
public:
	wxButton* addButton;
	wxTextCtrl* inputField;
	wxGridSizer* grid;
	wxBoxSizer* mainSizer;
	wxBoxSizer* inputSizer;
	wxPanel* panel;
	wxStaticText* headlineText;
	wxCheckListBox* checkListBox;
	wxButton* deleteButton;
	wxButton* clearButton;
	wxButton* moveUpButton;
	wxButton* moveDownButton;
	wxBoxSizer* buttonsSizer;
	wxArrayString tasks;

	int x_pos;
	int y_pos;
	int width;
	int height;
	wxString title;
	wxPanel* parent;
	bool mHover;
	bool mHold;
	bool isActive;
	bool mDrag;
	bool disabled;
	wxPoint dragStartPos; // Mouse position when dragging starts
	wxPoint noteStartPos; // Initial position of the Note
	wxPoint offset;

	MainFrame* mainFrame = nullptr;

	ToDoList(int X_POS, int Y_POS, int WIDTH, int HEIGHT, wxPanel* PARENT, wxString TITLE, MainFrame* frame);
private:
	void OnAddTask(wxCommandEvent& evt);
	void OnAddButtonClicked(wxCommandEvent& evt);
	void OnInputEnter(wxCommandEvent& evt);
	void OnListKeyDown(wxKeyEvent& evt);
	void OnClearButtonClicked(wxCommandEvent& evt);
	void OnDeleteTask(wxCommandEvent& evt);
	void SetupSizers();
	void BindEventHandlers();
	void CreateControls();
	void AddTaskFromInput();
	void MoveSelectedTask(int offset);
	void SwapTasks(int i, int j);
	void moveDown(wxCommandEvent& evt);
	void moveUp(wxCommandEvent& evt);
	void showRenameDialog(wxMouseEvent& evt);
};

