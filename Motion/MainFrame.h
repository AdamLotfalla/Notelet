#pragma once
#include <wx/wx.h>
#include "Note.h"
#include <vector>
#include <wx/splitter.h>
#include <wx/tglbtn.h>
#include <wx/wrapsizer.h>
#include "ImagePanel.h"
#include <deque>
#include "ToDoList.h"


class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	wxPanel* ColorPtr;
	Note* activeNote = nullptr;
	ToDoList* activeToDo = nullptr;
	bool occupied = false;
	std::vector<wxPanel*> paletes;
	wxColor NoteBColor = wxColor("#FFDB58");
	wxColor NoteFColor = wxColor("#2f2f2f");
	wxColor NoteColorBuffer;

	int noteDefaultPosX;
	int noteDefaultPosY;

	wxButton* addButton;
	wxTextCtrl* inputField;
	wxGridSizer* grid;
	wxBoxSizer* mainSizer;
	wxBoxSizer* inputSizer;
	wxStaticText* headlineText;
	wxCheckListBox* checkListBox;
	wxButton* clearButton;
	wxButton* deleteButton;
	wxButton* upButton;
	wxButton* downButton;
	wxArrayString tasks;
	wxBoxSizer* toDoButtonsSizer;

	wxRadioBox* F_BColorCheck;
	wxArrayString ColorChoices;
	wxPanel* fColorChoice;
	wxPanel* bColorChoice;
	wxFont font;
	wxPanel* notePanel;
	wxPanel* colorPanel;

	wxSplitterWindow* splitter;
	wxPanel* sidePanel;
	wxPanel* mainPanel;
	wxPanel* toDoPanel;
	wxSplitterWindow* sidePanelSplitter;
	wxSplitterWindow* toDoSplitter;
	wxWindow* listName;

	wxButton* button;
	wxButton* updateButton;
	wxToggleButton* boldButton;
	wxToggleButton* italicButton;
	wxToggleButton* underlineButton;

	wxStaticText* colorPanelTitle;
	wxStaticText* notePanelTitle;

	wxBoxSizer* sidePanelSizer;
	wxBoxSizer* colorPanelSizer;
	wxBoxSizer* notePanelSizer;
	wxBoxSizer* toDoPanelSizer;
	wxBoxSizer* stylingSizer;
	wxBoxSizer* colorSelectSizer;
	wxBoxSizer* colorChoiceSizer;
	wxBoxSizer* querySizer;

	wxWrapSizer* colorWraper;

	wxTextCtrl* queryEnterName;
	wxButton* queryAdd;

	wxDialog* listNameDialog;

	std::deque<Note* > notes;
	std::deque<ToDoList*> todolists;
	//std::vector<ImagePanel*> imagePanels;



	std::vector<std::string> colorPalette = {
		// Black and White
		"#000000",  // Black
		"#FFFFFF",  // White

		// Pastel Colors
		"#FFB3BA", // Soft Pink
		"#FFDFBA", // Soft Peach
		"#FFFFBA", // Soft Yellow
		"#BAFFC9", // Soft Mint Green
		"#BAE1FF", // Soft Light Blue
		"#E0BBE4", // Soft Lavender
		"#FFDAC1", // Light Peach
		"#BFFCC6", // Light Mint
		"#D5AAFF", // Soft Lilac

		// Desaturated Colors
		"#D85D51", // Muted Coral
		"#D46A36", // Muted Orange
		"#6D9A3B", // Muted Olive Green
		"#5C9E9F", // Soft Teal
		"#F0A500", // Muted Golden Yellow
		"#F28D35", // Muted Tomato
		"#607D8B", // Muted Slate Blue
		"#7A4B91", // Muted Purple

		// Selected Mustard Yellow and Dark Charcoal
		"#FFDB58", // Mustard Yellow
		"#2f2f2f"  // Dark Charcoal
	};

	bool isBold = false;
	bool isItalic = false;
	bool isUnderlined = false;

	void SetActive(Note* activenote);
	void SetActiveToDo(ToDoList* activetodo);

private:
	wxTextCtrl* noteEnterText;
	wxPanel* panel;
	wxString currentFilePath; // To store the current file path

	
	// Methods
	void declareVariables();
	void declareObjects(wxWindow* parent);
	void addSizers();
	void configureObjects();
	void SaveNotesToFile(const wxString& filePath);
	void LoadNotesFromFile(const wxString& filePath);
	void MakeToDoList();
	void AddTaskFromInput();
	void MakeNote();
	void MoveSelectedTask(int offset);
	void SwapTasks(int i, int j);


	// Events
	void OnNoteButtonClicked(wxCommandEvent& evt);
	void OnUpdateButtonClicked(wxCommandEvent& evt);
	void OnColorChoose(wxMouseEvent& evt);
	void OnColorHover(wxMouseEvent& evt);
	void OnBoldSelect(wxCommandEvent& evt);
	void OnItalicSelect(wxCommandEvent& evt);
	void OnUnderlinedSelect(wxCommandEvent& evt);
	void PaintColorBorder(wxPaintEvent& evt);
	void shortcuts(wxKeyEvent& evt);
	void OnDelete(wxKeyEvent& evt);
	void OnFileSave(wxCommandEvent& evt);
	void OnFileSaveAs(wxCommandEvent& evt);
	void OnFileOpen(wxCommandEvent& evt);
	void OnFileExit(wxCommandEvent& evt);
	void OnAddImage(wxCommandEvent& evt);
	void OnAddToDo(wxCommandEvent& evt);
	void AddListName(wxCommandEvent& evt);
	void OnAddButtonClicked(wxCommandEvent& evt);
	void OnInputEnter(wxCommandEvent& evt);
	void OnClearButton(wxCommandEvent& evt);
	void OnDeleteTask(wxCommandEvent& evt);
	void moveDown(wxCommandEvent& evt);
	void moveUp(wxCommandEvent& evt);
};