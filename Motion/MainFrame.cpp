#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/wrapsizer.h>
#include <wx/icon.h>
#include <wx/tglbtn.h>
#include <wx/splitter.h>
#include <wx/menu.h>
#include <queue>
#include "colorDialog.h"
#include "Tool.h"
#include "Note.h"
#include <wx/dcbuffer.h>
#include "Rectangle.h"
#include <wx/graphics.h>
#include "Cursors.h"
#include <wx/app.h>
#include "ToDoList.h"

using namespace std;

#define TextColor isDark ? "#FFFFFF" : "#3d3d3d"
#define PanelColor1 isDark ? "#424242" : "#F3F6FE" //F9F6EE
#define PanelColor2 isDark ? "#4f4f4f" : "#FDFDFD"

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	isDark = true;
	ForegroundColor = "#2f2f2f";
	BackgroundColor = "#FFDB58";

	addToolBar(this);
	addSideBar(notToolBarPanel);
	addColorPanel(colorPanel);
	addEditPanel(editPanel);
	addMenuBar();

	H2Font.SetPointSize(16);
	H2Font.MakeBold();
	//H2Font.SetFaceName("Arial Unicode MS");

	this->SetDoubleBuffered(true);
	//Bind(wxEVT_KEY_DOWN, &MainFrame::Shortcuts, this);

	TextInput->Newline();
	TextInput->WriteText("Starting...");

	showBrushes = false;
	MbrushActive = true;
	brushSize = 3;
	fontSize = 9;
	InputTextColor = wxColor(paletteColors[20]);
	UpdateBrushes();
	ResetFormatting();
	CreateStatusBar();
	UpdateTextInfo();

	isDark = !isDark;
	wxCommandEvent dummyEvent(wxEVT_BUTTON, wxID_ANY);
	SwitchThemeButton(dummyEvent);
	TextInput->SelectAll();
	TextInput->DeleteSelection();
}



// adding layout
void MainFrame::addSideBar(wxWindow* parent)
{
	sidePanelsSplitter = new wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	sidePanel = new wxPanel(sidePanelsSplitter);
	addScrolledPanel();

	wxBoxSizer* sidePanelsSizer = new wxBoxSizer(wxHORIZONTAL);
	sidePanelsSizer->Add(sidePanelsSplitter, 1, wxEXPAND);

	sidePanelsSplitter->SplitVertically(sidePanel, scrollPanel, 200);
	sidePanelsSplitter->SetMinimumPaneSize(220);

	sidePanel->SetBackgroundColour(wxColor(100,100,200));
	
	parent->SetSizerAndFit(sidePanelsSizer);

	//edit panel
	editPanelSplitter = new wxSplitterWindow(sidePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	colorPanel = new wxPanel(editPanelSplitter);
	editPanel = new wxPanel(editPanelSplitter);

	editPanelSizer = new wxBoxSizer(wxVERTICAL);
	editPanelSizer->Add(editPanelSplitter, 1, wxEXPAND);

	colorPanel->SetBackgroundColour(wxColor(200, 200, 100));
	editPanel->SetBackgroundColour(wxColor(100, 200, 200));

	sidePanel->SetSizerAndFit(editPanelSizer);

	editPanelSplitter->SplitHorizontally(colorPanel, editPanel, -300);
	editPanelSplitter->SetMinimumPaneSize(170);
}

void MainFrame::addMenuBar()
{
	wxMenuBar* menuBar = new wxMenuBar();

	wxMenu* fileMenu = new wxMenu();
	wxMenu* addMenu = new wxMenu();
	wxMenu* settingsMenu = new wxMenu();

	fileMenu->Append(wxID_SAVE, "&Save\tCtrl-S", "Save the notes");
	fileMenu->Append(wxID_SAVEAS, "Save &As...", "Save the notes to a specific file");
	fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O", "Open a file to load notes");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "E&xit", "Exit the application");

	addMenu->Append(wxID_NEW, "&Add Note\tCtrl-N", "Add a new note");
	addMenu->Append(5, "&Add To-Do List", "Add a new To-Do list");

	settingsMenu->Append(55, "Switch Dark/Light", "Switch between dark and light modes");

	menuBar->Append(fileMenu, "&File");
	menuBar->Append(addMenu, "&Add");
	menuBar->Append(settingsMenu, "&Settings");

	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, &MainFrame::SwitchThemeButton, this, 55);
}

void MainFrame::addScrolledPanel()
{
	scrollPanel = new wxScrolledWindow(sidePanelsSplitter, wxID_ANY);
	scrollPanel->SetScrollRate(1, 1);
	scrollPanel->SetBackgroundStyle(wxBG_STYLE_PAINT); // Enable double buffering

	notSidePanel = new wxPanel(scrollPanel, wxID_ANY, wxDefaultPosition, wxSize(5000,5000));
	notSidePanel->SetBackgroundColour(wxColor(200, 100, 100));

	scrollPanel->SetVirtualSize(notSidePanel->GetSize());
	//scrollPanel->Bind(wxEVT_RIGHT_DOWN, &MainFrame::OnRightMouseDown, this);
	//scrollPanel->Bind(wxEVT_RIGHT_UP, &MainFrame::OnRightMouseUp, this);
	//scrollPanel->Bind(wxEVT_MOTION, &MainFrame::OnMouseMotion, this);

	notSidePanel->Bind(wxEVT_RIGHT_DOWN, &MainFrame::OnRightDown, this);
	notSidePanel->Bind(wxEVT_RIGHT_UP, &MainFrame::OnRightUp, this);
	notSidePanel->Bind(wxEVT_MOTION, &MainFrame::OnMouseMotion, this);

	notSidePanel->Bind(wxEVT_LEFT_DOWN, &MainFrame::OnLeftDown, this);
	notSidePanel->Bind(wxEVT_LEFT_UP, &MainFrame::OnLeftUp, this);

	scrollPanel->SetDoubleBuffered(true);
	notSidePanel->SetDoubleBuffered(true);

	scrollPanel->Scroll(notSidePanel->GetSize().x / 2, notSidePanel->GetSize().y / 2);

	//scrollPanel->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyDown, this);
	//scrollPanel->Bind(wxEVT_KEY_UP, &MainFrame::OnKeyUp, this);
	//scrollPanel->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnLeaveWindow, this);

	notSidePanel->Bind(wxEVT_PAINT, &MainFrame::Draw, this);
}

