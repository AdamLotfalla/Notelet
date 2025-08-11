#pragma once
#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/vidmode.h>
#include <queue>
#include <deque>
#include <wx/wrapsizer.h>
#include <wx/scrolwin.h>
#include <wx/splitter.h>
#include "Note.h"
#include "Tool.h"
#include "Rectangle.h"
#include "Stroke.h"
#include "ToDoList.h"
#include "ImageBox.h"


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
	void UpdateBrushes();
	void ResetFormatting();
	bool XSbrushActive = false, SbrushActive = false, MbrushActive = false, LbrushActive = false, XLbrushActive = false, showBrushes = false;
	void SaveToFile(const wxString& filePath);
	void SaveToDoListsToFile(const wxString& filePaht);
	void LoadFromFile(const wxString& filePath);
	void LoadToDoListsToFile(const wxString& filePath);

	//void SetActive(Note* ACTIVENOTE);
	void UpdateToolInfo();

	bool isDark;
	bool isDrawingRect = false;
	bool isBrushActive = false;
	bool isEraseActive = false;
	//bool isImageActive = false;
	int noteDefaultPositionX;
	int noteDefaultPositionY;
	int todoCount = 0;
	int fontSize;

	int index;
	int grandChildren;
	int brushSize;
	bool hasGrandChildren;
	wxPoint startPos;
	wxPoint endPos;
	wxPoint bufferPoint;

	wxPanel* toolBarPanel;

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
	Tool* ToDoTool;
	Tool* ImageTool;

	Tool* XSbrush;
	Tool* Sbrush;
	Tool* Mbrush;
	Tool* Lbrush;
	Tool* XLbrush;

	Note* activeNote = nullptr;
	rectangle* activeRectangle = nullptr;
	wxCursor brushCursor = wxCursor("./Cursors/brush.cur", wxBITMAP_TYPE_CUR);
	wxCursor eraseCursor = wxCursor("./Cursors/Eraser.cur", wxBITMAP_TYPE_CUR);
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
	wxPanel* sidePanel;
	wxScrolledWindow* scrollPanel;

	wxFont H2Font;

	wxRadioBox* ColorTypeRadioBox;
	wxColor InputTextColor;


	wxString currentFilePath;

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
	vector<Stroke> strokes;
	deque<Note*> notes;
	deque<ToDoList*> todolists;
	deque<ImageBox*> images;

	void SwitchThemeButton(wxCommandEvent& evt);
	void OnSwatchHover(wxMouseEvent& evt);
	void OnSwatchSelect(wxMouseEvent& evt);
	void AddNewColor(wxCommandEvent& evt);
	void AddNote(wxCommandEvent& evt);
	void AddToDo(wxCommandEvent& evt);
	void OnRightDown(wxMouseEvent& evt);
	void OnRightUp(wxMouseEvent& evt);
	void OnMouseMotion(wxMouseEvent& evt);
	void DrawRectangle(wxCommandEvent& evt);
	void ActivateBrushTool(wxCommandEvent& evt);
	void ActivateEraseTool(wxCommandEvent& evt);

	void OnLeftDown(wxMouseEvent& evt);
	void OnLeftUp(wxMouseEvent& evt);
	void OnFontSizeSelect(wxCommandEvent& evt);
	void Draw(wxPaintEvent& evt);

	void Shortcuts(wxKeyEvent& evt);
	void ActivateImageTool(wxCommandEvent& evt);

	//brushes
	void XSbrushEvent(wxCommandEvent& evt);
	void SbrushEvent(wxCommandEvent& evt);
	void MbrushEvent(wxCommandEvent& evt);
	void LbrushEvent(wxCommandEvent& evt);
	void XLbrushEvent(wxCommandEvent& evt);

	// file
	void OnFileSaveAs(wxCommandEvent& evt);
	void OnFileSave(wxCommandEvent& evt);
	void OnFileExit(wxCommandEvent& evt);
	void OnFileOpen(wxCommandEvent& evt);

};