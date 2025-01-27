#include "ColorDialog.h"
#include <String>
#include "RGB2HSL.h"
#include "MainFrame.h"

using namespace std;

ColorDialog::ColorDialog(wxWindow* parent, wxString title, MainFrame* frame)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(-1, 250)) {

    mainframe = frame;
    color = "#000000";

    outerSizer = new wxGridSizer(1, 1, wxSize(0, 0));
    verticalSizer = new wxBoxSizer(wxVERTICAL);
    horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);

    wxArrayString colorFormats = { "RGB", "HSL", "Hex" };

    RH = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1,25));
    GS = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 25));
    BL = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 25));
    Hex = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 25));

    colorPreview = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1,-1), wxBORDER_SIMPLE);

    addButton = new wxButton(this, wxID_ANY, "Add",
        wxDefaultPosition, wxSize(-1, 30));
    cancelButton = new wxButton(this, wxID_ANY, "Cancel",
        wxDefaultPosition, wxSize(-1, 30));

    colorFormatChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, colorFormats);

    colorFormatChoice->SetSelection(0);

    // Setup outer sizer
    this->SetSizer(outerSizer);
    outerSizer->Add(verticalSizer, 1, wxEXPAND | wxALL, 10);

    // Add components to vertical sizer
    verticalSizer->Add(colorFormatChoice, 0, wxEXPAND);
    verticalSizer->AddSpacer(5);
    verticalSizer->Add(horizontalSizer, 0, wxEXPAND);

    // Add text fields to horizontal sizer
    horizontalSizer->Add(RH, 1, wxEXPAND | wxRIGHT, 3);
    horizontalSizer->Add(GS, 1, wxEXPAND | wxRIGHT, 3);
    horizontalSizer->Add(BL, 1, wxEXPAND, 3);

    // Add Hex field
    verticalSizer->Add(Hex, 0, wxEXPAND | wxTOP | wxBOTTOM, 5);
    Hex->Hide(); // Initially hide Hex field

    // Add color preview panel
    verticalSizer->Add(colorPreview, 1, wxEXPAND | wxALL, 5);

    // Add buttons
    buttonHorizontalSizer->Add(addButton, 1, wxEXPAND | wxRIGHT, 5);
    buttonHorizontalSizer->Add(cancelButton, 1, wxEXPAND);
    verticalSizer->Add(buttonHorizontalSizer, 0, wxEXPAND | wxTOP, 10);

    // Adjust visibility based on initial selection
    UpdateVisibility();

    // Update textBoxes
    colorFormatChoice->Bind(wxEVT_CHOICE, &ColorDialog::ChooseFormat, this);
    RH->Bind(wxEVT_COMMAND_TEXT_UPDATED, &ColorDialog::WriteBoxes, this);
    GS->Bind(wxEVT_COMMAND_TEXT_UPDATED, &ColorDialog::WriteBoxes, this);
    BL->Bind(wxEVT_COMMAND_TEXT_UPDATED, &ColorDialog::WriteBoxes, this);
    Hex->Bind(wxEVT_COMMAND_TEXT_UPDATED, &ColorDialog::WriteBoxes, this);

    this->Bind(wxEVT_KEY_DOWN, &ColorDialog::Shortcut, this);

    addButton->Bind(wxEVT_BUTTON, &ColorDialog::AddButton, this);
    cancelButton->Bind(wxEVT_BUTTON, &ColorDialog::CancelButton, this);



    // Layout components
    RH->SetFocus();
    colorPreview->SetBackgroundColour("#a7a7a7");
    Refresh();
    this->Layout();
}

void ColorDialog::UpdateVisibility() {
    int selection = colorFormatChoice->GetSelection();
    if (selection == 0 || selection == 1) {

        horizontalSizer->Show(true);
        Hex->Hide();

        RH->SetHint(selection == 0 ? "R" : "H");
        GS->SetHint(selection == 0 ? "G" : "S");
        BL->SetHint(selection == 0 ? "B" : "L");
    }
    else if (selection == 2) {
        horizontalSizer->Show(false);
        Hex->Show();
        Hex->SetHint("Hex");
    }

    float r = atol((RH->GetValue()).c_str());
    float g = atol((GS->GetValue()).c_str());
    float b = atol((BL->GetValue()).c_str());

    float r2;
    float g2;
    float b2;

    wxString hexCode = Hex->GetValue();

    switch (selection)
    {
    case 0:
        color = wxColor(r,g,b);
        break;
    case 1:
        if (r <= 360 && g <= 100 && b <= 100) {
            r2= hsl2rgb(r / 360, g / 100, b / 100).r;
            g2= hsl2rgb(r / 360, g / 100, b / 100).g;
            b2= hsl2rgb(r / 360, g / 100, b / 100).b;

            color = wxColor(r2, g2, b2);
        }
        else {
            color = *wxBLACK;
        }

        break;
    case 2:
        if (wxColor(hexCode).IsOk() || wxColor("#" + hexCode).IsOk()) {
            if (hexCode[0] == '#') {
                color = wxColor(hexCode);
            }
            else {
                color = wxColor("#" + hexCode);
            }
        }
        else {
            color = *wxBLACK;
        }
        break;
    default:
        wxLogError("Unsupported color format!");
        break;
    }

    colorPreview->SetBackgroundColour(color);

    Refresh();
    this->Layout();
}

void ColorDialog::ChooseFormat(wxCommandEvent& evt)
{
    UpdateVisibility();

    auto selection = colorFormatChoice->GetSelection();

    if (selection == 0 || selection == 1) {
        RH->SetFocus();
    }
    else if(selection == 2) {
        Hex->SetFocus();
    }

    evt.Skip();
}

void ColorDialog::WriteBoxes(wxCommandEvent& evt)
{
    UpdateVisibility();
    evt.Skip();
}

void ColorDialog::AddButton(wxCommandEvent& evt)
{
    mainframe->paletteColors.push_back(string(color.GetAsString()));
    mainframe->UpdateColors();
    this->EndModal(1);
}

void ColorDialog::CancelButton(wxCommandEvent& evt)
{
    this->EndModal(0);
}

void ColorDialog::Shortcut(wxKeyEvent& evt)
{
    if (evt.GetKeyCode() == WXK_NUMPAD_ENTER) {
        wxCommandEvent dummyEvent;

        AddButton(dummyEvent);
    }
    evt.Skip();
}
