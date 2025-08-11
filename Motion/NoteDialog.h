#pragma once
#include <wx/wx.h>
#include <wx/tglbtn.h>
#include <wx/richtext/richtextctrl.h>
#include <string>

struct MainFrame;

using namespace std;


struct NoteDialog : public wxDialog
{
	NoteDialog(wxWindow* parent, wxString title, bool isDark, MainFrame* frame);

	MainFrame* mainframe = nullptr;

	wxTextCtrl* textInput;
	wxButton* updateButton;
	wxButton* cancelButton;
	wxToggleButton* boldButton;
	wxToggleButton* italicButton;
	wxToggleButton* underlineButton;
	wxToggleButton* ToDoTab;
	wxToggleButton* NoteEditTab;
	wxBitmapButton* LeftAlignButton;
	wxBitmapButton* RightAlignButton;
	wxBitmapButton* CenterAlignButton;
	wxPanel* textColorPreview;
	wxColor InputTextColor;
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

	void ResetFormatting();


	wxPanel* noteEditPanel;


	wxRichTextCtrl* TextInput;

	wxComboBox* FontSizeBox;

	bool Bold = false;
	bool Italic = false;
	bool Underline = false;

	bool Laligntoggle = true;
	bool Raligntoggle = false;
	bool Caligntoggle = false;

	bool ForcedChange = false;

	int fontSize;




	wxRichTextAttr attr;
	void OnUpdateButtonClick(wxCommandEvent& evt);
	void OnCancelButtonClick(wxCommandEvent& evt);
	void OnBoldClick(wxCommandEvent& evt);
	void OnItalicClick(wxCommandEvent& evt);
	void OnUnderlineClick(wxCommandEvent& evt);

	void OnFontSizeSelect(wxCommandEvent& evt);

	void OnLeftAlignClick(wxCommandEvent& evt);
	void OnCenterAlignClick(wxCommandEvent& evt);
	void OnRightAlignClick(wxCommandEvent& evt);

	void TextInputShortcuts(wxKeyEvent& evt);
	void OnCaretClick(wxMouseEvent& evt);

	void UpdateTextInfo();
	void TextInputType(wxCommandEvent& evt);



	/*
	wxTextCtrl* textInput;
	wxChoice* colorChoice;
	wxButton* okButton;
	wxButton* cancelButton;
	wxBoxSizer* verticalSizer;
	wxBoxSizer* horizontalSizer;
	wxBoxSizer* buttonHorizontalSizer;
	wxString noteContent;
	wxString noteColor;
	int fontSize;
	void OnOk(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);
	void OnColorChange(wxCommandEvent& evt);
	void OnShortcut(wxKeyEvent& evt);
	*/
};