void MainFrame::addEditPanel(wxWindow* parent)
{
	wxBoxSizer* noteEditSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* SecondRowButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* FirstRowButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	
	TextInput = new wxRichTextCtrl(parent, wxID_ANY, "");

	//TextInput->SetHint("Enter Note message");

	addButton = new wxButton(parent, wxID_ANY, "+");
	addButton->SetToolTip("Add a new note \t CTRL+N");

	updateButton = new wxButton(parent, wxID_ANY, "Update");
	updateButton->SetToolTip("UpdateSelectedNote \t Ctrl+U");

	boldButton = new wxToggleButton(parent, wxID_ANY, "B", wxDefaultPosition, wxSize(25,25), wxNO_BORDER);
	boldButton->SetToolTip("Make Bold");
	boldButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnBoldClick, this);

	italicButton = new wxToggleButton(parent, wxID_ANY, "I", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	italicButton->SetToolTip("Make Italic");
	italicButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnItalicClick, this);

	underlineButton = new wxToggleButton(parent, wxID_ANY, "U", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	underlineButton->SetToolTip("Make Underlined");
	underlineButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnUnderlineClick, this);

	wxBitmap bitmap;
	
	wxIcon LalignIcon("./Lalign.ico", wxBITMAP_TYPE_ICO);
	wxIcon CalignIcon("./Calign.ico", wxBITMAP_TYPE_ICO);
	wxIcon RalignIcon("./Ralign.ico", wxBITMAP_TYPE_ICO);

	wxArrayString fontSizes = {
		"6",
		"8",
		"10",
		"11",
		"12",
		"14",
		"16",
		"20",
		"24",
		"30",
		"32",
		"36"
	};

	FontSizeBox = new wxComboBox(parent, wxID_ANY, to_string(fontSize), wxDefaultPosition, wxSize(-1, 25), fontSizes);

	bitmap = wxBitmap(LalignIcon);
	bitmap.Rescale(bitmap, wxSize(15, 15));
	LeftAlignButton = new wxBitmapButton(parent, wxID_ANY, bitmap, wxDefaultPosition, wxSize(25, 25), wxALIGN_CENTER | wxNO_BORDER);
	LeftAlignButton->SetToolTip("Left Align");
	LeftAlignButton->Bind(wxEVT_BUTTON, &MainFrame::OnLeftAlignClick, this);
	
	bitmap = wxBitmap(RalignIcon);
	bitmap.Rescale(bitmap, wxSize(15, 15));
	RightAlignButton = new wxBitmapButton(parent, wxID_ANY, bitmap, wxDefaultPosition, wxSize(25, 25), wxALIGN_CENTER | wxNO_BORDER);
	RightAlignButton->SetToolTip("Right Align");
	RightAlignButton->Bind(wxEVT_BUTTON, &MainFrame::OnRightAlignClick, this);
	
	bitmap = wxBitmap(CalignIcon);
	bitmap.Rescale(bitmap, wxSize(15, 15));
	CenterAlignButton = new wxBitmapButton(parent, wxID_ANY, bitmap, wxDefaultPosition, wxSize(25, 25), wxALIGN_CENTER | wxNO_BORDER);
	CenterAlignButton->SetToolTip("Center Align");
	CenterAlignButton->Bind(wxEVT_BUTTON, &MainFrame::OnCenterAlignClick, this);

	textColorPreview = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 25), wxBORDER_SIMPLE);
	textColorPreview->SetToolTip("Text color\n the same as the foreground color");

	noteEditSizer->AddSpacer(5);
	noteEditSizer->Add(FirstRowButtonSizer, 0, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->Add(SecondRowButtonSizer, 0, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->AddSpacer(5);
	noteEditSizer->Add(TextInput, 1, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->AddSpacer(5);
	noteEditSizer->Add(addButton, 0, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->Add(updateButton, 0, wxEXPAND | wxRIGHT | wxLEFT , 5);
	noteEditSizer->AddSpacer(5);


	FirstRowButtonSizer->Add(FontSizeBox, 1, wxEXPAND);

	SecondRowButtonSizer->Add(textColorPreview, 1, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(LeftAlignButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(CenterAlignButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(RightAlignButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(boldButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(italicButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(underlineButton, 0, wxEXPAND);

	//TextInput->BeginTextColour(ForegroundColor);

	parent->SetSizerAndFit(noteEditSizer);

	//FontSizeBox->Bind(wxEVT_TEXT, &MainFrame::OnFontSizeSelect, this);!
	FontSizeBox->Bind(wxEVT_COMBOBOX, &MainFrame::OnFontSizeSelect, this);

	TextInput->SetFocus();
	TextInput->Bind(wxEVT_KEY_DOWN, &MainFrame::TextInputShortcuts, this);
	TextInput->Bind(wxEVT_LEFT_DOWN, &MainFrame::OnCaretClick, this);
	TextInput->Bind(wxEVT_TEXT, &MainFrame::TextInputType, this);

	addButton->Bind(wxEVT_BUTTON, &MainFrame::OnAddButtonClick, this);
	updateButton->Bind(wxEVT_BUTTON, &MainFrame::OnUpdateButtonClick, this);
}



// theme
void MainFrame::SwitchThemeButton(wxCommandEvent& evt)
{
	for (auto child : this->GetChildren()) {
		themeQueue1.emplace(child);
	}

	isDark = !isDark;
	index = 0;
	grandChildren = 0;
	hasGrandChildren = false;

	switchTheme();
}

void MainFrame::UpdateBrushes()
{
	XSbrush->SetBackgroundColour(XSbrushActive ? wxColor(135, 135, 135) : XSbrush->GetParent()->GetBackgroundColour());
	Sbrush->SetBackgroundColour(SbrushActive ? wxColor(135, 135, 135) : Sbrush->GetParent()->GetBackgroundColour());
	Mbrush->SetBackgroundColour(MbrushActive ? wxColor(135, 135, 135) : Mbrush->GetParent()->GetBackgroundColour());
	Lbrush->SetBackgroundColour(LbrushActive ? wxColor(135, 135, 135) : Lbrush->GetParent()->GetBackgroundColour());
	XLbrush->SetBackgroundColour(XLbrushActive ? wxColor(135, 135, 135) : XLbrush->GetParent()->GetBackgroundColour());

	XSbrush->ChangeIcon(isDark ? "./XSbrushDarkMode.ico" : "./XSbrushWhiteMode.ico");
	Sbrush->ChangeIcon(isDark ? "./SbrushDarkMode.ico" : "./SbrushWhiteMode.ico");
	Mbrush->ChangeIcon(isDark ? "./MbrushDarkMode.ico" : "./MbrushWhiteMode.ico");
	Lbrush->ChangeIcon(isDark ? "./LbrushDarkMode.ico" : "./LbrushWhiteMode.ico");
	XLbrush->ChangeIcon(isDark ? "./XLbrushDarkMode.ico" : "./XLbrushWhiteMode.ico");

	XSbrush->Refresh();
	Sbrush->Refresh();
	Mbrush->Refresh();
	Lbrush->Refresh();
	XLbrush->Refresh();

	XSbrush->Show(showBrushes);
	Sbrush->Show(showBrushes);
	Mbrush->Show(showBrushes);
	Lbrush->Show(showBrushes);
	XLbrush->Show(showBrushes);

	toolBarPanel->Refresh();
	toolBarPanel->Layout();
}

void MainFrame::ResetFormatting()
{
	wxCommandEvent dummyEvent;

	if (Bold) { OnBoldClick(dummyEvent); }
	if (Italic) { OnItalicClick(dummyEvent); }
	if (Underline) { OnUnderlineClick(dummyEvent); }
	if (TextInput->HasSelection()) {
		TextInput->SetStyle(TextInput->GetSelectionRange(), TextInput->GetBasicStyle());
	}
	fontSize = 9;
	InputTextColor = wxColor(paletteColors[20]);
	attr.SetFontSize(fontSize);
	attr.SetTextColour(InputTextColor);
	TextInput->SetDefaultStyle(attr);

	ForcedChange = true;
	UpdateTextInfo();
}

void MainFrame::switchTheme()
{
	while(!themeQueue1.empty()) {
		index++;
		themeQueue1.front()->SetForegroundColour(TextColor);
		themeQueue1.front()->SetBackgroundColour((index % 2) ? PanelColor1 : PanelColor2);

		Refresh();

		for (auto child : themeQueue1.front()->GetChildren()) {
			grandChildren += child->GetChildren().GetCount();
			if (dynamic_cast<Note*>(child)) {
				continue;
			}
			themeQueue2.emplace(child);
		}

		hasGrandChildren = grandChildren > 0 ? true : false;
		if (!hasGrandChildren) {
			for (auto child : themeQueue1.front()->GetChildren()) {

				//if (child->IsKindOf(wxCLASSINFO(Note))) {
				//	continue;
				//}

				themeQueue2.pop();

				wxColor Fcolor = themeQueue1.front()->GetForegroundColour();
				wxColor Bcolor = themeQueue1.front()->GetBackgroundColour();

				wxSizer* containingSizer = child->GetContainingSizer();

				if (containingSizer && containingSizer->IsKindOf(wxCLASSINFO(wxWrapSizer))) {
					continue;
				}

				child->SetForegroundColour(Fcolor);
				child->SetBackgroundColour(Bcolor);

				/*
				if (wxDynamicCast(child, wxRadioBox)){
					wxRadioBox* radio = wxDynamicCast(child, wxRadioBox);

					//do not know what to do
				}
				*/
			}

			Refresh();
		}

		themeQueue1.pop();
	}

	themeQueue1 = themeQueue2;

	while (!themeQueue2.empty()) {
		themeQueue2.pop();
	}

	index = 0;
	grandChildren = 0;

	if (!themeQueue1.empty()) {
		switchTheme();
	}

	FcolorChoice->SetBackgroundColour(ForegroundColor);
	BcolorChoice->SetBackgroundColour(BackgroundColor);

	NoteTool->ChangeIcon(isDark ? "./NoteDarkMode.ico" : "./NoteWhiteMode.ico");
	ToDoTool->ChangeIcon(isDark ? "./ToDoDarkMode.ico" : "./ToDoWhiteMode.ico");
	RectangleTool->ChangeIcon(isDark ? "./RectangleDarkMode.ico" : "./RectangleWhiteMode.ico");
	ElipseTool->ChangeIcon(isDark ? "./ElipseDarkMode.ico" : "./ElipseWhiteMode.ico");
	BrushTool->ChangeIcon(isDark ? "./BrushDarkMode.ico" : "./BrushWhiteMode.ico");
	EraseTool->ChangeIcon(isDark ? "./EraseDarkMode.ico" : "./EraseWhiteMode.ico");

	textColorPreview->SetBackgroundColour(ForegroundColor);
	TextInput->SetBackgroundColour(*wxWHITE);
	UpdateColors();

	UpdateTextInfo();
	UpdateToolInfo();
	UpdateBrushes();

	Refresh();
}




// note controls
//void MainFrame::SetActive(Note* ACTIVENOTE) {
//	if (ACTIVENOTE == nullptr) {
//		wxLogStatus("Attempting to set active note to nullptr.");
//		return;
//	}
//	activeNote = ACTIVENOTE;
//	//noteEnterText->SetLabel(activeNote->text);
//	//noteEnterText->SelectAll();
//	wxLogStatus("Active ID: %i", activeNote->GetId());
//}

void MainFrame::UpdateTextInfo()
{
	long caretPos = TextInput->GetCaretPosition();
	wxRichTextAttr style;

	wxLogStatus("%i", ForcedChange);

	if (caretPos > 0 && !ForcedChange) {
		TextInput->GetStyle(caretPos, style);
		attr = style;
		TextInput->SetDefaultStyle(attr);

		switch (style.GetAlignment())
		{
		case wxTEXT_ALIGNMENT_LEFT:
			Laligntoggle = true;
			Caligntoggle = false;
			Raligntoggle = false;
			break;
		case wxTEXT_ALIGNMENT_CENTER:
			Laligntoggle = false;
			Caligntoggle = true;
			Raligntoggle = false;
			break;
		case wxTEXT_ALIGNMENT_RIGHT:
			Laligntoggle = false;
			Caligntoggle = false;
			Raligntoggle = true;
			break;
		default:
			break;
		}


		if (style.GetFontWeight() == wxFONTWEIGHT_BOLD) { Bold = true; }
		else { Bold = false; }

		if (style.GetFontStyle() == wxFONTSTYLE_ITALIC) { Italic = true; }
		else { Italic = false; }

		if (style.GetFontUnderlined()) { Underline = true; }
		else { Underline = false; }

		InputTextColor = style.GetTextColour();
	}


	//TextInput->GetStyle(caretPos, style);
	//switch (style.GetAlignment())
	//{
	//case wxTEXT_ALIGNMENT_LEFT:
	//	LeftAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
	//	Laligntoggle = true;
	//	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	//	Raligntoggle = false;
	//	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	//	Caligntoggle = false;
	//	break;
	//case wxTEXT_ALIGNMENT_CENTER:
	//	CenterAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
	//	Caligntoggle = true;
	//	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	//	Laligntoggle = false;
	//	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	//	Raligntoggle = false;
	//	break;
	//case wxTEXT_ALIGNMENT_RIGHT:
	//	RightAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
	//	Raligntoggle = true;
	//	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	//	Laligntoggle = false;
	//	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	//	Caligntoggle = false;
	//	break;
	//default:
	//	break;
	//}

	LeftAlignButton->SetBackgroundColour(Laligntoggle ? wxColor(135, 135, 135) : LeftAlignButton->GetParent()->GetBackgroundColour());
	CenterAlignButton->SetBackgroundColour(Caligntoggle ? wxColor(135, 135, 135) : CenterAlignButton->GetParent()->GetBackgroundColour());
	RightAlignButton->SetBackgroundColour(Raligntoggle ? wxColor(135, 135, 135) : RightAlignButton->GetParent()->GetBackgroundColour());

	////boldButton->SetBackgroundColour(style.GetFontWeight() == wxFONTWEIGHT_BOLD ? wxColor(135, 135, 135): boldButton->GetParent()->GetBackgroundColour());
	////italicButton->SetBackgroundColour(style.GetFontStyle() == wxFONTSTYLE_ITALIC ? wxColor(135, 135, 135): italicButton->GetParent()->GetBackgroundColour());
	////underlineButton->SetBackgroundColour(style.GetFontUnderlined() ? wxColor(135, 135, 135): underlineButton->GetParent()->GetBackgroundColour());
	
	boldButton->SetBackgroundColour(Bold ? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	italicButton->SetBackgroundColour(Italic ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	underlineButton->SetBackgroundColour(Underline ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());

	FontSizeBox->SetValue(to_string(fontSize));
	textColorPreview->SetBackgroundColour(InputTextColor);

	editPanel->Refresh();

	//textColorPreview->Refresh();
	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();
	//textColorPreview->Refresh();
	//LeftAlignButton->Refresh();
	//RightAlignButton->Refresh();
	//CenterAlignButton->Refresh();
	//FontSizeBox->Refresh();
	//FontSizeBox->Update();

	TextInput->SetFocus();
	ForcedChange = false;
}

void MainFrame::UpdateToolInfo()
{
	RectangleTool->SetBackgroundColour(isDrawingRect ? wxColor(135, 135, 135) : RectangleTool->GetParent()->GetBackgroundColour());
	BrushTool->SetBackgroundColour(isBrushActive ? wxColor(135, 135, 135) : BrushTool->GetParent()->GetBackgroundColour());
	EraseTool->SetBackgroundColour(isEraseActive? wxColor(135, 135, 135) : EraseTool->GetParent()->GetBackgroundColour());

}




// colors
void MainFrame::addColorPanel(wxWindow* parent)
{
	parent->SetBackgroundColour(PanelColor2);
	wxBoxSizer* verticalColorPanelSizer = new wxBoxSizer(wxVERTICAL);
	wxPanel* spacerColorPanel = new wxPanel(parent);

	wxStaticText* ColorPanelTitle = new wxStaticText(parent, wxID_ANY, "Colors");
	wxArrayString ColorTypes;
	ColorTypes.Add("Foreground Color");
	ColorTypes.Add("Background Color");


	ColorTypeRadioBox = new wxRadioBox(parent, wxID_ANY, "Choose what color to change"
		, wxDefaultPosition, wxDefaultSize, ColorTypes, 1, wxNO_BORDER);

	wxBoxSizer* ColorTypeHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* ColorTypeVerticalSizer = new wxBoxSizer(wxVERTICAL);

	FcolorChoice = new wxPanel(parent, wxID_ANY, ColorTypeRadioBox->GetPosition(), wxSize(25, 25));
	BcolorChoice = new wxPanel(parent, wxID_ANY, ColorTypeRadioBox->GetPosition(), wxSize(25, 25));

	FcolorChoice->SetBackgroundColour(ForegroundColor);
	BcolorChoice->SetBackgroundColour(BackgroundColor);

	colorWrapSizer = new wxWrapSizer(wxHORIZONTAL);

	UpdateColors();

	ColorPanelTitle->SetFont(H2Font);

	verticalColorPanelSizer->AddSpacer(5);
	verticalColorPanelSizer->Add(ColorPanelTitle, 0, wxEXPAND | wxALL, 7);

	ColorTypeHorizontalSizer->Add(ColorTypeRadioBox, 1, wxEXPAND);
	ColorTypeHorizontalSizer->AddSpacer(3);
	ColorTypeHorizontalSizer->Add(ColorTypeVerticalSizer, 0, wxEXPAND);

	ColorTypeVerticalSizer->AddSpacer(15);
	ColorTypeVerticalSizer->Add(FcolorChoice, 0, wxEXPAND);
	ColorTypeVerticalSizer->AddSpacer(3);
	ColorTypeVerticalSizer->Add(BcolorChoice, 0, wxEXPAND);

	verticalColorPanelSizer->Add(ColorTypeHorizontalSizer, 0, wxEXPAND | wxALL, 5);
	verticalColorPanelSizer->Add(colorWrapSizer, 0, wxEXPAND | wxALL, 5);
	verticalColorPanelSizer->Add(spacerColorPanel, 1, wxEXPAND | wxALL, 5);

	spacerColorPanel->SetBackgroundColour(parent->GetBackgroundColour());
	parent->SetSizerAndFit(verticalColorPanelSizer);
}

void MainFrame::OnSwatchHover(wxMouseEvent& evt)
{
	wxWindow* panel = dynamic_cast<wxWindow*>(evt.GetEventObject());

	if (panel) {
		panel->SetCursor(wxCURSOR_HAND);
		ColorBuffer = panel->GetBackgroundColour();
	}

	evt.Skip();
}

void MainFrame::OnSwatchSelect(wxMouseEvent& evt)
{
	if (ColorTypeRadioBox->GetSelection() == 0) {
		ForegroundColor = ColorBuffer;
		FcolorChoice->SetBackgroundColour(ForegroundColor);
		textColorPreview->SetBackgroundColour(ForegroundColor);

		attr.SetTextColour(ForegroundColor);
		TextInput->SetDefaultStyle(attr);

		if (TextInput->HasSelection()) {

			auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
			auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();

			for (long i = startCaretPosition ; i < endCaretPosition; ++i)
			{
				wxRichTextAttr charAttr;
				TextInput->GetStyle(i, charAttr);
				charAttr.SetTextColour(ForegroundColor); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
				TextInput->Update();
				TextInput->Refresh();
			}
		}

	}
	else {
		BackgroundColor = ColorBuffer;
		BcolorChoice->SetBackgroundColour(BackgroundColor);
	}

	Refresh();
}

void MainFrame::AddNewColor(wxCommandEvent& evt)
{
	//wxLogMessage("New colors will be added soon!");

	ColorDialog chooseColor(this, "Choose Color", this);

	chooseColor.ShowModal();

	//if (chooseColor.ShowModal() == wxID_OK) {
	//	wxMessageBox("Dialog closed with OK", "Info", wxOK | wxICON_INFORMATION);
	//}
	//else {
	//	wxMessageBox("Dialog closed with Cancel", "Info", wxOK | wxICON_INFORMATION);
	//}
}

void MainFrame::UpdateColors()
{
	colorWrapSizer->Clear(true);
	paletteSwatches.clear();

	wxIcon AddIcon("./plus.ico", wxBITMAP_TYPE_ICO);
	wxBitmap AddBitmap(AddIcon);
	AddBitmap.Rescale(AddBitmap, wxSize(15, 15));

	wxBitmapButton* newColorButton = new wxBitmapButton(colorPanel, wxID_ANY, AddBitmap, wxDefaultPosition, wxSize(25, 25), wxNO_BORDER | wxALIGN_CENTER);
	newColorButton->SetBackgroundColour("#808080");
	newColorButton->Bind(wxEVT_BUTTON, &MainFrame::AddNewColor, this);
	newColorButton->SetMinSize(wxSize(25, 25));
	colorWrapSizer->Add(newColorButton, 0, wxEXPAND | wxCENTER);

	for (int i = 0; i < paletteColors.size(); i++) {
		wxPanel* swatch = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(25, 25), wxColor(paletteColors[i]) == colorPanel->GetBackgroundColour() ? wxBORDER_SIMPLE : wxNO_BORDER);
		swatch->SetBackgroundColour(wxColor(paletteColors[i]));
		swatch->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnSwatchHover, this);
		swatch->Bind(wxEVT_LEFT_DOWN, &MainFrame::OnSwatchSelect, this);
		swatch->SetToolTip(paletteColors[i]);
		colorWrapSizer->Add(swatch, 0, wxEXPAND);
		paletteSwatches.push_back(swatch);
	}

	colorPanel->Layout();
}



// tool buttons
void MainFrame::addToolBar(wxWindow* parent)
{
	//objects
	toolBarPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 30));
	notToolBarPanel = new wxPanel(parent);

	wxBoxSizer* toolBarSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* toolSizer = new wxBoxSizer(wxHORIZONTAL);

	//sizers
	parent->SetSizerAndFit(toolBarSizer);
	toolBarSizer->Add(toolBarPanel, 0, wxEXPAND);
	toolBarSizer->Add(notToolBarPanel, 1, wxEXPAND);


	//style
	toolBarPanel->SetBackgroundColour(PanelColor1);
	notToolBarPanel->SetBackgroundColour(wxColor(100, 200, 100));

	//Tools
	NoteTool = new Tool(toolBarPanel, "Add a note \t Ctrl+N", isDark ? "./NoteDarkMode.ico" : "./NoteWhiteMode.ico");
	ToDoTool = new Tool(toolBarPanel, "Add a To-Do list \t Ctrl+T", isDark ? "./ToDoDarkMode.ico" : "./ToDoWhiteMode.ico");
	RectangleTool = new Tool(toolBarPanel, "Draw a rectangle \t Ctrl+R", isDark ? "./RectangleDarkMode.ico" : "./RectangleWhiteMode.ico");
	ElipseTool = new Tool(toolBarPanel, "Draw an Elipse \t Ctrl+E", isDark ? "./ElipseDarkMode.ico" : "./ElipseWhiteMode.ico");
	BrushTool = new Tool(toolBarPanel, "Draw using the Brush", isDark ? "./BrushDarkMode.ico" : "./BrushWhiteMode.ico");
	EraseTool = new Tool(toolBarPanel, "Erase strokes", isDark ? "./EraseDarkMode.ico" : "./EraseWhiteMode.ico");

	XSbrush = new Tool(toolBarPanel, "1px brush nib", isDark ? "./XSbrushDarkMode.ico" : "./XSbrushWhiteMode.ico");
	Sbrush = new Tool(toolBarPanel, "2px brush nib", isDark ? "./SbrushDarkMode.ico" : "./SbrushWhiteMode.ico");
	Mbrush = new Tool(toolBarPanel, "3px brush nib", isDark ? "./MbrushDarkMode.ico" : "./MbrushWhiteMode.ico");
	Lbrush = new Tool(toolBarPanel, "5px brush nib", isDark ? "./LbrushDarkMode.ico" : "./LbrushWhiteMode.ico");
	XLbrush = new Tool(toolBarPanel, "10px brush nib", isDark ? "./XLbrushDarkMode.ico" : "./XLbrushWhiteMode.ico");


	toolSizer->Add(NoteTool, 0, wxEXPAND);
	toolSizer->Add(ToDoTool, 0, wxEXPAND);
	toolSizer->Add(RectangleTool, 0, wxEXPAND);
	toolSizer->Add(ElipseTool, 0, wxEXPAND);
	toolSizer->Add(BrushTool, 0, wxEXPAND);
	toolSizer->Add(EraseTool, 0, wxEXPAND);
	toolSizer->AddStretchSpacer();
	toolSizer->Add(XSbrush, 0, wxEXPAND);
	toolSizer->Add(Sbrush, 0, wxEXPAND);
	toolSizer->Add(Mbrush, 0, wxEXPAND);
	toolSizer->Add(Lbrush, 0, wxEXPAND);
	toolSizer->Add(XLbrush, 0, wxEXPAND);
	toolSizer->AddSpacer(50);



	NoteTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::AddNote, this);
	ToDoTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::AddToDo, this);
	RectangleTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::DrawRectangle, this);
	BrushTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::ActivateBrushTool, this);
	EraseTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::ActivateEraseTool, this);

	XSbrush->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::XSbrushEvent, this);
	Sbrush->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::SbrushEvent, this);
	Mbrush->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::MbrushEvent, this);
	Lbrush->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::LbrushEvent, this);
	XLbrush->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::XLbrushEvent, this);

	toolBarPanel->SetSizerAndFit(toolSizer);
	this->Layout();
	isDrawingRect = false;
}

