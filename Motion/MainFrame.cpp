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
#include <wx/xml/xml.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <wx/base64.h>
#include "ImageBox.h"

using namespace std;

#define TextColor isDark ? "#FFFFFF" : "#3d3d3d"
#define PanelColor1 isDark ? "#424242" : "#F3F6FE" //F9F6EE
#define PanelColor2 isDark ? "#4f4f4f" : "#FDFDFD"

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {

	isDark = true;
	ForegroundColor = "#2f2f2f";
	BackgroundColor = "#FFDB58";
	SetIcon(wxIcon("./Icon.ico", wxBITMAP_TYPE_ICO));

	addToolBar(this);
	addSideBar(notToolBarPanel);
	addColorPanel(colorPanel);
	addEditPanel(editPanel);
	addMenuBar();

	H2Font.SetPointSize(16);
	H2Font.MakeBold();
	//H2Font.SetFaceName("Arial Unicode MS");

	this->SetDoubleBuffered(true);
	this->Bind(wxEVT_KEY_DOWN, &MainFrame::Shortcuts, this);

	TextInput->Newline();
	TextInput->WriteText("Starting...");

	showBrushes = false;
	MbrushActive = true;
	brushSize = 3;
	fontSize = 9;
	InputTextColor = wxColor(paletteColors[20]);
	UpdateBrushes();
	ResetFormatting();
	//CreateStatusBar();
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


	fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save the notes");
	fileMenu->Append(wxID_SAVEAS, "Save &As...", "Save the notes to a specific file");
	fileMenu->Append(wxID_OPEN, "&Open\tCtrl+O", "Open a file to load notes");
	fileMenu->AppendSeparator();
	fileMenu->Append(wxID_EXIT, "E&xit", "Exit the application");

	addMenu->Append(wxID_NEW, "&Add Note\tCtrl+N", "Add a new note");
	addMenu->Append(5, "&Add To-Do List \tCtrl+T", "Add a new To-Do list");
	addMenu->Append(6, "&Add an image \tCtrl+I", "Import a new image");

	settingsMenu->Append(55, "Switch Dark/Light", "Switch between dark and light modes");

	menuBar->Append(fileMenu, "&File");
	menuBar->Append(addMenu, "&Add");
	menuBar->Append(settingsMenu, "&Settings");

	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, &MainFrame::SwitchThemeButton, this, 55);
	Bind(wxEVT_MENU, &MainFrame::AddNote, this, wxID_NEW);
	Bind(wxEVT_MENU, &MainFrame::AddToDo, this, 5);
	Bind(wxEVT_MENU, &MainFrame::ActivateImageTool, this, 6);
	Bind(wxEVT_MENU, &MainFrame::OnFileSaveAs, this, wxID_SAVEAS);
	Bind(wxEVT_MENU, &MainFrame::OnFileSave, this, wxID_SAVE);
	Bind(wxEVT_MENU, &MainFrame::OnFileOpen, this, wxID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::OnFileExit, this, wxID_EXIT);
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
	boldButton->SetFont(boldButton->GetFont().MakeBold());

	italicButton = new wxToggleButton(parent, wxID_ANY, "I", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	italicButton->SetToolTip("Make Italic");
	italicButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnItalicClick, this);
	italicButton->SetFont(italicButton->GetFont().MakeItalic());

	underlineButton = new wxToggleButton(parent, wxID_ANY, "U", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	underlineButton->SetToolTip("Make Underlined");
	underlineButton->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::OnUnderlineClick, this);
	underlineButton->SetFont(underlineButton->GetFont().MakeUnderlined());

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
	//RectangleTool->ChangeIcon(isDark ? "./RectangleDarkMode.ico" : "./RectangleWhiteMode.ico");
	//ElipseTool->ChangeIcon(isDark ? "./ElipseDarkMode.ico" : "./ElipseWhiteMode.ico");
	BrushTool->ChangeIcon(isDark ? "./BrushDarkMode.ico" : "./BrushWhiteMode.ico");
	EraseTool->ChangeIcon(isDark ? "./EraseDarkMode.ico" : "./EraseWhiteMode.ico");
	ImageTool->ChangeIcon(isDark ? "./ImageDarkMode.ico" : "./ImageWhiteMode.ico");

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
	//RectangleTool->SetBackgroundColour(isDrawingRect ? wxColor(135, 135, 135) : RectangleTool->GetParent()->GetBackgroundColour());
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
	wxInitAllImageHandlers();


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
	//RectangleTool = new Tool(toolBarPanel, "Draw a rectangle \t Ctrl+R", isDark ? "./RectangleDarkMode.ico" : "./RectangleWhiteMode.ico");
	//ElipseTool = new Tool(toolBarPanel, "Draw an Elipse \t Ctrl+E", isDark ? "./ElipseDarkMode.ico" : "./ElipseWhiteMode.ico");
	BrushTool = new Tool(toolBarPanel, "Draw using the Brush \t Ctrl+B", isDark ? "./BrushDarkMode.ico" : "./BrushWhiteMode.ico");
	EraseTool = new Tool(toolBarPanel, "Erase strokes \t Ctrl+E", isDark ? "./EraseDarkMode.ico" : "./EraseWhiteMode.ico");
	ImageTool = new Tool(toolBarPanel, "Add an image \t Ctrl+I", isDark ? "./ImageDarkMode.ico" : "./ImageWhiteMode.ico");

	XSbrush = new Tool(toolBarPanel, "1px brush nib", isDark ? "./XSbrushDarkMode.ico" : "./XSbrushWhiteMode.ico");
	Sbrush = new Tool(toolBarPanel, "2px brush nib", isDark ? "./SbrushDarkMode.ico" : "./SbrushWhiteMode.ico");
	Mbrush = new Tool(toolBarPanel, "3px brush nib", isDark ? "./MbrushDarkMode.ico" : "./MbrushWhiteMode.ico");
	Lbrush = new Tool(toolBarPanel, "5px brush nib", isDark ? "./LbrushDarkMode.ico" : "./LbrushWhiteMode.ico");
	XLbrush = new Tool(toolBarPanel, "10px brush nib", isDark ? "./XLbrushDarkMode.ico" : "./XLbrushWhiteMode.ico");


	toolSizer->Add(NoteTool, 0, wxEXPAND);
	toolSizer->Add(ToDoTool, 0, wxEXPAND);
	//toolSizer->Add(RectangleTool, 0, wxEXPAND);
	//toolSizer->Add(ElipseTool, 0, wxEXPAND);
	toolSizer->Add(BrushTool, 0, wxEXPAND);
	toolSizer->Add(EraseTool, 0, wxEXPAND);
	toolSizer->Add(ImageTool, 0, wxEXPAND);
	toolSizer->AddStretchSpacer();
	toolSizer->Add(XSbrush, 0, wxEXPAND);
	toolSizer->Add(Sbrush, 0, wxEXPAND);
	toolSizer->Add(Mbrush, 0, wxEXPAND);
	toolSizer->Add(Lbrush, 0, wxEXPAND);
	toolSizer->Add(XLbrush, 0, wxEXPAND);
	toolSizer->AddSpacer(50);



	NoteTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::AddNote, this);
	ToDoTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::AddToDo, this);
	//RectangleTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::DrawRectangle, this);
	BrushTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::ActivateBrushTool, this);
	EraseTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::ActivateEraseTool, this);
	ImageTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::ActivateImageTool, this);
	

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
	notes.push_back(note);

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
	todolists.push_back(todo);

	todo->Raise();

	isDrawingRect = false;
	isBrushActive = false;
	isEraseActive = false;
	showBrushes = false;


	UpdateBrushes();
	UpdateToolInfo();
}

