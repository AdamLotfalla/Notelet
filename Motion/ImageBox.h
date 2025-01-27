#pragma once
#include <wx/wx.h>
#include "Rectangle.h"
//#include "MainFrame.h"

struct MainFrame;

struct ImageBox : public rectangle
{
	ImageBox(wxWindow* parent, wxPoint POS, wxSize SIZE, wxImage IMAGE, MainFrame* frame);

	wxBitmap bitmap;
	wxImage image;
	//wxString path;
	wxStaticBitmap* display;
	wxBoxSizer* mainSizer;

	int margin = 15;
	double ratio;

	void OnKnobMotion(wxMouseEvent& evt);
};