void MainFrame::AddNote(wxCommandEvent& evt)
{
	int offset = 200;

	noteDefaultPositionX = scrollPanel->GetScrollPos(wxHORIZONTAL) + offset;
	noteDefaultPositionY = scrollPanel->GetScrollPos(wxVERTICAL) + offset;

	Note* note = new Note(150, 150, 0, noteDefaultPositionX, noteDefaultPositionY, TextInput, ForegroundColor, BackgroundColor, notSidePanel, this);

	TextInput->SelectAll();

	note->Raise();

	isDrawingRect = false;
	isBrushActive = false;
	isEraseActive = false;
	showBrushes = false;

	UpdateBrushes();
	UpdateBrushes();
	UpdateToolInfo();
}

void MainFrame::AddToDo(wxCommandEvent& evt)
{
	int offset = 150;

	noteDefaultPositionX = scrollPanel->GetScrollPos(wxHORIZONTAL) + offset;
	noteDefaultPositionY = scrollPanel->GetScrollPos(wxVERTICAL) + offset;

	todoCount++;

	wxString title = "To-Do List #" + to_string(todoCount);

	ToDoList* todo = new ToDoList(noteDefaultPositionX, noteDefaultPositionY, 150, 300, notSidePanel, title, this);

	todo->Raise();

	isDrawingRect = false;
	isBrushActive = false;
	isEraseActive = false;
	showBrushes = false;


	UpdateBrushes();
	UpdateToolInfo();
}

