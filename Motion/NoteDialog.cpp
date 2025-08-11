#include "NoteDialog.h"
#include <wx/mstream.h>
#include <wx/base64.h>
#include "MainFrame.h"

NoteDialog::NoteDialog(wxWindow* parent, wxString title, bool isDark, MainFrame* frame)
	: wxDialog(parent, wxID_ANY, title) {


	fontSize = 9;
	mainframe = frame;

	wxPanel* mainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(100,200));
	wxBoxSizer* noteEditSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* SecondRowButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* FirstRowButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* ActionButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

	TextInput = new wxRichTextCtrl(mainPanel, wxID_ANY, "");

	//TextInput->SetHint("Enter Note message");

	updateButton = new wxButton(mainPanel, wxID_ANY, "Update");
	updateButton->SetToolTip("UpdateSelectedNote \t Ctrl+U");

	cancelButton = new wxButton(mainPanel, wxID_ANY, "Cancel");

	ActionButtonsSizer->Add(cancelButton, 1, wxEXPAND);
	ActionButtonsSizer->AddSpacer(1);
	ActionButtonsSizer->Add(updateButton, 1, wxEXPAND);

	boldButton = new wxToggleButton(mainPanel, wxID_ANY, "B", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	boldButton->SetToolTip("Make Bold");
	boldButton->Bind(wxEVT_TOGGLEBUTTON, &NoteDialog::OnBoldClick, this);
	boldButton->SetFont(boldButton->GetFont().MakeBold());

	italicButton = new wxToggleButton(mainPanel, wxID_ANY, "I", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	italicButton->SetToolTip("Make Italic");
	italicButton->Bind(wxEVT_TOGGLEBUTTON, &NoteDialog::OnItalicClick, this);
	italicButton->SetFont(italicButton->GetFont().MakeItalic());

	underlineButton = new wxToggleButton(mainPanel, wxID_ANY, "U", wxDefaultPosition, wxSize(25, 25), wxNO_BORDER);
	underlineButton->SetToolTip("Make Underlined");
	underlineButton->Bind(wxEVT_TOGGLEBUTTON, &NoteDialog::OnUnderlineClick, this);
	underlineButton->SetFont(underlineButton->GetFont().MakeUnderlined());

	wxBitmap bitmap;

	wxIcon LalignIcon("./Lalign.ico", wxBITMAP_TYPE_ICO);
	wxIcon CalignIcon("./Calign.ico", wxBITMAP_TYPE_ICO);
	wxIcon RalignIcon("./Ralign.ico", wxBITMAP_TYPE_ICO);

	wxArrayString fontSizes = {
		"6",
		"8",
		"10",
		"11",
		"12",
		"14",
		"16",
		"20",
		"24",
		"30",
		"32",
		"36"
	};

	FontSizeBox = new wxComboBox(mainPanel, wxID_ANY, to_string(fontSize), wxDefaultPosition, wxSize(-1, 25), fontSizes);

	bitmap = wxBitmap(LalignIcon);
	bitmap.Rescale(bitmap, wxSize(15, 15));
	LeftAlignButton = new wxBitmapButton(mainPanel, wxID_ANY, bitmap, wxDefaultPosition, wxSize(25, 25), wxALIGN_CENTER | wxNO_BORDER);
	LeftAlignButton->SetToolTip("Left Align");
	LeftAlignButton->Bind(wxEVT_BUTTON, &NoteDialog::OnLeftAlignClick, this);

	bitmap = wxBitmap(RalignIcon);
	bitmap.Rescale(bitmap, wxSize(15, 15));
	RightAlignButton = new wxBitmapButton(mainPanel, wxID_ANY, bitmap, wxDefaultPosition, wxSize(25, 25), wxALIGN_CENTER | wxNO_BORDER);
	RightAlignButton->SetToolTip("Right Align");
	RightAlignButton->Bind(wxEVT_BUTTON, &NoteDialog::OnRightAlignClick, this);

	bitmap = wxBitmap(CalignIcon);
	bitmap.Rescale(bitmap, wxSize(15, 15));
	CenterAlignButton = new wxBitmapButton(mainPanel, wxID_ANY, bitmap, wxDefaultPosition, wxSize(25, 25), wxALIGN_CENTER | wxNO_BORDER);
	CenterAlignButton->SetToolTip("Center Align");
	CenterAlignButton->Bind(wxEVT_BUTTON, &NoteDialog::OnCenterAlignClick, this);

	textColorPreview = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxSize(-1, 25), wxBORDER_SIMPLE);
	textColorPreview->SetToolTip("Text color\n the same as the foreground color");

	noteEditSizer->AddSpacer(5);
	noteEditSizer->Add(FirstRowButtonSizer, 0, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->Add(SecondRowButtonSizer, 0, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->AddSpacer(5);
	noteEditSizer->Add(TextInput, 1, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->AddSpacer(5);
	noteEditSizer->Add(ActionButtonsSizer, 0, wxEXPAND | wxRIGHT | wxLEFT, 5);
	noteEditSizer->AddSpacer(5);


	FirstRowButtonSizer->Add(FontSizeBox, 1, wxEXPAND);

	SecondRowButtonSizer->Add(textColorPreview, 1, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(LeftAlignButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(CenterAlignButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(RightAlignButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(boldButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(italicButton, 0, wxEXPAND);
	SecondRowButtonSizer->AddSpacer(1);
	SecondRowButtonSizer->Add(underlineButton, 0, wxEXPAND);

	//TextInput->BeginTextColour(ForegroundColor);

	mainPanel->SetSizerAndFit(noteEditSizer);

	//FontSizeBox->Bind(wxEVT_TEXT, &NoteDialog::OnFontSizeSelect, this);!
	FontSizeBox->Bind(wxEVT_COMBOBOX, &NoteDialog::OnFontSizeSelect, this);

	TextInput->SetFocus();
	TextInput->Bind(wxEVT_KEY_DOWN, &NoteDialog::TextInputShortcuts, this);
	TextInput->Bind(wxEVT_LEFT_DOWN, &NoteDialog::OnCaretClick, this);
	TextInput->Bind(wxEVT_TEXT, &NoteDialog::TextInputType, this);

	updateButton->Bind(wxEVT_BUTTON, &NoteDialog::OnUpdateButtonClick, this);
	cancelButton->Bind(wxEVT_BUTTON, &NoteDialog::OnCancelButtonClick, this);

	InputTextColor = mainframe->ForegroundColor;
	textColorPreview->SetBackgroundColour(InputTextColor);
	UpdateTextInfo();

	this->Refresh();
	this->Layout();
	this->Update();
}


// styling
void NoteDialog::OnBoldClick(wxCommandEvent& evt)
{
	Bold = !Bold;
	attr.SetFontWeight(Bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
	TextInput->SetDefaultStyle(attr);

	if (TextInput->HasSelection()) {

		bool flag = TextInput->IsSelectionBold() ? true : false;

		for (long i = TextInput->GetSelectionRange().GetStart(); i < TextInput->GetSelectionRange().GetEnd(); ++i) {
			wxRichTextAttr charAttr;

			TextInput->GetStyle(i, charAttr);
			charAttr.SetFontWeight(flag ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD);
			TextInput->SetStyle(i, i + 1, charAttr);
			TextInput->Update();
			TextInput->Refresh();

		}
		wxLogStatus("Has selection");
	}

	//CallAfter(&NoteDialog::UpdateTextInfo);
	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void NoteDialog::OnItalicClick(wxCommandEvent& evt)
{
	Italic = !Italic;
	italicButton->SetBackgroundColour(Italic ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());

	attr.SetFontStyle(Italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
	TextInput->SetDefaultStyle(attr);

	if (TextInput->HasSelection()) {

		bool flag = TextInput->IsSelectionItalics() ? true : false;

		for (long i = TextInput->GetSelectionRange().GetStart(); i < TextInput->GetSelectionRange().GetEnd(); i++) {
			wxRichTextAttr charAttr;

			if (TextInput->GetStyle(i, charAttr)) {
				charAttr.SetFontStyle(flag ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC);
				TextInput->SetStyle(i, i + 1, charAttr);
			}
		}
	}

	TextInput->SetFocus();
	italicButton->Refresh();

	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void NoteDialog::OnUnderlineClick(wxCommandEvent& evt)
{
	if (TextInput->HasSelection()) {

		bool flag = TextInput->IsSelectionUnderlined() ? true : false;

		for (long i = TextInput->GetSelectionRange().GetStart(); i < TextInput->GetSelectionRange().GetEnd(); i++) {
			wxRichTextAttr charAttr;

			if (TextInput->GetStyle(i, charAttr)) {
				charAttr.SetFontUnderlined(!flag);
				TextInput->SetStyle(i, i + 1, charAttr);
			}
		}
	}

	Underline = !Underline;

	underlineButton->SetBackgroundColour(Underline ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());

	attr.SetFontUnderlined(Underline);
	TextInput->SetDefaultStyle(attr);

	TextInput->SetFocus();
	underlineButton->Refresh();

	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void NoteDialog::OnLeftAlignClick(wxCommandEvent& evt)
{
	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	Raligntoggle = false;
	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	Caligntoggle = false;


	if (!Laligntoggle) {
		Laligntoggle = true;
		LeftAlignButton->SetBackgroundColour(wxColor(135, 135, 135));

		attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
		TextInput->SetDefaultStyle(attr);
		auto lastCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphStart();
		auto startCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphEnd();
		TextInput->WriteText(" ");
		auto endCaretPosition = TextInput->GetCaretPosition();


		for (long i = startCaretPosition + 1; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetAlignment(wxTEXT_ALIGNMENT_LEFT); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}

		TextInput->SetSelection(endCaretPosition, endCaretPosition + 1);
		TextInput->DeleteSelection();

		TextInput->SetCaretPosition(lastCaretPosition);
	}

	//boldButton->SetBackgroundColour(attr.GetFontWeight() == wxFONTWEIGHT_BOLD? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	//italicButton->SetBackgroundColour(attr.GetFontStyle() == wxFONTSTYLE_ITALIC? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	//boldButton->SetBackgroundColour(attr.HasFontUnderlined() ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());
	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();

	TextInput->SetFocus();
	TextInput->Refresh();
	TextInput->Update();

	ForcedChange = true;
	CallAfter(&NoteDialog::UpdateTextInfo);
}

void NoteDialog::OnCenterAlignClick(wxCommandEvent& evt)
{
	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	Laligntoggle = false;
	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	Raligntoggle = false;

	if (!Caligntoggle) {
		Caligntoggle = true;
		CenterAlignButton->SetBackgroundColour(wxColor(135, 135, 135));

		attr.SetAlignment(wxTEXT_ALIGNMENT_CENTER);
		TextInput->SetDefaultStyle(attr);
		auto lastCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphStart();
		auto startCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphEnd();
		TextInput->WriteText(" ");
		auto endCaretPosition = TextInput->GetCaretPosition();

		for (long i = startCaretPosition + 1; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetAlignment(wxTEXT_ALIGNMENT_CENTER); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}

		TextInput->SetSelection(endCaretPosition, endCaretPosition + 1);
		TextInput->DeleteSelection();

		TextInput->SetCaretPosition(lastCaretPosition);
	}

	//boldButton->SetBackgroundColour(attr.GetFontWeight() == wxFONTWEIGHT_BOLD ? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	//italicButton->SetBackgroundColour(attr.GetFontStyle() == wxFONTSTYLE_ITALIC ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	//boldButton->SetBackgroundColour(attr.HasFontUnderlined() ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());

	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();

	TextInput->SetFocus();
	TextInput->Refresh();
	TextInput->Update();

	ForcedChange = true;
	CallAfter(&NoteDialog::UpdateTextInfo);
}

void NoteDialog::OnRightAlignClick(wxCommandEvent& evt)
{
	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	Laligntoggle = false;
	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	Caligntoggle = false;

	if (!Raligntoggle) {
		Raligntoggle = true;
		RightAlignButton->SetBackgroundColour(wxColor(135, 135, 135));

		attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
		TextInput->SetDefaultStyle(attr);
		auto lastCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphStart();
		auto startCaretPosition = TextInput->GetCaretPosition();
		TextInput->MoveToParagraphEnd();
		TextInput->WriteText(" ");
		auto endCaretPosition = TextInput->GetCaretPosition();

		for (long i = startCaretPosition + 1; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT); // Change ONLY alignment
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}

		TextInput->SetSelection(endCaretPosition, endCaretPosition + 1);
		TextInput->DeleteSelection();

		TextInput->SetCaretPosition(lastCaretPosition);
	}

	//boldButton->SetBackgroundColour(attr.GetFontWeight() == wxFONTWEIGHT_BOLD ? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	//italicButton->SetBackgroundColour(attr.GetFontStyle() == wxFONTSTYLE_ITALIC ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	//boldButton->SetBackgroundColour(attr.HasFontUnderlined() ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());
	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();

	TextInput->SetFocus();
	TextInput->Refresh();
	TextInput->Update();

	ForcedChange = true;
	CallAfter(&NoteDialog::UpdateTextInfo);
}



// text input
void NoteDialog::TextInputShortcuts(wxKeyEvent& evt)
{

	//if (evt.GetKeyCode() == WXK_BACK || evt.GetKeyCode() == WXK_DELETE) {
	//	wxRichTextAttr defaultAttr = TextInput->GetBasicStyle();  // Store default style
	//	CallAfter([this, defaultAttr]() {
	//		long caretPos = TextInput->GetCaretPosition();
	//		if (caretPos > 0) {
	//			TextInput->SetStyle(caretPos, caretPos + 1, defaultAttr);  // Reset style at caret
	//		}
	//		TextInput->SetDefaultStyle(defaultAttr);  // Ensure new text follows default style
	//		});
	//}

	if (evt.ControlDown()) {

		wxCommandEvent dummyEvent;

		switch (evt.GetKeyCode())
		{
		case 'B':
			OnBoldClick(dummyEvent);
			break;
		case 'I':
			OnItalicClick(dummyEvent);
			break;
		case 'U':
			OnUnderlineClick(dummyEvent);
			break;
		case WXK_SPACE:
			ResetFormatting();
			break;
		case 'L':
			OnLeftAlignClick(dummyEvent);
			break;
		case 'R':
			OnRightAlignClick(dummyEvent);
			break;
		case 'E':
			OnCenterAlignClick(dummyEvent);
			break;
		default:
			break;
		}


		TextInput->SetDefaultStyle(attr);
	}

	if (evt.ControlDown() && evt.ShiftDown()) {
		switch (evt.GetKeyCode())
		{
		case '.':
			if (TextInput->HasSelection()) {

				auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
				auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();

				for (long i = startCaretPosition; i < endCaretPosition; ++i)
				{
					wxRichTextAttr charAttr;
					if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
						int newFontSize = charAttr.GetFontSize() + 1;
						charAttr.SetFontSize(newFontSize);
						TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
					}
				}
			}
			attr.SetFontSize(attr.GetFontSize() + 1);
			fontSize++;
			//FontSizeBox->SetValue(to_string(attr.GetFontSize()));
			TextInput->SetDefaultStyle(attr);
			UpdateTextInfo();
			break;
		case ',':
			if (TextInput->HasSelection()) {

				auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
				auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();


				for (long i = startCaretPosition; i < endCaretPosition; ++i)
				{
					wxRichTextAttr charAttr;
					if (TextInput->GetStyle(i, charAttr)) { // Get each character's style

						int newFontSize = charAttr.GetFontSize();
						if (newFontSize > 1) {
							newFontSize -= 1;
							charAttr.SetFontSize(newFontSize);
						}

						TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
					}
				}

			}


			if (attr.GetFontSize() > 1) {
				attr.SetFontSize(attr.GetFontSize() - 1);
				fontSize--;
			}
			//FontSizeBox->SetValue(to_string(attr.GetFontSize()));
			TextInput->SetDefaultStyle(attr);
			UpdateTextInfo();
			break;
		default:
			break;
		}
	}

	if (!evt.ControlDown() && !evt.ShiftDown()) {
		switch (evt.GetKeyCode()) {
		case WXK_LEFT:
			if (TextInput->HasSelection()) TextInput->SetCaretPosition(TextInput->GetSelectionRange().GetStart());
			CallAfter(&NoteDialog::UpdateTextInfo);
			break;
		case WXK_RIGHT:
			if (TextInput->HasSelection()) TextInput->SetCaretPosition(TextInput->GetSelectionRange().GetEnd() - 2);
			CallAfter(&NoteDialog::UpdateTextInfo);
			break;
			//case WXK_UP:
			//	CallAfter(&NoteDialog::UpdateTextInfo);
			//	break;
			//case WXK_DOWN:
			//	CallAfter(&NoteDialog::UpdateTextInfo);
			//	break;
		default:
			break;
		}
	}

	if (TextInput->HasSelection()) wxLogStatus("Selection: (%i, %i)", TextInput->GetSelectionRange().GetStart(), TextInput->GetSelectionRange().GetEnd());

	if (evt.GetKeyCode() == WXK_NUMPAD_ENTER) {
		wxCommandEvent dummyEvent;

		//AddButton(dummyEvent);
	}

	if ((evt.GetKeyCode() == WXK_RETURN || evt.GetKeyCode() == WXK_NUMPAD_ENTER) && evt.ControlDown()) {
		wxCommandEvent dummyEvent;
		OnUpdateButtonClick(dummyEvent);
	}
	evt.Skip();
}

void NoteDialog::OnCaretClick(wxMouseEvent& evt)
{
	CallAfter(&NoteDialog::UpdateTextInfo);
	//UpdateTextInfo();
	evt.Skip();
}

void NoteDialog::TextInputType(wxCommandEvent& evt)
{
	CallAfter(&NoteDialog::UpdateTextInfo);
	evt.Skip();
}

void NoteDialog::OnFontSizeSelect(wxCommandEvent& evt)
{
	auto cursorPos = TextInput->GetCaretPosition();
	//wxRichTextAttr style;
	//TextInput->GetStyle(cursorPos, style);
	//style.SetFontSize(atol(FontSizeBox->GetValue()));
	//TextInput->SetStyle(cursorPos, cursorPos , style);

	fontSize = atol(FontSizeBox->GetValue());

	//newFontSize = true;
	wxLogStatus("New Font Size triggered!");

	if (TextInput->HasSelection()) {

		auto startCaretPosition = TextInput->GetSelectionRange().GetStart();
		auto endCaretPosition = TextInput->GetSelectionRange().GetEnd();

		for (long i = startCaretPosition; i <= endCaretPosition; ++i)
		{
			wxRichTextAttr charAttr;
			if (TextInput->GetStyle(i, charAttr)) { // Get each character's style
				charAttr.SetFontSize(atol(FontSizeBox->GetValue()));
				TextInput->SetStyle(i, i + 1, charAttr); // Apply updated style
			}
		}
	}
	//attr.SetFontSize(attr.GetFontSize() + 1);
	//FontSizeBox->SetValue(to_string(attr.GetFontSize()));

	attr.SetFontSize(fontSize);
	TextInput->SetDefaultStyle(attr);
	TextInput->SetCaretPosition(cursorPos);

	ForcedChange = true;
	UpdateTextInfo();
	evt.Skip();
}

void NoteDialog::OnUpdateButtonClick(wxCommandEvent& evt)
{
	//wxLogStatus("On Update triggered");
	if (mainframe->activeNote != nullptr) {
		mainframe->activeNote->UpdateNote(TextInput, mainframe->BackgroundColor);
	}
	this->EndModal(true);
}

void NoteDialog::OnCancelButtonClick(wxCommandEvent& evt)
{
	this->EndModal(true);
}

void NoteDialog::UpdateTextInfo()
{
	long caretPos = TextInput->GetCaretPosition();
	wxRichTextAttr style;

	wxLogStatus("%i", ForcedChange);

	if (caretPos > 0 && !ForcedChange) {
		TextInput->GetStyle(caretPos, style);
		attr = style;
		TextInput->SetDefaultStyle(attr);

		switch (style.GetAlignment())
		{
		case wxTEXT_ALIGNMENT_LEFT:
			Laligntoggle = true;
			Caligntoggle = false;
			Raligntoggle = false;
			break;
		case wxTEXT_ALIGNMENT_CENTER:
			Laligntoggle = false;
			Caligntoggle = true;
			Raligntoggle = false;
			break;
		case wxTEXT_ALIGNMENT_RIGHT:
			Laligntoggle = false;
			Caligntoggle = false;
			Raligntoggle = true;
			break;
		default:
			break;
		}


		if (style.GetFontWeight() == wxFONTWEIGHT_BOLD) { Bold = true; }
		else { Bold = false; }

		if (style.GetFontStyle() == wxFONTSTYLE_ITALIC) { Italic = true; }
		else { Italic = false; }

		if (style.GetFontUnderlined()) { Underline = true; }
		else { Underline = false; }

		InputTextColor = style.GetTextColour();
	}


	//TextInput->GetStyle(caretPos, style);
	//switch (style.GetAlignment())
	//{
	//case wxTEXT_ALIGNMENT_LEFT:
	//	LeftAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
	//	Laligntoggle = true;
	//	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	//	Raligntoggle = false;
	//	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	//	Caligntoggle = false;
	//	break;
	//case wxTEXT_ALIGNMENT_CENTER:
	//	CenterAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
	//	Caligntoggle = true;
	//	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	//	Laligntoggle = false;
	//	RightAlignButton->SetBackgroundColour(RightAlignButton->GetParent()->GetBackgroundColour());
	//	Raligntoggle = false;
	//	break;
	//case wxTEXT_ALIGNMENT_RIGHT:
	//	RightAlignButton->SetBackgroundColour(wxColor(135, 135, 135));
	//	Raligntoggle = true;
	//	LeftAlignButton->SetBackgroundColour(LeftAlignButton->GetParent()->GetBackgroundColour());
	//	Laligntoggle = false;
	//	CenterAlignButton->SetBackgroundColour(CenterAlignButton->GetParent()->GetBackgroundColour());
	//	Caligntoggle = false;
	//	break;
	//default:
	//	break;
	//}

	LeftAlignButton->SetBackgroundColour(Laligntoggle ? wxColor(135, 135, 135) : LeftAlignButton->GetParent()->GetBackgroundColour());
	CenterAlignButton->SetBackgroundColour(Caligntoggle ? wxColor(135, 135, 135) : CenterAlignButton->GetParent()->GetBackgroundColour());
	RightAlignButton->SetBackgroundColour(Raligntoggle ? wxColor(135, 135, 135) : RightAlignButton->GetParent()->GetBackgroundColour());

	////boldButton->SetBackgroundColour(style.GetFontWeight() == wxFONTWEIGHT_BOLD ? wxColor(135, 135, 135): boldButton->GetParent()->GetBackgroundColour());
	////italicButton->SetBackgroundColour(style.GetFontStyle() == wxFONTSTYLE_ITALIC ? wxColor(135, 135, 135): italicButton->GetParent()->GetBackgroundColour());
	////underlineButton->SetBackgroundColour(style.GetFontUnderlined() ? wxColor(135, 135, 135): underlineButton->GetParent()->GetBackgroundColour());

	boldButton->SetBackgroundColour(Bold ? wxColor(135, 135, 135) : boldButton->GetParent()->GetBackgroundColour());
	italicButton->SetBackgroundColour(Italic ? wxColor(135, 135, 135) : italicButton->GetParent()->GetBackgroundColour());
	underlineButton->SetBackgroundColour(Underline ? wxColor(135, 135, 135) : underlineButton->GetParent()->GetBackgroundColour());

	FontSizeBox->SetValue(to_string(fontSize));
	textColorPreview->SetBackgroundColour(InputTextColor);
	attr.SetTextColour(InputTextColor);

	//textColorPreview->Refresh();
	//boldButton->Refresh();
	//italicButton->Refresh();
	//underlineButton->Refresh();
	//textColorPreview->Refresh();
	//LeftAlignButton->Refresh();
	//RightAlignButton->Refresh();
	//CenterAlignButton->Refresh();
	//FontSizeBox->Refresh();
	//FontSizeBox->Update();

	TextInput->SetDefaultStyle(attr);
	TextInput->SetFocus();
	ForcedChange = false;
}

void NoteDialog::ResetFormatting()
{
	wxCommandEvent dummyEvent;

	if (Bold) { OnBoldClick(dummyEvent); }
	if (Italic) { OnItalicClick(dummyEvent); }
	if (Underline) { OnUnderlineClick(dummyEvent); }
	if (TextInput->HasSelection()) {
		TextInput->SetStyle(TextInput->GetSelectionRange(), TextInput->GetBasicStyle());
	}
	fontSize = 9;
	InputTextColor = wxColor(paletteColors[20]);
	attr.SetFontSize(fontSize);
	attr.SetTextColour(InputTextColor);
	TextInput->SetDefaultStyle(attr);

	ForcedChange = true;
	UpdateTextInfo();
}