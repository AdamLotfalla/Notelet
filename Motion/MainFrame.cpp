#include "MainFrame.h"
#include "Note.h"
#include <wx/wx.h>
#include <string>
#include <vector>
#include <cmath>
#include <wx/wrapsizer.h>
#include <wx/splitter.h>
#include <wx/tglbtn.h>
#include "PanPanel.h"

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
	sidePanel->Bind(wxEVT_CHAR_HOOK, &MainFrame::CreateNoteShortcut, this);
	panel->Bind(wxEVT_CHAR_HOOK, &MainFrame::OnDelete, this);

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
	//Notes.push_back(note);
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
	}
	if (evt.GetKeyCode() == WXK_DELETE) {
		active->Destroy();
		active = nullptr;
	}
	if (evt.GetKeyCode() == WXK_HOME) {
		active->Raise();
	}
	if (evt.GetKeyCode() == WXK_END) {
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
void MainFrame::SetActive(Note* activeNote) {
	if (activeNote == nullptr) {
		wxLogStatus("Attempting to set active note to nullptr.");
		return;
	}
	active = activeNote;
	wxLogStatus("Active ID: %i", active->GetId());
}