void MainFrame::DrawRectangle(wxCommandEvent& evt)
{
	isDrawingRect = !isDrawingRect;
	isBrushActive = false;
	isEraseActive = false;
	showBrushes = false;


	wxLogStatus("DrawingRectangle");
	SetCursor(wxCURSOR_CROSS);


	UpdateBrushes();
	UpdateToolInfo();
}

void MainFrame::ActivateBrushTool(wxCommandEvent& evt)
{
	isBrushActive = !isBrushActive;
	isDrawingRect = false;
	isEraseActive = false;
	showBrushes = true;

	UpdateBrushes();
	UpdateToolInfo();
}

void MainFrame::ActivateEraseTool(wxCommandEvent& evt)
{
	isEraseActive = !isEraseActive;
	isDrawingRect = false;
	isBrushActive = false;
	showBrushes = true;

	UpdateBrushes();
	UpdateToolInfo();
}



// tool gestures
void MainFrame::OnRightDown(wxMouseEvent& evt)
{
	if (!notSidePanel->HasCapture()) {
		notSidePanel->CaptureMouse();
	}
	m_isPanning = true;
	endPos = wxGetMousePosition();
	m_scrollOffset = scrollPanel->GetViewStart(); // Get current scroll position in logical units
	SetCursor(wxCURSOR_HAND);
}

