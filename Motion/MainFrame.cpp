#include "MainFrame.h"
#include "Note.h"
#include "CustomButton.h"
#include <wx/wx.h>
#include <string>
#include <vector>
#include <cmath>
#include <wx/wrapsizer.h>
#include <wx/splitter.h>
#include <wx/tglbtn.h>

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


MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	//activePtr = nullptr;
	ColorPtr = nullptr;
	NoteBColor = wxColor("#FFDB58");
	NoteFColor = wxColor("#2f2f2f");

	ColorChoices.Add("Foreground color");
	ColorChoices.Add("Background color");
	
	auto splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	panel = new wxPanel(splitter);
	panel->SetBackgroundColour(wxColor("#ffffff"));

	auto sidePanel = new wxPanel(splitter);
	auto sidePanelSizer = new wxBoxSizer(wxVERTICAL);
	sidePanel->SetBackgroundColour(wxColor("#f7f7f5"));
	sidePanel->SetSizerAndFit(sidePanelSizer);

	splitter->SplitVertically(sidePanel, panel, 100);
	splitter->SetMinimumPaneSize(220);


	auto sidePanelSplitter = new wxSplitterWindow(sidePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);
	sidePanelSizer->Add(sidePanelSplitter, 1, wxEXPAND);

	auto colorPanel = new wxPanel(sidePanelSplitter);
	auto colorWraper = new wxWrapSizer(wxHORIZONTAL);
	auto colorPanelSizer = new wxBoxSizer(wxVERTICAL);
	colorPanel->SetSizerAndFit(colorPanelSizer);

	auto notePanel = new wxPanel(sidePanelSplitter);
	auto notePanelSizer = new wxBoxSizer(wxVERTICAL);
	notePanel->SetSizerAndFit(notePanelSizer);

	sidePanelSplitter->SplitHorizontally(colorPanel, notePanel, 150);
	sidePanelSplitter->SetMinimumPaneSize(200);

	

	noteEnterText = new wxTextCtrl(notePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 300), wxTE_MULTILINE | wxTE_NO_VSCROLL);
	noteEnterText->SetForegroundColour(wxColor("9f9e9b"));

	auto stylingSizer = new wxBoxSizer(wxHORIZONTAL);

	auto button = new wxButton(notePanel, wxID_ANY, "Add note", wxDefaultPosition, wxSize(-1, 25));
	auto boldButton = new wxToggleButton(notePanel, wxID_ANY, "B", wxDefaultPosition, wxSize(25, 25));
	font = boldButton->GetFont();
	font.MakeBold();
	boldButton->SetFont(font);

	auto italicButton = new wxToggleButton(notePanel, wxID_ANY, "I", wxDefaultPosition, wxSize(25, 25));
	font = italicButton->GetFont();
	font.MakeItalic();
	italicButton->SetFont(font);

	auto underlineButton = new wxToggleButton(notePanel, wxID_ANY, "U", wxDefaultPosition, wxSize(25, 25));
	font = underlineButton->GetFont();
	font.MakeUnderlined();
	underlineButton->SetFont(font);

	stylingSizer->Add(button, 1);
	stylingSizer->Add(boldButton, 0);
	stylingSizer->Add(italicButton, 0);
	stylingSizer->Add(underlineButton, 0);



	auto updateButton = new wxButton(notePanel, wxID_ANY, "Update", wxDefaultPosition, wxSize(-1, 25));
	auto colorSelectSizer = new wxBoxSizer(wxHORIZONTAL);
	auto colorPanelTitle = new wxStaticText(colorPanel, wxID_ANY, "Color palete", wxDefaultPosition, wxDefaultSize, wxBOLD);

	colorPanelSizer->Add(colorPanelTitle, 0, wxALL, 4);
	colorPanelSizer->AddSpacer(3);
	colorPanelSizer->Add(colorSelectSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	colorPanelSizer->Add(colorWraper, 0, wxEXPAND | wxALL, 5 );

	F_BColorCheck = new wxRadioBox(colorPanel, wxID_ANY, "Choose what color to change: ", wxDefaultPosition, wxDefaultSize, ColorChoices, 1);
	F_BColorCheck->SetForegroundColour(wxColor("#000000"));
	F_BColorCheck->SetSelection(1);
	colorSelectSizer->Add(F_BColorCheck, 0);
	auto colorChoiceSizer = new wxBoxSizer(wxVERTICAL);
	colorSelectSizer->Add(colorChoiceSizer, 1, wxEXPAND);
	fColorChoice = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(25,25));
	fColorChoice->SetBackgroundColour(NoteFColor);
	bColorChoice = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(25, 25));
	bColorChoice->SetBackgroundColour(NoteBColor);
	colorChoiceSizer->AddSpacer(20);
	colorChoiceSizer->Add(fColorChoice, 0, wxALIGN_RIGHT );
	colorChoiceSizer->Add(bColorChoice, 0, wxALIGN_RIGHT );
	colorChoiceSizer->AddSpacer(5);
	notePanelSizer->Add(stylingSizer, 0, wxEXPAND | wxRIGHT | wxLEFT | wxUP, 2);
	notePanelSizer->Add(updateButton, 0, wxEXPAND | wxRIGHT | wxLEFT | wxDOWN, 2);
	notePanelSizer->Add(noteEnterText, 1, wxEXPAND| wxALL, 2);




	button->Bind(wxEVT_BUTTON, &MainFrame::OnNoteButtonClicked, this);
	boldButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnBoldSelect, this);
	italicButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnItalicSelect, this);
	underlineButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnUnderlinedSelect, this);
	updateButton->Bind(wxEVT_BUTTON, &MainFrame::OnUpdate, this);

	for (int i = 0; i < colorPalette.size(); i++) {
		auto color = new wxPanel(colorPanel, wxID_ANY, wxPoint(-1, -1), wxSize(25, 25));
		color->SetBackgroundColour(wxColor(colorPalette[i]));
		paletes.push_back(color);
	}
	for (auto color : paletes) {
		color->Bind(wxEVT_LEFT_DOWN, &MainFrame::OnColorChoose, this);
		color->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnColorHover, this);
		colorWraper->Add(color, 0, wxEXPAND);
	}

	CreateStatusBar();
}

void MainFrame::OnNoteButtonClicked(wxCommandEvent& evt) {
	auto message = noteEnterText->GetValue();
	font.SetWeight(wxFONTWEIGHT_NORMAL);
	font.SetStyle(wxFONTSTYLE_NORMAL);
	font.SetUnderlined(false);
	if (isBold) { font.MakeBold(); }
	if (isItalic) { font.MakeItalic(); }
	if (isUnderlined) { font.MakeUnderlined(); }

	Note* note = new Note(250, 100, 0, 250, 300, message, font, NoteFColor, NoteBColor, panel);
	wxLogStatus(wxString::Format("note created at (%d,%d)", note->GetPosition().x, note->GetPosition().y));
	evt.Skip();
	//Notes.push_back(note);
}
void MainFrame::OnUpdate(wxCommandEvent& evt) {
	auto message = noteEnterText->GetValue();
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
