#pragma once
#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/vidmode.h>
#include <queue>
#include <wx/wrapsizer.h>
#include <wx/scrolwin.h>
#include <wx/splitter.h>
#include "Note.h"
#include "Tool.h"
#include "Rectangle.h"
#include <wx/richtext/richtextctrl.h>


using namespace std;

struct MainFrame : public wxFrame
{
	void addToolBar(wxWindow* parent);
	void addSideBar(wxWindow* parent);
	void addColorPanel(wxWindow* parent);
	void addMenuBar();
	void switchTheme();
	void UpdateColors();
	void addScrolledPanel();
	void addEditPanel(wxWindow* parent);

	void SetActive(Note* activenote);
	void UpdateTextInfo();
	void UpdateToolInfo();

	bool isDark;
	bool isDrawingRect = false;
	bool isBrushActive = false;
	bool isEraseActive = false;
	int noteDefaultPositionX;
	int noteDefaultPositionY;

	bool Bold = false;
	bool Italic = false;
	bool Underline = false;

	bool Laligntoggle = true;
	bool Raligntoggle = false;
	bool Caligntoggle = false;

	bool newFontSize = false;

	int index;
	int grandChildren;
	int brushSize;
	bool hasGrandChildren;
	wxPoint startPos;
	wxPoint endPos;
	wxPoint bufferPoint;

	rectangle* drawnPanel;
	//wxPanel* drawnPanel;

	bool m_isPanning;
	wxPoint m_startScrollPos;
	wxPoint m_scrollOffset;

	vector<wxPanel*> rectangles;

	Tool* NoteTool;
	Tool* RectangleTool;
	Tool* ElipseTool;
	Tool* BrushTool;
	Tool* EraseTool;

	Note* activeNote = nullptr;
	rectangle* activeRectangle = nullptr;
	bool occupied = false;

	wxColor ForegroundColor;
	wxColor BackgroundColor;
	wxColor ColorBuffer;

	queue<wxWindow*> themeQueue1;
	queue<wxWindow*> themeQueue2;

	wxWrapSizer* colorWrapSizer;
	wxBoxSizer* editPanelSizer;

	wxSplitterWindow* sidePanelsSplitter;
	wxSplitterWindow* editPanelSplitter;

	wxPanel* notToolBarPanel;
	wxPanel* colorPanel;
	wxPanel* FcolorChoice;
	wxPanel* BcolorChoice;
	wxPanel* notSidePanel;
	wxPanel* editPanel;
	wxPanel* noteEditPanel;
	wxPanel* textColorPreview;
	wxPanel* sidePanel;
	wxScrolledWindow* scrollPanel;

	wxTextCtrl* textInput;
	wxButton* addButton;
	wxButton* updateButton;
	wxToggleButton* boldButton;
	wxToggleButton* italicButton;
	wxToggleButton* underlineButton;
	wxToggleButton* ToDoTab;
	wxToggleButton* NoteEditTab;
	wxBitmapButton* LeftAlignButton;
	wxBitmapButton* RightAlignButton;
	wxBitmapButton* CenterAlignButton;

	wxRichTextCtrl* TextInput;

	wxComboBox* FontSizeBox;

	wxFont H2Font;

	wxRichTextAttr attr;

	wxRadioBox* ColorTypeRadioBox;

	MainFrame(const wxString& title);
	vector<std::string> paletteColors = {
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
	vector<wxPanel*> paletteSwatches;
	vector<pair< pair<int, wxColor> , vector<wxPoint> >> strokes;

	void SwitchThemeButton(wxCommandEvent& evt);
	void OnSwatchHover(wxMouseEvent& evt);
	void OnSwatchSelect(wxMouseEvent& evt);
	void AddNewColor(wxCommandEvent& evt);
	void AddNote(wxCommandEvent& evt);
	void OnRightDown(wxMouseEvent& evt);
	void OnRightUp(wxMouseEvent& evt);
	void OnMouseMotion(wxMouseEvent& evt);
	void DrawRectangle(wxCommandEvent& evt);
	void ActivateBrushTool(wxCommandEvent& evt);
	void ActivateEraseTool(wxCommandEvent& evt);

	void OnLeftDown(wxMouseEvent& evt);
	void OnLeftUp(wxMouseEvent& evt);
	void OnBoldClick(wxCommandEvent& evt);
	void OnItalicClick(wxCommandEvent& evt);
	void OnUnderlineClick(wxCommandEvent& evt);
	void OnLeftAlignClick(wxCommandEvent& evt);
	void OnCenterAlignClick(wxCommandEvent& evt);
	void OnRightAlignClick(wxCommandEvent& evt);
	void TextInputShortcuts(wxKeyEvent& evt);
	void OnCaretClick(wxMouseEvent& evt);
	void TextInputType(wxCommandEvent& evt);
	void OnFontSizeSelect(wxCommandEvent& evt);
	void OnAddButtonClick(wxCommandEvent& evt);
	void Draw(wxPaintEvent& evt);
};