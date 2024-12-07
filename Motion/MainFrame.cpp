#include "MainFrame.h"
#include "Note.h"
#include <wx/wx.h>
#include <string>
#include <vector>
#include <cmath>
#include <wx/wrapsizer.h>
#include <wx/splitter.h>
#include <wx/tglbtn.h>
#include <wx/menu.h>
#include <wx/filedlg.h>
#include <wx/xml/xml.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <deque>
#include <wx/icon.h>
#include "ToDoList.h"
#include <string>


MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	declareVariables();
	declareObjects(this);
	addSizers();
	configureObjects();

	// Event Binding
	button->Bind(wxEVT_BUTTON, &MainFrame::OnNoteButtonClicked, this);
	boldButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnBoldSelect, this);
	italicButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnItalicSelect, this);
	underlineButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnUnderlinedSelect, this);
	updateButton->Bind(wxEVT_BUTTON, &MainFrame::OnUpdateButtonClicked, this);
	clearButton->Bind(wxEVT_BUTTON, &MainFrame::OnClearButton, this);
	deleteButton->Bind(wxEVT_BUTTON, &MainFrame::OnDeleteTask, this);
	upButton->Bind(wxEVT_BUTTON, &MainFrame::moveUp, this);
	downButton->Bind(wxEVT_BUTTON, &MainFrame::moveDown, this);

	this->Bind(wxEVT_CHAR_HOOK, &MainFrame::shortcuts, this);

	Bind(wxEVT_MENU, &MainFrame::OnFileSave, this, wxID_SAVE);
	Bind(wxEVT_MENU, &MainFrame::OnFileSaveAs, this, wxID_SAVEAS);
	Bind(wxEVT_MENU, &MainFrame::OnFileOpen, this, wxID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::OnFileExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainFrame::OnNoteButtonClicked, this, wxID_NEW);
	Bind(wxEVT_MENU, &MainFrame::OnAddImage, this, wxID_ADD);
	//Bind(wxEVT_MENU, &MainFrame::OnAddToDo, this, 5);

	//panel->Bind(wxEVT_CHAR_HOOK, &MainFrame::OnDelete, this);

	this->SetDoubleBuffered(true);
/*	wxIcon appIcon("./Icon.ico", wxBITMAP_TYPE_ICO);
	SetIcon(appIcon)*/;
}

