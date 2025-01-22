#pragma once
#include <wx/wx.h>

using namespace std;

struct Stroke {
	wxColor color;
	int size;
	vector<wxPoint> points;

	Stroke(wxColor COLOR, int SIZE) {
		color = COLOR;
		size = SIZE;
	}
};