//void MainFrame::DrawRectangle(wxCommandEvent& evt)
//{
//	isDrawingRect = !isDrawingRect;
//	isBrushActive = false;
//	isEraseActive = false;
//	showBrushes = false;
//
//
//	wxLogStatus("DrawingRectangle");
//	SetCursor(wxCURSOR_CROSS);
//
//
//	UpdateBrushes();
//	UpdateToolInfo();
//}

void MainFrame::ActivateBrushTool(wxCommandEvent& evt)
{
	isBrushActive = !isBrushActive;
	if (isBrushActive) {
		SetCursor(brushCursor);
	}
	else {
		SetCursor(wxCURSOR_DEFAULT);
	}
	isDrawingRect = false;
	isEraseActive = false;
	//isImageActive = false;
	showBrushes = true;
	
	UpdateBrushes();
	UpdateToolInfo();
}

void MainFrame::ActivateEraseTool(wxCommandEvent& evt)
{
	isEraseActive = !isEraseActive;
	if (isEraseActive) {
		SetCursor(eraseCursor);
	}
	else {
		SetCursor(wxCURSOR_DEFAULT);
	}
	isDrawingRect = false;
	isBrushActive = false;
	//isImageActive = false;
	showBrushes = true;


	UpdateBrushes();
	UpdateToolInfo();
	Refresh();
}