void MainFrame::OnLeftDown(wxMouseEvent& evt)
{
	if (evt.GetEventObject() == notSidePanel) {
		wxLogStatus("Left down triggered fron notSidePanel");
		activeNote = nullptr;
	}

	this->SetFocus();
	if (activeRectangle != nullptr) {
		activeRectangle->active = false;
		activeRectangle->sizerKnob->Show(false);
		activeRectangle = nullptr;
	}

	startPos = evt.GetPosition();

	if (isDrawingRect) {

		if (!notSidePanel->HasCapture()) {
			notSidePanel->CaptureMouse();
		}

		bufferPoint = startPos;

		drawnPanel = new rectangle(notSidePanel, startPos, wxSize(0, 0), 5, ForegroundColor, BackgroundColor, this);
		//drawnPanel = new wxPanel(notSidePanel, wxID_ANY, startPos, wxSize(10, 10));
		//drawnPanel->SetBackgroundColour(*wxGREEN);
		drawnPanel->Raise();
		//drawnPanel->SetTransparent(true);
		drawnPanel->SetDoubleBuffered(true);
		activeRectangle = drawnPanel;
		//drawnPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
		//rectangles.push_back(drawnPanel);

		wxLogStatus("LeftDown, rectangle drawn at (%i,%i)", drawnPanel->GetPosition().x, drawnPanel->GetPosition().y);
	}

	if (isBrushActive) {
		if (!notSidePanel->HasCapture()) {
			notSidePanel->CaptureMouse();
		}

		Stroke stroke(ForegroundColor, brushSize);
		stroke.points.push_back(startPos);
		strokes.push_back(stroke);
	}
	wxLogStatus("down");
	//notSidePanel->Refresh();
	notSidePanel->SetDoubleBuffered(true);
}

