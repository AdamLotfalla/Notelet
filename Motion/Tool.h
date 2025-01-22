#pragma once
#include <wx/wx.h>
#include <wx/tglbtn.h>


struct Tool : wxBitmapToggleButton
{
	wxBitmap Bitmap;
	Tool(wxWindow* parent, wxString TOOLTIP, wxString ICONPATH) 
		: wxBitmapToggleButton(parent, wxID_ANY, wxBitmap(wxIcon(ICONPATH, wxBITMAP_TYPE_ICO)), wxDefaultPosition, wxSize(30, 30), wxNO_BORDER) {

		this->SetToolTip(TOOLTIP);
		wxIcon Icon(ICONPATH, wxBITMAP_TYPE_ICO);
		Bitmap = wxBitmap(Icon);
		//Bitmap.Rescale(Bitmap, wxSize(24, 24));
		//this->SetBitmap(Bitmap);
		this->SetBackgroundColour(parent->GetBackgroundColour());
		this->Bind(wxEVT_ENTER_WINDOW, &Tool::OnHover, this);


		Refresh();
	}

	void OnHover(wxMouseEvent& evt) {
		SetCursor(wxCURSOR_OPEN_HAND);
	}

	void ChangeIcon(wxString NEWPATH) {
		wxIcon Icon(NEWPATH, wxBITMAP_TYPE_ICO);
		Bitmap = wxBitmap(Icon);

		this->SetBitmap(Bitmap);
	}

};