void MainFrame::declareVariables() {
	ColorChoices.Add("Foreground color");
	ColorChoices.Add("Background color");

	noteDefaultPosX = 200;
	noteDefaultPosY = 250;

}
void MainFrame::declareObjects(wxWindow* parent) {
	// --- Splitter windows ---
	splitter = new wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
	panel = new wxPanel(splitter);
	sidePanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);

	sidePanelSplitter = new wxSplitterWindow(sidePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	toDoPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	panel->SetSizerAndFit(toDoPanelSizer);
	toDoSplitter = new wxSplitterWindow(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
	toDoPanelSizer->Add(toDoSplitter, 1, wxEXPAND);
	mainPanel = new wxPanel(toDoSplitter);
	toDoPanel = new wxPanel(toDoSplitter);

	toDoSplitter->SplitVertically(mainPanel, toDoPanel, FromDIP(-150));
	toDoSplitter->SetMinimumPaneSize(200);


	wxFont headlineFont(wxFontInfo(wxSize(0, 14)).Bold());
	wxFont mainFont(wxFontInfo(wxSize(0, 12)));

	toDoPanel->SetFont(mainFont);

	headlineText = new wxStaticText(toDoPanel, wxID_ANY, "To-Do list", wxPoint(-1, -1), wxSize(-1, -1), wxALIGN_CENTER_HORIZONTAL);
	headlineText->SetFont(headlineFont);

	inputField = new wxTextCtrl(toDoPanel, wxID_ANY, "", wxDefaultPosition /*wxPoint(100, 80)*/, wxSize(-1, 25), wxTE_PROCESS_ENTER);
	addButton = new wxButton(toDoPanel, wxID_ANY, "Add", wxDefaultPosition /*wxPoint(600, 80)*/, wxSize(-1, 25));
	checkListBox = new wxCheckListBox(toDoPanel, wxID_ANY, wxDefaultPosition /*wxPoint(100, 120)*/, wxSize(-1, -1), tasks, wxTE_MULTILINE | wxTE_NO_VSCROLL);
	clearButton = new wxButton(toDoPanel, wxID_ANY, "Clear", wxDefaultPosition /*wxPoint(100, 525)*/, wxSize(-1, 25));
	deleteButton = new wxButton(toDoPanel, wxID_ANY, "Delete task", wxDefaultPosition /*wxPoint(100, 525)*/, wxSize(-1, 25));
	wxFont buttonFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial Unicode MS");
	upButton = new wxButton(toDoPanel, wxID_ANY, L"\u21e7", wxDefaultPosition, wxSize(25, 25));
	upButton->SetFont(buttonFont);
	downButton = new wxButton(toDoPanel, wxID_ANY, L"\u21e9", wxDefaultPosition, wxSize(25, 25));
	downButton->SetFont(buttonFont);
	toDoButtonsSizer = new wxBoxSizer(wxHORIZONTAL);


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
	toDoButtonsSizer->AddSpacer(5);
	toDoButtonsSizer->Add(clearButton, 1, wxEXPAND);
	toDoButtonsSizer->AddSpacer(1);
	toDoButtonsSizer->Add(deleteButton, 1, wxEXPAND);
	toDoButtonsSizer->AddSpacer(1);
	toDoButtonsSizer->Add(upButton, 0, wxEXPAND);
	toDoButtonsSizer->AddSpacer(1);
	toDoButtonsSizer->Add(downButton, 0, wxEXPAND);
	toDoButtonsSizer->AddSpacer(5);
	mainSizer->Add(toDoButtonsSizer, 0, wxEXPAND);
	mainSizer->AddSpacer(5);

	toDoPanel->SetSizerAndFit(mainSizer);

	addButton->Bind(wxEVT_BUTTON, &MainFrame::OnAddButtonClicked, this);
	inputField->Bind(wxEVT_TEXT_ENTER, &MainFrame::OnInputEnter, this);

	// --- Panels ---
	colorPanel = new wxPanel(sidePanelSplitter);
	notePanel = new wxPanel(sidePanelSplitter);

	// --- Text Control ---
	noteEnterText = new wxTextCtrl(notePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_NO_VSCROLL);

	// --- Buttons and Toggles ---
	button = new wxButton(notePanel, wxID_ANY, "Add note", wxDefaultPosition, wxSize(-1, FromDIP(25)));
	updateButton = new wxButton(notePanel, wxID_ANY, "Update", wxDefaultPosition, wxSize(-1, FromDIP(25)));

	// Bold toggle button
	boldButton = new wxToggleButton(notePanel, wxID_ANY, "B", wxDefaultPosition, wxSize(FromDIP(25), FromDIP(25)));
	font = boldButton->GetFont();
	font.MakeBold();
	boldButton->SetFont(font);

	// Italic toggle button
	italicButton = new wxToggleButton(notePanel, wxID_ANY, "I", wxDefaultPosition, wxSize(FromDIP(25), FromDIP(25)));
	font = italicButton->GetFont();
	font.MakeItalic();
	italicButton->SetFont(font);

	// Underline toggle button
	underlineButton = new wxToggleButton(notePanel, wxID_ANY, "U", wxDefaultPosition, wxSize(FromDIP(25), FromDIP(25)));
	font = underlineButton->GetFont();
	font.MakeUnderlined();
	underlineButton->SetFont(font);

	// Reset font properties
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	font.SetStyle(wxFONTSTYLE_NORMAL);
	font.SetUnderlined(false);
	font.MakeBold();
	font.SetPointSize(9.5);

	// --- Color-related elements ---
	F_BColorCheck = new wxRadioBox(colorPanel, wxID_ANY, "Choose what color to change: ", wxDefaultPosition, wxDefaultSize, ColorChoices, 1);
	fColorChoice = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(25), FromDIP(25)));
	bColorChoice = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(25), FromDIP(25)));
	colorPanelTitle = new wxStaticText(colorPanel, wxID_ANY, "Color palette");

	// --- Titles ---
	notePanelTitle = new wxStaticText(notePanel, wxID_ANY, "Note options");

	// Menu Bar
	wxMenuBar* menuBar = new wxMenuBar();
	wxMenu* fileMenu = new wxMenu();
	wxMenu* addMenu = new wxMenu();

	fileMenu->Append(wxID_SAVE, "&Save\tCtrl-S", "Save the notes");
	fileMenu->Append(wxID_SAVEAS, "Save &As...", "Save the notes to a specific file");
	fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O", "Open a file to load notes");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "E&xit", "Exit the application");

	addMenu->Append(wxID_NEW, "&Add Note\tCtrl-N", "Add a new note");
	addMenu->Append(5, "&Add To-Do List", "Add a new To-Do list");
	//addMenu->Append(wxID_ADD, "&Add Image\tCtrl-I", "Add an image to the panel");

	menuBar->Append(fileMenu, "&File");
	menuBar->Append(addMenu, "&Add");

	SetMenuBar(menuBar);


	// --- Status Bar ---
	CreateStatusBar();
}
void MainFrame::addSizers() {
	sidePanelSizer = new wxBoxSizer(wxVERTICAL);
	colorWraper = new wxWrapSizer(wxHORIZONTAL);
	colorPanelSizer = new wxBoxSizer(wxVERTICAL);
	notePanelSizer = new wxBoxSizer(wxVERTICAL);
	stylingSizer = new wxBoxSizer(wxHORIZONTAL);
	colorSelectSizer = new wxBoxSizer(wxHORIZONTAL);
	colorChoiceSizer = new wxBoxSizer(wxVERTICAL);

	for (int i = 0; i < colorPalette.size(); i++) {
		auto color = new wxPanel(colorPanel, wxID_ANY, wxPoint(-1, -1), wxSize(FromDIP(25), FromDIP(25)));
		color->SetBackgroundColour(wxColor(colorPalette[i]));
		color->Bind(wxEVT_PAINT, &MainFrame::PaintColorBorder, this);
		paletes.push_back(color);
	}

	for (auto color : paletes) {
		color->Bind(wxEVT_LEFT_DOWN, &MainFrame::OnColorChoose, this);
		color->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnColorHover, this);
		colorWraper->Add(color, 0, wxEXPAND);
	}

	sidePanelSizer->Add(sidePanelSplitter, 1, wxEXPAND);
	stylingSizer->Add(button, 1);
	stylingSizer->Add(boldButton, 0);
	stylingSizer->Add(italicButton, 0);
	stylingSizer->Add(underlineButton, 0);

	colorPanelSizer->AddSpacer(3);
	colorPanelSizer->Add(colorPanelTitle, 0, wxALL, 4);
	colorPanelSizer->AddSpacer(3);
	colorPanelSizer->Add(colorSelectSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	colorPanelSizer->Add(colorWraper, 0, wxEXPAND | wxALL, 5);

	colorSelectSizer->Add(F_BColorCheck, 0);
	colorSelectSizer->Add(colorChoiceSizer, 1, wxEXPAND);

	colorChoiceSizer->AddSpacer(20);
	colorChoiceSizer->Add(fColorChoice, 0, wxALIGN_RIGHT);
	colorChoiceSizer->Add(bColorChoice, 0, wxALIGN_RIGHT);
	colorChoiceSizer->AddSpacer(5);

	notePanelSizer->AddSpacer(6);
	notePanelSizer->Add(notePanelTitle, 0, wxALL, 4);
	notePanelSizer->AddSpacer(4);
	notePanelSizer->Add(stylingSizer, 0, wxEXPAND | wxRIGHT | wxLEFT | wxUP, 2);
	notePanelSizer->Add(updateButton, 0, wxEXPAND | wxRIGHT | wxLEFT | wxDOWN, 2);
	notePanelSizer->Add(noteEnterText, 1, wxEXPAND | wxALL, 2);
}
void MainFrame::configureObjects() {
	panel->SetBackgroundColour(wxColor("#ffffff"));
	sidePanel->SetBackgroundColour(wxColor("#f7f7f5"));
	sidePanel->SetSizerAndFit(sidePanelSizer);
	splitter->SplitVertically(sidePanel, panel, FromDIP(100));
	splitter->SetMinimumPaneSize(FromDIP(220));
	colorPanel->SetSizerAndFit(colorPanelSizer);
	notePanel->SetSizerAndFit(notePanelSizer);
	sidePanelSplitter->SplitHorizontally(colorPanel, notePanel, FromDIP(150));
	sidePanelSplitter->SetMinimumPaneSize(200);
	noteEnterText->SetForegroundColour(wxColor("9f9e9b"));
	colorPanelTitle->SetFont(font);
	F_BColorCheck->SetForegroundColour(wxColor("#000000"));
	F_BColorCheck->SetSelection(1);
	fColorChoice->SetBackgroundColour(NoteFColor);
	bColorChoice->SetBackgroundColour(NoteBColor);
	notePanelTitle->SetFont(font);
}

