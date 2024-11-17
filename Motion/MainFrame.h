#pragma once
#include <wx/wx.h>
#include "Note.h"
#include <vector>



class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
	wxPanel* ColorPtr;
	std::vector<wxPanel*> paletes;
	wxColor NoteBColor;
	wxColor NoteFColor;
	wxColor NoteColorBuffer;
	wxRadioBox* F_BColorCheck;
	wxArrayString ColorChoices;
	wxPanel* fColorChoice;
	wxPanel* bColorChoice;
	wxFont font;
private:
	wxTextCtrl* noteEnterText;
	wxPanel* panel;
	
	// Methods
	void declareObjects(wxWindow* parent);
	void addSizers();

	// Events
	void OnNoteButtonClicked(wxCommandEvent& evt);
	void OnUpdate(wxCommandEvent& evt);
	void OnColorChoose(wxMouseEvent& evt);
	void OnColorHover(wxMouseEvent& evt);
	void OnBoldSelect(wxCommandEvent& evt);
	void OnItalicSelect(wxCommandEvent& evt);
	void OnUnderlinedSelect(wxCommandEvent& evt);
};

