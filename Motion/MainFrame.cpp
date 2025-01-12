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
	addMenuBar();

	H2Font.SetPointSize(16);
	H2Font.MakeBold();
	//H2Font.SetFaceName("Arial Unicode MS");

	this->SetDoubleBuffered(true);

	isDark = !isDark;
	wxCommandEvent dummyEvent(wxEVT_BUTTON, wxID_ANY);
	SwitchThemeButton(dummyEvent);

	noteDefaultPositionX = scrollPanel->GetPosition().x;
	noteDefaultPositionY = scrollPanel->GetPosition().y;

	//temporary
	CreateStatusBar();

}


// adding layout
void MainFrame::addSideBar(wxWindow* parent)
{
	sidePanelsSplitter = new wxSplitterWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	wxPanel* sidePanel = new wxPanel(sidePanelsSplitter);
	addScrolledPanel();

	wxBoxSizer* sidePanelsSizer = new wxBoxSizer(wxHORIZONTAL);
	sidePanelsSizer->Add(sidePanelsSplitter, 1, wxEXPAND);

	sidePanelsSplitter->SplitVertically(sidePanel, scrollPanel, 200);
	sidePanelsSplitter->SetMinimumPaneSize(220);

	sidePanel->SetBackgroundColour(wxColor(100,100,200));
	
	parent->SetSizerAndFit(sidePanelsSizer);

	//edit panel
	wxSplitterWindow* editPanelSplitter = new wxSplitterWindow(sidePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

	colorPanel = new wxPanel(editPanelSplitter);
	wxPanel* editPanel = new wxPanel(editPanelSplitter);

	wxBoxSizer* editPanelSizer = new wxBoxSizer(wxHORIZONTAL);
	editPanelSizer->Add(editPanelSplitter, 1, wxEXPAND);

	editPanelSplitter->SplitHorizontally(colorPanel, editPanel, -300);
	editPanelSplitter->SetMinimumPaneSize(170);

	colorPanel->SetBackgroundColour(wxColor(200, 200, 100));
	editPanel->SetBackgroundColour(wxColor(100, 200, 200));

	sidePanel->SetSizerAndFit(editPanelSizer);
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
	RectangleTool->ChangeIcon(isDark ? "./RectangleDarkMode.ico" : "./RectangleWhiteMode.ico");
	ElipseTool->ChangeIcon(isDark ? "./ElipseDarkMode.ico" : "./ElipseWhiteMode.ico");
}




// note controls
void MainFrame::SetActive(Note* activenote) {
	if (activenote == nullptr) {
		wxLogStatus("Attempting to set active note to nullptr.");
		return;
	}
	activeNote = activenote;
	//noteEnterText->SetLabel(activeNote->text);
	//noteEnterText->SelectAll();
	wxLogStatus("Active ID: %i", activeNote->GetId());
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
		wxPanel* swatch = new wxPanel(colorPanel, wxID_ANY, wxDefaultPosition, wxSize(25, 25));
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
	wxPanel* toolBarPanel = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, 30));
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
	RectangleTool = new Tool(toolBarPanel, "Draw a rectangle \t Ctrl+R", isDark ? "./RectangleDarkMode.ico" : "./RectangleWhiteMode.ico");
	ElipseTool = new Tool(toolBarPanel, "Draw an Elipse \t Ctrl+E", isDark ? "./ElipseDarkMode.ico" : "./ElipseWhiteMode.ico");


	toolSizer->Add(NoteTool, 0, wxEXPAND);
	toolSizer->Add(RectangleTool, 0, wxEXPAND);
	toolSizer->Add(ElipseTool, 0, wxEXPAND);

	NoteTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::AddNote, this);
	RectangleTool->Bind(wxEVT_TOGGLEBUTTON, &MainFrame::DrawRectangle, this);

	toolBarPanel->SetSizerAndFit(toolSizer);
}

void MainFrame::AddNote(wxCommandEvent& evt)
{
	wxFont font;
	int offset = 200;

	noteDefaultPositionX = scrollPanel->GetScrollPos(wxHORIZONTAL) + offset;
	noteDefaultPositionY = scrollPanel->GetScrollPos(wxVERTICAL) + offset;

	Note* note = new Note(150, 150, 0, noteDefaultPositionX, noteDefaultPositionY, "Hello world!", font, ForegroundColor, BackgroundColor, notSidePanel, this);
}

void MainFrame::DrawRectangle(wxCommandEvent& evt)
{
	isDrawingRect = true;
	wxLogStatus("DrawingRectangle");
}



// tool gestures
void MainFrame::OnRightDown(wxMouseEvent& evt)
{
	notSidePanel->CaptureMouse();
	m_isPanning = true;
	endPos = wxGetMousePosition();
	m_scrollOffset = scrollPanel->GetViewStart(); // Get current scroll position in logical units
	SetCursor(wxCURSOR_HAND);
}

void MainFrame::OnLeftDown(wxMouseEvent& evt)
{
	startPos = evt.GetPosition();

	if (isDrawingRect) {
		notSidePanel->CaptureMouse();

		bufferPoint = startPos;

		drawnPanel = new rectangle(notSidePanel, startPos, wxSize(10, 10), 5, *wxRED);
		//drawnPanel = new wxPanel(notSidePanel, wxID_ANY, startPos, wxSize(10, 10));
		//drawnPanel->SetBackgroundColour(*wxGREEN);
		drawnPanel->Raise();
		drawnPanel->SetTransparent(true);
		drawnPanel->SetDoubleBuffered(true);
		//drawnPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
		//rectangles.push_back(drawnPanel);

		wxLogStatus("LeftDown, rectangle drawn at (%i,%i)", drawnPanel->GetPosition().x, drawnPanel->GetPosition().y);
	}
}

void MainFrame::OnRightUp(wxMouseEvent& evt) {
	notSidePanel->ReleaseMouse();
	m_isPanning = false;
	SetCursor(wxCURSOR_DEFAULT);
}

void MainFrame::OnLeftUp(wxMouseEvent& evt)
{
	if (isDrawingRect) { notSidePanel->ReleaseMouse(); }
	isDrawingRect = false;
	drawnPanel->SetTransparent(false);
	wxLogStatus("LeftUp");
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

		activeNote = nullptr;

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

		Refresh();
	}

	evt.Skip();
}