void MainFrame::ActivateImageTool(wxCommandEvent& evt)
{
	//isImageActive = !isImageActive;
	isDrawingRect = false;
	isBrushActive = false;
	isEraseActive = false;
	showBrushes = false;

	wxFileDialog openFileDialog(
		this,
		_("Open Image File"),
		"",
		"",
		"JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG files (*.png)|*.png|All Files (*.*)|*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);

	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		wxLogStatus("File open canceled.");
		return; // User canceled, so exit the function.
	}
	wxString filePath = openFileDialog.GetPath();


	int offset = 200;

	noteDefaultPositionX = scrollPanel->GetScrollPos(wxHORIZONTAL) + offset;
	noteDefaultPositionY = scrollPanel->GetScrollPos(wxVERTICAL) + offset;
	
	ImageBox* imagebox = new ImageBox(notSidePanel, wxPoint(noteDefaultPositionX, noteDefaultPositionY), wxSize(-1, 200), wxImage(filePath), this);
	images.push_back(imagebox);

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
		SetCursor(wxCURSOR_DEFAULT);
	}

	if (isBrushActive) {
		if (notSidePanel->HasCapture()) {
			notSidePanel->ReleaseMouse();
		}

		int LastStrokeIndex = strokes.size();
		if (strokes.back().points.size() == 1) {
			strokes.pop_back();
		}

		wxLogStatus("Refreshed, number of circles: %i", int(strokes[LastStrokeIndex - 1].points.size()));
	}

	isDrawingRect = false;
	notSidePanel->Refresh();
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

