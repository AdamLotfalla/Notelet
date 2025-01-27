#pragma once
#include <wx/wx.h>

class MainFrame;

struct rectangle : public wxWindow
{
	rectangle(wxWindow* PARENT, wxPoint STARTPOS, wxSize defaultSize, int SIZE, wxColor FCOLOR, wxColor BCOLOR, MainFrame* FRAME);

	wxPoint startPos;
	wxPoint endPos;
	wxPoint bufferPoint;
	wxPoint offset;
	wxSize initialSize;
	wxSize minSize = wxSize(25,25);
	wxWindow* parent;

	int size;
	int sizeX, sizeY;
	wxColor fcolor;
	wxColor bcolor;

	MainFrame* mainframe = nullptr;

	wxPanel* sizerKnob;

	bool drawing;
	bool active;

	void OnPanelPaint(wxPaintEvent& evt);
	void OnKnobPaint(wxPaintEvent& evt);
	void OnPanelLeftDown(wxMouseEvent& evt);
	void OnPanelLeftUp(wxMouseEvent& evt);
	void OnPanelHover(wxMouseEvent& evt);
	void OnKnobLeftDown(wxMouseEvent& evt);
	void OnKnobLeftUp(wxMouseEvent& evt);
	void OnPanelMotion(wxMouseEvent& evt);
	void OnKnobMotion(wxMouseEvent& evt);
	void OnKnobHover(wxMouseEvent& evt);
	void ShortCuts(wxKeyEvent& evt);
};

