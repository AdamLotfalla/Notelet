#pragma once
#include <wx/wx.h>

struct MainFrame;

struct ColorDialog : public wxDialog
{
	ColorDialog(wxWindow* parent, wxString title, MainFrame* frame);

	wxChoice* colorFormatChoice;

	wxTextCtrl* RH;
	wxTextCtrl* GS;
	wxTextCtrl* BL;
	wxTextCtrl* Hex;

	wxPanel* colorPreview;

	wxButton* addButton;
	wxButton* cancelButton;

	wxBoxSizer* verticalSizer;
	wxBoxSizer* horizontalSizer;
	wxBoxSizer* buttonHorizontalSizer;
	wxGridSizer* outerSizer;

	wxColor color;

	MainFrame* mainframe = nullptr;

	void UpdateVisibility();
	void ChooseFormat(wxCommandEvent& evt);
	void WriteBoxes(wxCommandEvent& evt);
	void AddButton(wxCommandEvent& evt);
	void CancelButton(wxCommandEvent& evt);
	void Shortcut(wxKeyEvent& evt);
};

