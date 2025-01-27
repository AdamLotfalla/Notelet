#include "ImageBox.h"
#include "MainFrame.h"

ImageBox::ImageBox(wxWindow* parent, wxPoint POS, wxSize SIZE, wxImage IMAGE, MainFrame* frame) : rectangle(parent, POS, SIZE, 2, *wxBLACK, *wxWHITE, frame)
{
	image = IMAGE;
	bitmap = wxBitmap(image);

	ratio = (double) bitmap.GetSize().GetWidth() / (double) bitmap.GetSize().GetHeight();
	int height = SIZE.GetHeight();
	bitmap.Rescale(bitmap, wxSize(ratio * height,height));

	mainSizer = new wxBoxSizer(wxVERTICAL);
	display = new wxStaticBitmap(this, wxID_ANY, bitmap);

	//this->minSize = wxSize((ratio * height + 2 * margin)/10, (height + 2 * margin)/10);
	mainSizer->Add(display, 0, wxALL, margin);

	this->sizerKnob->Unbind(wxEVT_MOTION, &rectangle::OnKnobMotion, this);
	this->sizerKnob->Bind(wxEVT_MOTION, &ImageBox::OnKnobMotion, this);
	display->Bind(wxEVT_LEFT_DOWN, &rectangle::OnPanelLeftDown, this);
	display->Bind(wxEVT_LEFT_UP, &rectangle::OnPanelLeftUp, this);
	display->Bind(wxEVT_MOTION, &rectangle::OnPanelMotion, this);
	this->SetSizerAndFit(mainSizer);
	this->sizerKnob->SetPosition(wxPoint(this->GetSize().x - 15, this->GetSize().y - 15));

	this->Layout();
	this->Refresh();
}

void ImageBox::OnKnobMotion(wxMouseEvent& evt) {
	rectangle::OnKnobMotion(evt);
	if (evt.LeftIsDown() && evt.Dragging() && this->active) {
		int width = this->GetSize().GetWidth();
		int height = this->GetSize().GetHeight();

		bitmap=wxBitmap(image);
		wxSize size;
		bitmap.Rescale(bitmap, width > (height * ratio) ? wxSize(ratio * height - (2 * margin), height - (2 * margin)) : wxSize(width - (2 * margin), width / ratio - (2 * margin)));
		display->SetBitmap(bitmap);

		this->Layout();
		this->Refresh();
	}
}