void MainFrame::Shortcuts(wxKeyEvent& evt)
{
	wxCommandEvent dummyEvent;

	if (evt.ControlDown()) {
		switch (evt.GetKeyCode())
		{
		//case 'R':
		//	DrawRectangle(dummyEvent);
		//	break;
		case 'B':
			ActivateBrushTool(dummyEvent);
			break;
		case 'E':
			ActivateEraseTool(dummyEvent);
			break;
		case '1':
			if(showBrushes) XSbrushEvent(dummyEvent);
			break;
		case '2':
			if (showBrushes) SbrushEvent(dummyEvent);
			break;
		case '3':
			if (showBrushes) MbrushEvent(dummyEvent);
			break;
		case '4':
			if (showBrushes) LbrushEvent(dummyEvent);
			break;
		case '5':
			if (showBrushes) XLbrushEvent(dummyEvent);
			break;
		default:
			break;
		}

	}
	wxLogStatus("Key pressed: %d", evt.GetKeyCode());

	evt.Skip();
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



// saving and loading
void MainFrame::OnFileSaveAs(wxCommandEvent& evt) {
	wxFileDialog saveFileDialog(this, "Save Notes", "", "", "XML files (*.xml)|*.xml", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) return;

	currentFilePath = saveFileDialog.GetPath();

	// Ensure file path has .xml extension
	if (!currentFilePath.Lower().EndsWith(".xml")) {
		currentFilePath += ".xml";
	}

	SaveToFile(currentFilePath);
	this->SetTitle(wxFileName(currentFilePath).GetName());

}

void MainFrame::OnFileSave(wxCommandEvent& evt) {
	if (currentFilePath.IsEmpty()) {
		OnFileSaveAs(evt);
	}
	else {
		SaveToFile(currentFilePath);
	}
}

void MainFrame::SaveToFile(const wxString& filePath) {
	wxXmlDocument xmlDoc;
	wxXmlNode* root = new wxXmlNode(wxXML_ELEMENT_NODE, "Notes");
	xmlDoc.SetRoot(root);

	wxXmlNode* defaultPath = new wxXmlNode(wxXML_ELEMENT_NODE, "Path");
	defaultPath->AddAttribute("Path", currentFilePath);

	root->AddChild(defaultPath);

	for (const auto& note : notes) {
		wxXmlNode* noteNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Note");

		// Store position
		noteNode->AddAttribute("x", to_string(note->GetPosition().x));
		noteNode->AddAttribute("y", to_string(note->GetPosition().y));

		noteNode->AddAttribute("width", to_string(note->GetSize().GetWidth()));
		noteNode->AddAttribute("height", to_string(note->GetSize().GetHeight()));

		// Store background color
		noteNode->AddAttribute("background", note->bcolor.GetAsString(wxC2S_HTML_SYNTAX));

		// Store text content and formatting
		wxRichTextBuffer& buffer = note->textContent->GetBuffer();
		wxRichTextParagraphLayoutBox* container = &buffer;

		for (wxRichTextObjectList::compatibility_iterator node = container->GetChildren().GetFirst();
			node; node = node->GetNext()) {

			wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
			if (!para) continue;

			wxXmlNode* paraNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Paragraph");
			noteNode->AddChild(paraNode);

			for (wxRichTextObjectList::compatibility_iterator textNode = para->GetChildren().GetFirst();
				textNode; textNode = textNode->GetNext()) {

				wxRichTextPlainText* text = wxDynamicCast(textNode->GetData(), wxRichTextPlainText);
				if (text) {
					wxXmlNode* textXmlNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Text");
					textXmlNode->AddAttribute("value", text->GetText());

					wxRichTextAttr attr = text->GetAttributes();

					// Store formatting attributes
					if (attr.GetFontWeight() == wxFONTWEIGHT_BOLD)
						textXmlNode->AddAttribute("bold", "1");
					if (attr.GetFontStyle() == wxFONTSTYLE_ITALIC)
						textXmlNode->AddAttribute("italic", "1");
					if (attr.GetFontUnderlined())
						textXmlNode->AddAttribute("underline", "1");

					switch (attr.GetAlignment())
					{
					case wxTEXT_ALIGNMENT_LEFT:
						textXmlNode->AddAttribute("align", "left");
						break;
					case wxTEXT_ALIGNMENT_CENTER:
						textXmlNode->AddAttribute("align", "center");
						break;
					case wxTEXT_ALIGNMENT_RIGHT:
						textXmlNode->AddAttribute("align", "right");
						break;
					default:
						break;
					}

					if (attr.HasTextColour())
						textXmlNode->AddAttribute("color", attr.GetTextColour().GetAsString(wxC2S_HTML_SYNTAX));

					if (attr.HasFontPointSize())
						textXmlNode->AddAttribute("fontSize", wxString::Format("%d", attr.GetFontSize()));

					paraNode->AddChild(textXmlNode);
				}
			}
		}

		root->AddChild(noteNode);
	}

	for (const auto& todo : todolists) {
		wxXmlNode* todoNode = new wxXmlNode(wxXML_ELEMENT_NODE, "ToDo");

		todoNode->AddAttribute("Title", todo->headlineText->GetLabel());

		todoNode->AddAttribute("x", to_string(todo->GetPosition().x));
		todoNode->AddAttribute("y", to_string(todo->GetPosition().y));

		todoNode->AddAttribute("width", to_string(todo->GetSize().GetWidth()));
		todoNode->AddAttribute("height", to_string(todo->GetSize().GetHeight()));

		for (int i = 0; i < todo->checkListBox->GetCount(); i++) {
			wxXmlNode* taskNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Task");
			taskNode->AddAttribute("Description", todo->checkListBox->GetString(i));
			taskNode->AddAttribute("State", wxString::Format("%i", todo->checkListBox->IsChecked(i)));

			todoNode->AddChild(taskNode);
		}

		root->AddChild(todoNode);
	}

	for (const auto& stroke : strokes) {
		wxXmlNode* strokeNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Stroke");

		strokeNode->AddAttribute("Color", stroke.color.GetAsString());
		strokeNode->AddAttribute("Size", to_string(stroke.size));

		for (auto point : stroke.points) {
			wxXmlNode* pointNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Point");

			pointNode->AddAttribute("x", to_string(point.x));
			pointNode->AddAttribute("y", to_string(point.y));

			strokeNode->AddChild(pointNode);
		}

		root->AddChild(strokeNode);
	}

	for (auto imagebox : images) {
		wxMemoryOutputStream memStream;
		(imagebox->image).SaveFile(memStream, wxBITMAP_TYPE_PNG);

		// Convert memory buffer to Base64
		size_t size = memStream.GetLength();
		wxMemoryBuffer buffer(size);
		memStream.CopyTo(buffer.GetData(), size);
		wxString base64Data = wxBase64Encode((const unsigned char*)buffer.GetData(), size);

		// Create XML structure
		wxXmlNode* imageNode = new wxXmlNode(wxXML_ELEMENT_NODE, "Bitmap");
		imageNode->AddAttribute("x", to_string(imagebox->GetPosition().x));
		imageNode->AddAttribute("y", to_string(imagebox->GetPosition().y));
		imageNode->AddAttribute("width", to_string(imagebox->GetSize().GetWidth()));
		imageNode->AddAttribute("height", to_string(imagebox->GetSize().GetHeight()));
		imageNode->AddAttribute("imageWidth", to_string(imagebox->bitmap.GetSize().GetWidth()));
		imageNode->AddAttribute("imageHeight", to_string(imagebox->bitmap.GetSize().GetHeight()));


		imageNode->AddAttribute("data", base64Data);

		root->AddChild(imageNode);
	}


	if (!xmlDoc.Save(filePath)) {
		wxLogError("Unable to save the file!");
		wxLogError("Unable to save the file!");
	}
	else {
		wxLogStatus("File saved successfully!");
		wxLogMessage("File saved successfully!");
	}
}

void MainFrame::LoadFromFile(const wxString& filePath) {

	bool newWindow = false;
	MainFrame* newFrame = new MainFrame("Notelet");

	if (notes.size() > 0 || todolists.size() > 0 || strokes.size() > 0) {
		newWindow = true;
		newFrame->Show();
		newFrame->SetClientSize(1000, 600);
		//newFrame->Center();
	}
	else {
		newFrame->Destroy();
	}


	wxXmlDocument xmlDoc;
	if (!xmlDoc.Load(filePath)) {
		wxLogError("Unable to load the file!");
		return;
	}

	wxXmlNode* root = xmlDoc.GetRoot();

	wxXmlNode* child = root->GetChildren();
	while (child) {

		if (child->GetName() == "Path") {
			(newWindow ? newFrame : this)->currentFilePath = child->GetAttribute("Path");
		}
		else if (child->GetName() == "Note") {
			// Extract attributes
			int x, y, width, height;
			wxString bgColor;
			child->GetAttribute("x").ToLong((long*)&x);
			child->GetAttribute("y").ToLong((long*)&y);
			child->GetAttribute("width").ToLong((long*)&width);
			child->GetAttribute("height").ToLong((long*)&height);
			bgColor = child->GetAttribute("background");

			// Create an empty text control
			wxRichTextCtrl* emptyTextCtrl = new wxRichTextCtrl((newWindow ? newFrame : this)->notSidePanel, wxID_ANY, "", wxDefaultPosition, wxSize(width, height));

			// Create a new Note
			Note* newNote = new Note(width, height, 0, x, y, emptyTextCtrl, *wxBLACK, bgColor, (newWindow ? newFrame : this)->notSidePanel, newWindow? newFrame : this);
			newNote->bcolor.Set(bgColor);

			wxXmlNode* paraNode = child->GetChildren();
			while (paraNode) {
				if (paraNode->GetName() == "Paragraph") {
					wxXmlNode* textNode = paraNode->GetChildren();
					while (textNode) {
						if (textNode->GetName() == "Text") {
							wxString textValue = textNode->GetAttribute("value");

							wxRichTextAttr attr;
							bool bold = textNode->GetAttribute("bold") == "1";
							bool italic = textNode->GetAttribute("italic") == "1";
							bool underline = textNode->GetAttribute("underline") == "1";
							wxString align = textNode->GetAttribute("align");

							if (textNode->HasAttribute("color"))
								attr.SetTextColour(wxColour(textNode->GetAttribute("color")));

							if (textNode->HasAttribute("fontSize")) {
								long fontSize;
								textNode->GetAttribute("fontSize").ToLong(&fontSize);
								attr.SetFontSize(fontSize);
							}

							// Set alignment if provided
							if (align == "center") {
								attr.SetAlignment(wxTEXT_ALIGNMENT_CENTER);
							}
							else if (align == "right") {
								attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
							}
							else if (align == "left") {
								attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);  // Default to left alignment
							}

							emptyTextCtrl->SetDefaultStyle(attr);

							// Start formatting
							if (bold) emptyTextCtrl->BeginBold();
							if (italic) emptyTextCtrl->BeginItalic();
							if (underline) emptyTextCtrl->BeginUnderline();

							emptyTextCtrl->WriteText(textValue);

							// End formatting
							if (bold) emptyTextCtrl->EndBold();
							if (italic) emptyTextCtrl->EndItalic();
							if (underline) emptyTextCtrl->EndUnderline();
						}
						textNode = textNode->GetNext();
					}
					emptyTextCtrl->WriteText("\n"); // Ensure newline for each paragraph
				}
				paraNode = paraNode->GetNext();
			}

			newNote->UpdateNote(emptyTextCtrl, bgColor);
			(newWindow ? newFrame : this)->notes.push_back(newNote);
		}
		else if (child->GetName() == "ToDo") {
			int x, y, width, height;
			wxString title;

			title = child->GetAttribute("Title");
			child->GetAttribute("x").ToLong((long*)&x);
			child->GetAttribute("y").ToLong((long*)&y);
			child->GetAttribute("width").ToLong((long*)&width);
			child->GetAttribute("height").ToLong((long*)&height);

			ToDoList* todo = new ToDoList(x, y, width, height, (newWindow ? newFrame : this)->notSidePanel, title, newWindow ? newFrame : this);

			wxXmlNode* tasks = child->GetChildren();

			while (tasks) {

				wxString Description;
				bool State;

				Description = tasks->GetAttribute("Description");
				State = atol(tasks->GetAttribute("State"));

				todo->checkListBox->Insert(Description, todo->checkListBox->GetCount());
				todo->checkListBox->Check(todo->checkListBox->GetCount() - 1, State);


				tasks = tasks->GetNext();
			}

			(newWindow ? newFrame : this)->todolists.push_back(todo);
		}
		else if (child->GetName() == "Stroke") {
			wxColor color;
			int size;

			color = wxColor(child->GetAttribute("Color"));
			size = atol(child->GetAttribute("Size"));

			Stroke stroke = Stroke(color, size);

			wxXmlNode* points = child->GetChildren();

			while (points) {

				int x, y;

				x = atol(points->GetAttribute("x"));
				y = atol(points->GetAttribute("y"));

				stroke.points.push_back(wxPoint(x, y));

				points = points->GetNext();
			}

			(newWindow ? newFrame : this)->strokes.push_back(stroke);
		}
		else if (child->GetName() == "Bitmap") {
			// Read Base64 string
			wxString base64Data = child->GetAttribute("data", "");

			// Decode Base64 to memory buffer
			wxMemoryBuffer buffer = wxBase64Decode(base64Data);
			wxMemoryInputStream memStream(buffer.GetData(), buffer.GetDataLen());

			// Load the image from memory
			wxImage image;
			image.LoadFile(memStream, wxBITMAP_TYPE_PNG);

			int x, y;
			int width, height;
			int imageWidht, imageHeight;

			x = atol(child->GetAttribute("x"));
			y = atol(child->GetAttribute("y"));

			width = atol(child->GetAttribute("width"));
			height = atol(child->GetAttribute("height"));

			imageWidht = atol(child->GetAttribute("imageWidth"));
			imageHeight = atol(child->GetAttribute("imageHeight"));

			ImageBox* imagebox = new ImageBox((newWindow ? newFrame : this)->notSidePanel, wxPoint(x, y), wxSize(width, height), image, (newWindow ? newFrame : this));
			imagebox->bitmap.Rescale(imagebox->bitmap, wxSize(imageWidht, imageHeight));
			imagebox->display->SetBitmap(imagebox->bitmap);
			imagebox->SetSize(width, height);
			imagebox->sizerKnob->SetPosition(wxPoint(imagebox->GetSize().x - 15, imagebox->GetSize().y - 15));
			(newWindow ? newFrame : this)->images.push_back(imagebox);
		}

		child = child->GetNext();
	}

	(newWindow ? newFrame : this)->SetTitle(wxFileName(filePath).GetName());
	(newWindow ? newFrame : this)->Refresh(); // Refresh UI to show loaded notes
	wxLogStatus("File loaded successfully!");
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
	LoadFromFile(filePath);
}

void MainFrame::OnFileExit(wxCommandEvent& evt) {
	Close(true);
}