void MainFrame::OnRightUp(wxMouseEvent& evt) {
	if (notSidePanel->HasCapture()) {
		notSidePanel->ReleaseMouse();
	}
	m_isPanning = false;
	SetCursor(wxCURSOR_DEFAULT);
}

void MainFrame::OnLeftUp(wxMouseEvent& evt)
{
	if (isDrawingRect && activeRectangle != nullptr) { 
		if (notSidePanel->HasCapture()) {
			notSidePanel->ReleaseMouse();
		}
		int sizeX = std::max(drawnPanel->GetSize().x, 25);
		int sizeY = std::max(drawnPanel->GetSize().y, 25);

		drawnPanel->SetSize(wxSize(sizeX, sizeY));

		wxLogStatus("LeftUp");
		drawnPanel->sizerKnob->SetPosition(wxPoint(drawnPanel->GetSize().GetWidth() - 15, drawnPanel->GetSize().GetHeight() - 15));
		wxLogStatus("drawn panel (%i,%i)", drawnPanel->GetPosition().x, drawnPanel->GetPosition().y);
	}

	if (isBrushActive) {
		if (notSidePanel->HasCapture()) {
			notSidePanel->ReleaseMouse();
		}

		int LastStrokeIndex = strokes.size();

		wxLogStatus("Refreshed, number of circles: %i", int(strokes[LastStrokeIndex - 1].points.size()));
	}

	isDrawingRect = false;
	notSidePanel->Refresh();
	SetCursor(wxCURSOR_DEFAULT);
}

void MainFrame::OnMouseMotion(wxMouseEvent& evt) {
	if (m_isPanning && evt.Dragging() && evt.RightIsDown()) {
		wxPoint delta = wxGetMousePosition() - endPos;
		wxPoint scrollCord = wxPoint(scrollPanel->GetScrollPos(wxHORIZONTAL), scrollPanel->GetScrollPos(wxVERTICAL));
		scrollPanel->Scroll(scrollCord - delta);
	}
	endPos = wxGetMousePosition();

	if (isDrawingRect && evt.LeftIsDown()) {
		endPos = evt.GetPosition();

		if (bufferPoint.x > endPos.x && bufferPoint.y > endPos.y) {
			drawnPanel->SetPosition(bufferPoint - (startPos - endPos));
		}
		else if (bufferPoint.x > endPos.x && bufferPoint.y < endPos.y) {
			drawnPanel->SetPosition(wxPoint(bufferPoint.x - (startPos.x - endPos.x), bufferPoint.y));
		}
		else if (bufferPoint.x < endPos.x && bufferPoint.y > endPos.y) {
			drawnPanel->SetPosition(wxPoint(bufferPoint.x, bufferPoint.y - (startPos.y - endPos.y)));
		}

		drawnPanel->SetSize(abs((endPos - startPos).x), abs((endPos - startPos).y));

		drawnPanel->Refresh(); // Refresh only the drawn panel
	}

	if (isBrushActive && evt.LeftIsDown()) {
		wxPoint mousePos = evt.GetPosition();
		strokes.back().points.push_back(mousePos);
		wxLogStatus("Drawing");

		// Get the last two points and refresh only the affected area
		if (strokes.back().points.size() > 1) {
			wxPoint lastPoint = strokes.back().points[strokes.back().points.size() - 2];
			wxPoint newPoint = strokes.back().points.back();

			wxRect updateRegion = wxRect(lastPoint, newPoint);
			updateRegion.Inflate(brushSize + 2); // Expand to include brush size

			notSidePanel->RefreshRect(updateRegion);
		}
	}

	wxPoint mousePos = evt.GetPosition();

	if (isEraseActive && evt.LeftIsDown()) {
		float eraseRadius = 10.0f; // Adjust eraser sensitivity
		wxRect eraseBounds(mousePos.x - eraseRadius, mousePos.y - eraseRadius,
			eraseRadius * 2, eraseRadius * 2);
		bool erasedSomething = false;

		strokes.erase(std::remove_if(strokes.begin(), strokes.end(),
			[&](const auto& stroke) {
				for (const wxPoint& point : stroke.points) {
					if (std::hypot(mousePos.x - point.x, mousePos.y - point.y) < eraseRadius) {
						eraseBounds.Union(wxRect(point, wxSize(1, 1))); // Expand erase region
						erasedSomething = true;
						return true; // Mark stroke for removal
					}
				}
				return false; // Keep stroke
			}),
			strokes.end()
		);

		// Only refresh the affected area if something was erased
		if (erasedSomething) {
			eraseBounds.Inflate(1000); // Slightly expand the region for better clearing
			notSidePanel->RefreshRect(eraseBounds);
		}
	}

	evt.Skip();
}