void MainFrame::SaveNotesToFile(const wxString& filePath) {
	wxXmlDocument xmlDoc;
	wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Notes");
	xmlDoc.SetRoot(root);

	for (const auto& note : notes) {
		wxXmlNode* noteNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Note");
		noteNode->AddAttribute("x", wxString::Format("%d", note->GetPosition().x));
		noteNode->AddAttribute("y", wxString::Format("%d", note->GetPosition().y));
		noteNode->AddAttribute("foreground", note->fcolor.GetAsString());
		noteNode->AddAttribute("background", note->bcolor.GetAsString());
		noteNode->AddAttribute("fontSize", wxString::Format("%d", note->font.GetPointSize()));
		noteNode->AddAttribute("bold", note->font.GetWeight() == wxFONTWEIGHT_BOLD ? "1" : "0");
		noteNode->AddAttribute("italic", note->font.GetStyle() == wxFONTSTYLE_ITALIC ? "1" : "0");
		noteNode->AddAttribute("underline", note->font.GetUnderlined() ? "1" : "0");

		wxXmlNode* textNode = new wxXmlNode(wxXML_TEXT_NODE, "", note->text);
		noteNode->AddChild(textNode);
		root->AddChild(noteNode);
	}

	for (int i = 0; i < checkListBox->GetCount(); i++) {
		wxXmlNode* taskNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Task");
		taskNode->AddAttribute("Description", checkListBox->GetString(i));
		taskNode->AddAttribute("State", wxString::Format("%i", checkListBox->IsChecked(i)));

		root->AddChild(taskNode);
	}

	if (!xmlDoc.Save(filePath)) {
		wxLogError("Unable to save the file!");
	}
	else {
		wxLogStatus("File saved successfully!");
	}
}
void MainFrame::LoadNotesFromFile(const wxString& filePath) {
	wxXmlDocument xmlDoc;
	if (!xmlDoc.Load(filePath)) {
		wxLogError("Unable to load the file!");
		return;
	}

	wxXmlNode* root = xmlDoc.GetRoot();
	if (root->GetName() != "Notes") {
		wxLogError("Invalid file format!");
		return;
	}

	// Clear existing notes
	for (auto note : notes) {
		note->Destroy();
	}
	notes.clear();

	wxXmlNode* child = root->GetChildren();
	while (child) {
		if (child->GetName() == "Note") {
			int x = wxAtoi(child->GetAttribute("x", "0"));
			int y = wxAtoi(child->GetAttribute("y", "0"));
			wxColour foreground(child->GetAttribute("foreground", "black"));
			wxColour background(child->GetAttribute("background", "white"));

			wxFont font;
			font.SetPointSize(wxAtoi(child->GetAttribute("fontSize", "12")));
			font.SetWeight(child->GetAttribute("bold", "0") == "1" ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
			font.SetStyle(child->GetAttribute("italic", "0") == "1" ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
			font.SetUnderlined(child->GetAttribute("underline", "0") == "1");

			wxString text = child->GetNodeContent();

			Note* note = new Note(FromDIP(150), FromDIP(150), 0, x, y, text, font, foreground, background, mainPanel, this);
			notes.push_back(note);
		}
		else if (child->GetName() == "Task") {
			wxString description = child->GetAttribute("Description", "error loading task");
			int state = wxAtoi(child->GetAttribute("State", "0"));

			checkListBox->Insert(description, checkListBox->GetCount());
			checkListBox->Check(checkListBox->GetCount()-1, state);
		}
		child = child->GetNext();
	}

	wxLogStatus("File loaded successfully!");
}
void MainFrame::SetActive(Note* activenote) {
	if (activenote == nullptr) {
		wxLogStatus("Attempting to set active note to nullptr.");
		return;
	}
	activeNote = activenote;
	noteEnterText->SetLabel(activeNote->text);
	noteEnterText->SelectAll();
	wxLogStatus("Active ID: %i", activeNote->GetId());
}
//void MainFrame::SetActiveToDo(ToDoList* activetodo) {
//	if (activetodo == nullptr) {
//		wxLogStatus("Attempting to set active note to nullptr.");
//		return;
//	}
//	activeToDo = activetodo;
//	wxLogStatus("Active ID: %i", activeToDo->GetId());
//}
//void MainFrame::MakeToDoList() {
//	listNameDialog = new wxDialog(this, wxID_ANY, "Add New List", wxDefaultPosition, wxSize(300, 65));
//	listNameDialog->Center();
//	querySizer = new wxBoxSizer(wxHORIZONTAL);
//
//	queryEnterName = new wxTextCtrl(listNameDialog, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 25));
//	queryAdd = new wxButton(listNameDialog, wxID_ANY, "Add list", wxDefaultPosition, wxSize(70, 25));
//
//	querySizer->Add(queryEnterName, 1, wxALL | wxEXPAND, 1);
//	querySizer->Add(queryAdd, 0, wxALL, 1);
//
//	listNameDialog->SetSizer(querySizer);      // Set the sizer
//
//	queryAdd->Bind(wxEVT_BUTTON, &MainFrame::AddListName, this);
//
//	listNameDialog->ShowModal(); // Display as modal popup
//	delete listNameDialog; // Destroy the dialog after use
//}
void MainFrame::MakeNote()
{
	wxLogStatus("control pressed");
	auto message = noteEnterText->GetValue();
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	font.SetStyle(wxFONTSTYLE_NORMAL);
	font.SetUnderlined(false);
	if (isBold) { font.MakeBold(); }
	if (isItalic) { font.MakeItalic(); }
	if (isUnderlined) { font.MakeUnderlined(); }

	Note* note = new Note(FromDIP(150), FromDIP(150), 0, noteDefaultPosX, noteDefaultPosY, message, font, NoteFColor, NoteBColor, mainPanel, this);
	notes.push_back(note);
	wxLogStatus(wxString::Format("note created at (%d,%d)", note->GetPosition().x, note->GetPosition().y));
	noteEnterText->SelectAll();
	activeNote = note;
	if (noteDefaultPosX + note->GetSize().x + FromDIP(50) < this->GetSize().GetWidth() && noteDefaultPosY + note->GetSize().y + FromDIP(50) < this->GetSize().GetHeight()) {
		noteDefaultPosX += FromDIP(25);
		noteDefaultPosY += FromDIP(25);
	}
	else {
		noteDefaultPosX = 0;
		noteDefaultPosY = 0;
	}

}
void MainFrame::MoveSelectedTask(int offset){
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

void MainFrame::SwapTasks(int i, int j) {

	auto task1 = checkListBox->GetString(i);
	auto task2 = checkListBox->GetString(j);
	bool task1state = checkListBox->IsChecked(i);
	bool task2state = checkListBox->IsChecked(j);

	checkListBox->SetString(j, task1);
	checkListBox->Check(j, task1state);

	checkListBox->SetString(i, task2);
	checkListBox->Check(i, task2state);

}


// Events
void MainFrame::OnNoteButtonClicked(wxCommandEvent& evt) {
	MakeNote();
	evt.Skip();
}
void MainFrame::OnUpdateButtonClicked(wxCommandEvent& evt)
{
	if (activeNote != nullptr) {
		auto message = noteEnterText->GetValue();
		font.SetWeight(wxFONTWEIGHT_NORMAL);
		font.SetStyle(wxFONTSTYLE_NORMAL);
		font.SetUnderlined(false);
		if (isBold) { font.MakeBold(); }
		if (isItalic) { font.MakeItalic(); }
		if (isUnderlined) { font.MakeUnderlined(); }

		activeNote->UpdateNote(FromDIP(150), FromDIP(150), message, font, NoteFColor, NoteBColor);
	}
}
void MainFrame::OnColorChoose(wxMouseEvent& evt) {
	if (F_BColorCheck->GetSelection() == 1) { NoteBColor = NoteColorBuffer; wxLogStatus("BColor is chosen"); bColorChoice->SetBackgroundColour(NoteColorBuffer); }
	else if (F_BColorCheck->GetSelection() == 0) { NoteFColor = NoteColorBuffer; wxLogStatus("FColor is chosen"); fColorChoice->SetBackgroundColour(NoteColorBuffer); }
	else { wxLogStatus("WTF!?"); }

	//wxLogStatus("Color selected: %s", NoteBColor.GetAsString());
	Refresh();
	evt.Skip();
}
void MainFrame::OnColorHover(wxMouseEvent& evt) {
	wxWindow* panel = dynamic_cast<wxWindow*>(evt.GetEventObject());
	if (panel) {
		panel->SetCursor(wxCURSOR_HAND);
		NoteColorBuffer = panel->GetBackgroundColour();  // Set NoteColor to the selected panel's color
	}
	evt.Skip();
}
void MainFrame::OnBoldSelect(wxCommandEvent& evt)
{
	isBold = !isBold;
	wxLogStatus(wxString::Format("set bold state to %d", isBold));
}
void MainFrame::OnItalicSelect(wxCommandEvent& evt)
{
	isItalic = !isItalic;
	wxLogStatus(wxString::Format("set italic state to %d", isItalic));
}
void MainFrame::OnUnderlinedSelect(wxCommandEvent& evt)
{
	isUnderlined = !isUnderlined;
	wxLogStatus(wxString::Format("set underline state to %d", isUnderlined));
}
void MainFrame::PaintColorBorder(wxPaintEvent& evt) {

	wxLogStatus("Paint event triggered");

	//wxPaintDC dc(this);

	//// Set the pen for the outline (color and thickness)
	//dc.SetPen(wxPen(*wxWHITE, 2)); // Black color, 2-pixel width

	//// Draw the rectangle outline
	//dc.DrawRectangle(0, 0, FromDIP(25), FromDIP(25));
}
void MainFrame::shortcuts(wxKeyEvent& evt)
{
	wxLogStatus("Key pressed");
	if (evt.ControlDown() && evt.GetKeyCode() == 'N') {
		MakeNote();
		evt.Skip(false);
		return;
	}
	else if (evt.ControlDown() && evt.GetKeyCode() == 'U') {
		if (activeNote != nullptr) {
			auto message = noteEnterText->GetValue();
			font.SetWeight(wxFONTWEIGHT_NORMAL);
			font.SetStyle(wxFONTSTYLE_NORMAL);
			font.SetUnderlined(false);
			if (isBold) { font.MakeBold(); }
			if (isItalic) { font.MakeItalic(); }
			if (isUnderlined) { font.MakeUnderlined(); }

			activeNote->UpdateNote(FromDIP(150), FromDIP(150), message, font, NoteFColor, NoteBColor);
		}
	}
	//else if (evt.ControlDown() && evt.GetKeyCode() == 'T') {
	//	MakeToDoList();
	//}
	else if (evt.GetKeyCode() == WXK_DELETE) {
		if(notes.size() > 0){
			auto it = std::find(notes.begin(), notes.end(), activeNote);
			notes.erase(it);
			activeNote->Destroy();
			activeNote = nullptr;
		}
	}
	else if (evt.GetKeyCode() == WXK_HOME) {
		activeNote->Raise();
	}
	else if (evt.GetKeyCode() == WXK_END) {
		activeNote->Lower();
	}

	evt.Skip();
}
void MainFrame::OnDelete(wxKeyEvent& evt)
{
	if (activeNote != nullptr) {
		activeNote->Destroy();
		activeNote = nullptr;
	}
	else {
		wxLogStatus("No active note to delete.");
	}
}
void MainFrame::OnFileSave(wxCommandEvent& evt) {
	if (currentFilePath.IsEmpty()) {
		OnFileSaveAs(evt);
	}
	else {
		SaveNotesToFile(currentFilePath);
	}
}
void MainFrame::OnFileSaveAs(wxCommandEvent& evt) {
	wxFileDialog saveFileDialog(this, "Save Notes", "", "",
		"XML files (*.xml)|*.xml",
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;

	currentFilePath = saveFileDialog.GetPath();

	// Ensure file path has .xml extension
	if (!currentFilePath.Lower().EndsWith(".xml")) {
		currentFilePath += ".xml";
	}

	SaveNotesToFile(currentFilePath);
}
void MainFrame::OnFileOpen(wxCommandEvent& evt) {
	wxFileDialog openFileDialog(
		this, _("Open Notes File"), "", "",
		"XML files (*.xml)|*.xml|All Files (*.*)|*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		wxLogStatus("File open canceled.");
		return; // User canceled, so exit the function.
	}

	wxString filePath = openFileDialog.GetPath();
	wxLogStatus("Opening file: %s", filePath);

	LoadNotesFromFile(filePath);
}
void MainFrame::OnFileExit(wxCommandEvent& evt) {
	Close(true);
}
void MainFrame::OnAddImage(wxCommandEvent& event) {


	wxImage image;
	image.LoadFile("cat.png", wxBITMAP_TYPE_PNG);
	//if (!image.LoadFile("C:/Users/EGYTEK/OneDrive/Desktop/cat.png")) {
	//	// Handle error, e.g., log an error message
	//	wxLogError("Failed to load image file.");
	//}

	wxBitmap bitmap = wxBitmap(image);
	wxStaticBitmap* staticBitmap = new wxStaticBitmap(panel, wxID_ANY, bitmap);


	//// Create a file dialog for image selection
	//wxFileDialog openFileDialog(this, _("Open Image file"), "", "",
	//	"Image files (*.png;*.jpg)|*.png;*.jpg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	//// If the user selects a file and presses OK
	//if (openFileDialog.ShowModal() == wxID_OK) {
	//	wxString fileName = openFileDialog.GetPath();

	//	// Try loading the image
	//	wxImage image(fileName, wxBITMAP_TYPE_ANY); // Alternatively, use wxBITMAP_TYPE_PNG or 
	//	if (image.IsOk()) {
	//		// Image loaded successfully, you can display or process it

	//		// For example, display the image in a static bitmap control (assuming you have a wxStaticBitmap)
	//		// myStaticBitmapControl->SetBitmap(bitmap);

	//		wxBitmap bitmap(image);
	//		auto imagepanel = new ImagePanel(panel);
	//	}
	//	else {
	//		wxLogError("Failed to load the image: %s", fileName);
	//	}
	//}
}
//void MainFrame::OnAddToDo(wxCommandEvent& evt)
//{
//	MakeToDoList();
//}
void MainFrame::AddListName(wxCommandEvent& evt)
{
	auto todo = new ToDoList(noteDefaultPosX, noteDefaultPosY, 100, 200, panel, queryEnterName->GetValue(), this);
	todolists.push_back(todo);
	listNameDialog->EndModal(wxID_OK);
	listNameDialog->Destroy();
	listNameDialog = nullptr;
}
void MainFrame::AddTaskFromInput()
{
	wxString description = inputField->GetValue();
	if (!description.IsEmpty()) {
		checkListBox->Insert(description, checkListBox->GetCount());
		inputField->Clear();
	}
	inputField->SetFocus();
}
void MainFrame::OnAddButtonClicked(wxCommandEvent& evt)
{
	AddTaskFromInput();
}
void MainFrame::OnInputEnter(wxCommandEvent& evt)
{
	AddTaskFromInput();
}
void MainFrame::OnClearButton(wxCommandEvent& evt)
{
	if (checkListBox->IsEmpty()) {
		return;
	}

	wxMessageDialog dialog(this, "Are you sure you want to clear your To-Do list?", "Clear", wxYES_NO | wxCANCEL);
	int result = dialog.ShowModal();

	if (result == wxID_YES) {
		checkListBox->Clear();
	}
}
void MainFrame::OnDeleteTask(wxCommandEvent& evt) {

	int selection = checkListBox->GetSelection();

	if (selection == wxNOT_FOUND) {
		return;
	}

	checkListBox->Delete(selection);

}

void MainFrame::moveDown(wxCommandEvent& evt)
{
	MoveSelectedTask(1);
}

void MainFrame::moveUp(wxCommandEvent& evt)
{
	MoveSelectedTask(-1);
}
