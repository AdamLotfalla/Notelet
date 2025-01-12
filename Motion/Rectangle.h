#pragma once
#include <wx/wx.h>

struct rectangle : public wxPanel
{
	rectangle(wxWindow* PARENT, wxPoint STARTPOS, wxSize defaultSize, int SIZE, wxColor COLOR);

	wxPoint startPos;
	wxPoint endPos;

	int size;
	wxColor color;

	bool drawing;

	void OnPaint(wxPaintEvent& evt);
};

