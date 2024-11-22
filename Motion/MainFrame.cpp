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
	this->Bind(wxEVT_CHAR_HOOK, &MainFrame::CreateNoteShortcut, this);

	Bind(wxEVT_MENU, &MainFrame::OnFileSave, this, wxID_SAVE);
	Bind(wxEVT_MENU, &MainFrame::OnFileSaveAs, this, wxID_SAVEAS);
	Bind(wxEVT_MENU, &MainFrame::OnFileOpen, this, wxID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::OnFileExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainFrame::OnNoteButtonClicked, this, wxID_NEW);
	Bind(wxEVT_MENU, &MainFrame::OnAddImage, this, wxID_ADD);


	//panel->Bind(wxEVT_CHAR_HOOK, &MainFrame::OnDelete, this);

	this->SetDoubleBuffered(true);
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

	// --- Panels ---
	colorPanel = new wxPanel(sidePanelSplitter);
	notePanel = new wxPanel(sidePanelSplitter);

	// --- Text Control ---
	noteEnterText = new wxTextCtrl(notePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_NO_VSCROLL);

	// --- Buttons and Toggles ---
	button = new wxButton(notePanel, wxID_ANY, "Add note", wxDefaultPosition, wxSize(-1, 25));
	updateButton = new wxButton(notePanel, wxID_ANY, "Update", wxDefaultPosition, wxSize(-1, 25));

	// Bold toggle button
	boldButton = new wxToggleButton(notePanel, wxID_ANY, "B", wxDefaultPosition, wxSize(25, 25));
	font = boldButton->GetFont();
	font.MakeBold();
	boldButton->SetFont(font);

	// Italic toggle button
	italicButton = new wxToggleButton(notePanel, wxID_ANY, "I", wxDefaultPosition, wxSize(25, 25));
	font = italicButton->GetFont();
	font.MakeItalic();
	italicButton->SetFont(font);

	// Underline toggle button
	underlineButton = new wxToggleButton(notePanel, wxID_ANY, "U", wxDefaultPosition, wxSize(25, 25));
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
	fColorChoice = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(25, 25));
	bColorChoice = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(25, 25));
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
		auto color = new wxPanel(colorPanel, wxID_ANY, wxPoint(-1, -1), wxSize(25, 25));
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
	splitter->SplitVertically(sidePanel, panel, 100);
	splitter->SetMinimumPaneSize(220);
	colorPanel->SetSizerAndFit(colorPanelSizer);
	notePanel->SetSizerAndFit(notePanelSizer);
	sidePanelSplitter->SplitHorizontally(colorPanel, notePanel, 150);
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

			Note* note = new Note(150, 150, 0, x, y, text, font, foreground, background, panel, this);
			notes.push_back(note);
		}
		child = child->GetNext();
	}

	wxLogStatus("File loaded successfully!");
}
void MainFrame::SetActive(Note* activeNote) {
	if (activeNote == nullptr) {
		wxLogStatus("Attempting to set active note to nullptr.");
		return;
	}
	active = activeNote;
	noteEnterText->SetLabel(active->text);
	noteEnterText->SelectAll();
	wxLogStatus("Active ID: %i", active->GetId());
}


// Events
void MainFrame::OnNoteButtonClicked(wxCommandEvent& evt) {
	auto message = noteEnterText->GetValue();
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	font.SetStyle(wxFONTSTYLE_NORMAL);
	font.SetUnderlined(false);
	if (isBold) { font.MakeBold(); }
	if (isItalic) { font.MakeItalic(); }
	if (isUnderlined) { font.MakeUnderlined(); }

	Note* note = new Note(150, 150, 0, noteDefaultPosX, noteDefaultPosY, message, font, NoteFColor, NoteBColor, panel, this);
	notes.push_back(note);
	wxLogStatus(wxString::Format("note created at (%d,%d)", note->GetPosition().x, note->GetPosition().y));
	noteEnterText->SelectAll();
	if (noteDefaultPosX + note->GetSize().x + 50 < this->GetSize().GetWidth() && noteDefaultPosY + note->GetSize().y + 50 < this->GetSize().GetHeight()) {
		noteDefaultPosX += 25;
		noteDefaultPosY += 25;
	}
	else {
		noteDefaultPosX = 0;
		noteDefaultPosY = 0;
	}
	evt.Skip();
}
void MainFrame::OnUpdateButtonClicked(wxCommandEvent& evt)
{
	auto message = noteEnterText->GetValue();
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	font.SetStyle(wxFONTSTYLE_NORMAL);
	font.SetUnderlined(false);
	if (isBold) { font.MakeBold(); }
	if (isItalic) { font.MakeItalic(); }
	if (isUnderlined) { font.MakeUnderlined(); }

	active->UpdateNote(150, 150, message, font, NoteFColor, NoteBColor);
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
	//dc.DrawRectangle(0, 0, 25, 25);
}
void MainFrame::CreateNoteShortcut(wxKeyEvent& evt)
{
	wxLogStatus("Key pressed");
	if (evt.ControlDown() && evt.GetKeyCode() == 'N') {
		wxLogStatus("control pressed");
		auto message = noteEnterText->GetValue();
		font.SetWeight(wxFONTWEIGHT_NORMAL);
		font.SetStyle(wxFONTSTYLE_NORMAL);
		font.SetUnderlined(false);
		if (isBold) { font.MakeBold(); }
		if (isItalic) { font.MakeItalic(); }
		if (isUnderlined) { font.MakeUnderlined(); }

		Note* note = new Note(150, 150, 0, noteDefaultPosX, noteDefaultPosY, message, font, NoteFColor, NoteBColor, panel, this);
		notes.push_back(note);
		wxLogStatus(wxString::Format("note created at (%d,%d)", note->GetPosition().x, note->GetPosition().y));
		noteEnterText->SelectAll();
		if (noteDefaultPosX + note->GetSize().x + 50 < this->GetSize().GetWidth() && noteDefaultPosY + note->GetSize().y + 50 < this->GetSize().GetHeight()) {
			noteDefaultPosX += 25;
			noteDefaultPosY += 25;
		}
		else {
			noteDefaultPosX = 0;
			noteDefaultPosY = 0;
		}

		evt.Skip(false);
		return;
	}
	else if (evt.GetKeyCode() == WXK_DELETE) {
		auto it = std::find(notes.begin(), notes.end(), active);
		notes.erase(it);
		active->Destroy();
		active = nullptr;
	}
	else if (evt.GetKeyCode() == WXK_HOME) {
		active->Raise();
	}
	else if (evt.GetKeyCode() == WXK_END) {
		active->Lower();
	}

	evt.Skip();
}
void MainFrame::OnDelete(wxKeyEvent& evt)
{
	if (active != nullptr) {
		active->Destroy();
		active = nullptr;
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