// styling
void MainFrame::OnBoldClick(wxCommandEvent& evt)
{
	Bold = !Bold;
	attr.SetFontWeight(Bold? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
	TextInput->SetDefaultStyle(attr);

	if (TextInput->HasSelection()) {

		bool flag = TextInput->IsSelectionBold() ? true : false;

		for (long i = TextInput->GetSelectionRange().GetStart(); i < TextInput->GetSelectionRange().GetEnd(); ++i) {
			wxRichTextAttr charAttr;

			TextInput->GetStyle(i, charAttr);
			charAttr.SetFontWeight(flag ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD);
			TextInput->SetStyle(i, i + 1, charAttr);
			TextInput->Update();
			TextInput->Refresh();

		}
		wxLogStatus("Has selection");
	}

	//CallAfter(&MainFrame::UpdateTextInfo);
	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void MainFrame::OnItalicClick(wxCommandEvent& evt)
{
	Italic = !Italic;
	italicButton->SetBackgroundColour(Italic ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());

	attr.SetFontStyle(Italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
	TextInput->SetDefaultStyle(attr);

	if (TextInput->HasSelection()) {

		bool flag = TextInput->IsSelectionItalics() ? true : false;

		for (long i = TextInput->GetSelectionRange().GetStart(); i < TextInput->GetSelectionRange().GetEnd(); i++) {
			wxRichTextAttr charAttr;

			if (TextInput->GetStyle(i, charAttr)) {
				charAttr.SetFontStyle(flag ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC);
				TextInput->SetStyle(i, i + 1, charAttr);
			}
		}
	}

	TextInput->SetFocus();
	italicButton->Refresh();

	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void MainFrame::OnUnderlineClick(wxCommandEvent& evt)
{
	if (TextInput->HasSelection()) {

		bool flag = TextInput->IsSelectionUnderlined() ? true : false;

		for (long i = TextInput->GetSelectionRange().GetStart(); i < TextInput->GetSelectionRange().GetEnd(); i++) {
			wxRichTextAttr charAttr;

			if (TextInput->GetStyle(i, charAttr)) {
				charAttr.SetFontUnderlined(!flag);
				TextInput->SetStyle(i, i + 1, charAttr);
			}
		}
	}

	Underline = !Underline;

	underlineButton->SetBackgroundColour(Underline ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());

	attr.SetFontUnderlined(Underline);
	TextInput->SetDefaultStyle(attr);

	TextInput->SetFocus();
	underlineButton->Refresh();

	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void MainFrame::OnLeftAlignClick(wxCommandEvent& evt)
{
	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	Raligntoggle = false;
	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	Caligntoggle = false;


	if (!Laligntoggle) {
		Laligntoggle = true;
		LeftAlignButton->SetBackgroundColour(wxColor(135, 135, 135));

		attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
		TextInput->SetDefaultStyle(attr);
		auto lastCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphStart();
		auto startCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphEnd();
		TextInput->WriteText(" ");
		auto endCaretPosition = TextInput->GetCaretPosition();


		for (long i = startCaretPosition + 1; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetAlignment(wxTEXT_ALIGNMENT_LEFT); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}

		TextInput->SetSelection(endCaretPosition, endCaretPosition + 1);
		TextInput->DeleteSelection();

		TextInput->SetCaretPosition(lastCaretPosition);
	}

	//boldButton->SetBackgroundColour(attr.GetFontWeight() == wxFONTWEIGHT_BOLD? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	//italicButton->SetBackgroundColour(attr.GetFontStyle() == wxFONTSTYLE_ITALIC? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	//boldButton->SetBackgroundColour(attr.HasFontUnderlined() ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());
	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();

	TextInput->SetFocus();
	TextInput->Refresh();
	TextInput->Update();

	ForcedChange = true;
	CallAfter(&MainFrame::UpdateTextInfo);
}

void MainFrame::OnCenterAlignClick(wxCommandEvent& evt)
{
	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	Laligntoggle = false;
	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	Raligntoggle = false;

	if (!Caligntoggle) {
		Caligntoggle = true;
		CenterAlignButton->SetBackgroundColour(wxColor(135, 135, 135));

		attr.SetAlignment(wxTEXT_ALIGNMENT_CENTER);
		TextInput->SetDefaultStyle(attr);
		auto lastCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphStart();
		auto startCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphEnd();
		TextInput->WriteText(" ");
		auto endCaretPosition = TextInput->GetCaretPosition();

		for (long i = startCaretPosition + 1; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTER); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}

		TextInput->SetSelection(endCaretPosition, endCaretPosition + 1);
		TextInput->DeleteSelection();

		TextInput->SetCaretPosition(lastCaretPosition);
	}

	//boldButton->SetBackgroundColour(attr.GetFontWeight() == wxFONTWEIGHT_BOLD ? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	//italicButton->SetBackgroundColour(attr.GetFontStyle() == wxFONTSTYLE_ITALIC ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	//boldButton->SetBackgroundColour(attr.HasFontUnderlined() ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());

	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();

	TextInput->SetFocus();
	TextInput->Refresh();
	TextInput->Update();

	ForcedChange = true;
	CallAfter(&MainFrame::UpdateTextInfo);
}

void MainFrame::OnRightAlignClick(wxCommandEvent& evt)
{
	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	Laligntoggle = false;
	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	Caligntoggle = false;

	if (!Raligntoggle) {
		Raligntoggle = true;
		RightAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
		
		attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
		TextInput->SetDefaultStyle(attr);
		auto lastCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphStart();
		auto startCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphEnd();
		TextInput->WriteText(" ");
		auto endCaretPosition = TextInput->GetCaretPosition();

		for (long i = startCaretPosition + 1; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}

		TextInput->SetSelection(endCaretPosition, endCaretPosition + 1);
		TextInput->DeleteSelection();

		TextInput->SetCaretPosition(lastCaretPosition);
	}

	//boldButton->SetBackgroundColour(attr.GetFontWeight() == wxFONTWEIGHT_BOLD ? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	//italicButton->SetBackgroundColour(attr.GetFontStyle() == wxFONTSTYLE_ITALIC ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	//boldButton->SetBackgroundColour(attr.HasFontUnderlined() ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());
	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();

	TextInput->SetFocus();
	TextInput->Refresh();
	TextInput->Update();

	ForcedChange = true;
	CallAfter(&MainFrame::UpdateTextInfo);
}



// text input
void MainFrame::TextInputShortcuts(wxKeyEvent& evt)
{

	//if (evt.GetKeyCode() == WXK_BACK || evt.GetKeyCode() == WXK_DELETE) {
	//	wxRichTextAttr defaultAttr = TextInput->GetBasicStyle();  // Store default style
	//	CallAfter([this, defaultAttr]() {
	//		long caretPos = TextInput->GetCaretPosition();
	//		if (caretPos > 0) {
	//			TextInput->SetStyle(caretPos, caretPos + 1, defaultAttr);  // Reset style at caret
	//		}
	//		TextInput->SetDefaultStyle(defaultAttr);  // Ensure new text follows default style
	//		});
	//}

	if (evt.ControlDown()) {

		wxCommandEvent dummyEvent;

		switch (evt.GetKeyCode())
		{
		case 'B':
			OnBoldClick(dummyEvent);
			break;
		case 'I':
			OnItalicClick(dummyEvent);
			break;
		case 'U':
			OnUnderlineClick(dummyEvent);
			break;
		case WXK_SPACE:
			ResetFormatting();
			break;
		case 'L':
			OnLeftAlignClick(dummyEvent);
			break;
		case 'R':
			OnRightAlignClick(dummyEvent);
			break;
		case 'E':
			OnCenterAlignClick(dummyEvent);
			break;
		default:
			break;
		}


		TextInput->SetDefaultStyle(attr);
	}

	if (evt.ControlDown() && evt.ShiftDown()) {
		switch (evt.GetKeyCode())
		{
		case '.':
			if (TextInput->HasSelection()) {

				auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
				auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();

				for (long i = startCaretPosition; i < endCaretPosition; ++i)
				{
					wxRichTextAttr charAttr;
					if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
						int newFontSize = charAttr.GetFontSize() + 1;
						charAttr.SetFontSize(newFontSize);
						TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
					}
				}
			}
			attr.SetFontSize(attr.GetFontSize() + 1);
			fontSize++;
			//FontSizeBox->SetValue(to_string(attr.GetFontSize()));
			TextInput->SetDefaultStyle(attr);
			UpdateTextInfo();
			break;
		case ',':
			if (TextInput->HasSelection()) {

				auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
				auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();


				for (long i = startCaretPosition; i < endCaretPosition; ++i)
				{
					wxRichTextAttr charAttr;
					if (TextInput->GetStyle(i, charAttr)) { // Get each character's style

						int newFontSize = charAttr. GetFontSize();
						if (newFontSize > 1) {
							newFontSize -= 1;
							charAttr.SetFontSize(newFontSize);
						}

						TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
					}
				}

			}


			if (attr.GetFontSize() > 1) {
				attr.SetFontSize(attr.GetFontSize() - 1);
				fontSize--;
			}
			//FontSizeBox->SetValue(to_string(attr.GetFontSize()));
			TextInput->SetDefaultStyle(attr);
			UpdateTextInfo();
			break;
		default:
			break;
		}
	}

	if (!evt.ControlDown() && !evt.ShiftDown()) {
		switch (evt.GetKeyCode()) {
			case WXK_LEFT:
				if (TextInput->HasSelection()) TextInput->SetCaretPosition(TextInput->GetSelectionRange().GetStart());
				CallAfter(&MainFrame::UpdateTextInfo);
				break;
			case WXK_RIGHT:
				if (TextInput->HasSelection()) TextInput->SetCaretPosition(TextInput->GetSelectionRange().GetEnd() - 2);
				CallAfter(&MainFrame::UpdateTextInfo);
				break;
			//case WXK_UP:
			//	CallAfter(&MainFrame::UpdateTextInfo);
			//	break;
			//case WXK_DOWN:
			//	CallAfter(&MainFrame::UpdateTextInfo);
			//	break;
			default:
				break;
		}
	}

	if(TextInput->HasSelection()) wxLogStatus("Selection: (%i, %i)", TextInput->GetSelectionRange().GetStart(), TextInput->GetSelectionRange().GetEnd());

	evt.Skip();
}

void MainFrame::OnCaretClick(wxMouseEvent& evt)
{
	CallAfter(&MainFrame::UpdateTextInfo);
	//UpdateTextInfo();
	evt.Skip();
}

void MainFrame::TextInputType(wxCommandEvent& evt)
{
	CallAfter(&MainFrame::UpdateTextInfo);
	evt.Skip();
}

void MainFrame::OnFontSizeSelect(wxCommandEvent& evt)
{
	auto cursorPos = TextInput->GetCaretPosition();
	//wxRichTextAttr style;
	//TextInput->GetStyle(cursorPos, style);
	//style.SetFontSize(atol(FontSizeBox->GetValue()));
	//TextInput->SetStyle(cursorPos, cursorPos , style);

	fontSize = atol(FontSizeBox->GetValue());

	//newFontSize = true;
	wxLogStatus("New Font Size triggered!");

	if (TextInput->HasSelection()) {

		auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
		auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();

		for (long i = startCaretPosition; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetFontSize(atol(FontSizeBox->GetValue()));
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}
	}
	//attr.SetFontSize(attr.GetFontSize() + 1);
	//FontSizeBox->SetValue(to_string(attr.GetFontSize()));

	attr.SetFontSize(fontSize);
	TextInput->SetDefaultStyle(attr);
	TextInput->SetCaretPosition(cursorPos);

	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void MainFrame::OnAddButtonClick(wxCommandEvent& evt)
{
	wxCommandEvent dummyEvent;

	AddNote(dummyEvent);
}



// drawing
void MainFrame::Draw(wxPaintEvent& evt) {
	wxPaintDC dc(notSidePanel);
	wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
	if (!gc) return;

	gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);

	for (const auto& stroke : strokes) {
		gc->SetPen(wxPen(stroke.color, stroke.size));
		gc->SetBrush(*wxRED_BRUSH);

		if (stroke.points.size() < 2) continue;

		wxGraphicsPath path = gc->CreatePath();
		path.MoveToPoint(stroke.points[0].x, stroke.points[0].y);

		for (size_t i = 1; i < stroke.points.size() - 1; i++) {
			wxPoint mid = (stroke.points[i] + stroke.points[i + 1]) / 2;
			path.AddQuadCurveToPoint(
				stroke.points[i].x, stroke.points[i].y, // Control point
				mid.x, mid.y                             // End point
			);
		}

		path.AddLineToPoint(stroke.points.back().x, stroke.points.back().y);
		gc->StrokePath(path);
	}

	delete gc;
}

void MainFrame::OnUpdateButtonClick(wxCommandEvent& evt)
{
	//wxLogStatus("On Update triggered");
	if (activeNote != nullptr) {
		activeNote->UpdateNote(TextInput, BackgroundColor);
	}

}



// brushes
void MainFrame::XSbrushEvent(wxCommandEvent& evt)
{
	XSbrushActive = true;
	SbrushActive = false;
	MbrushActive = false;
	LbrushActive = false;
	XLbrushActive = false;

	brushSize = 1;

	UpdateBrushes();
}

void MainFrame::SbrushEvent(wxCommandEvent& evt)
{
	XSbrushActive = false;
	SbrushActive = true;
	MbrushActive = false;
	LbrushActive = false;
	XLbrushActive = false;

	brushSize = 2;

	UpdateBrushes();
}

void MainFrame::MbrushEvent(wxCommandEvent& evt)
{
	XSbrushActive = false;
	SbrushActive = false;
	MbrushActive = true;
	LbrushActive = false;
	XLbrushActive = false;

	brushSize = 3;

	UpdateBrushes();
}

void MainFrame::LbrushEvent(wxCommandEvent& evt)
{
	XSbrushActive = false;
	SbrushActive = false;
	MbrushActive = false;
	LbrushActive = true;
	XLbrushActive = false;

	brushSize = 5;

	UpdateBrushes();
}

void MainFrame::XLbrushEvent(wxCommandEvent& evt)
{
	XSbrushActive = false;
	SbrushActive = false;
	MbrushActive = false;
	LbrushActive = false;
	XLbrushActive = true;

	brushSize = 10;

	UpdateBrushes();
}
