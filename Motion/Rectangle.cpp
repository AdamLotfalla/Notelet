#include "Rectangle.h"
#include <wx/dcbuffer.h>
#include <wx/graphics.h>

rectangle::rectangle(wxWindow* PARENT, wxPoint STARTPOS, wxSize defaultSize, int THICKNESS, wxColor COLOR) : wxPanel(PARENT, wxID_ANY, STARTPOS) {

	size = THICKNESS;
	startPos = STARTPOS;
	color = COLOR;
	//this->SetBackgroundColour(color);
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->Bind(wxEVT_PAINT, &rectangle::OnPaint, this);
    this->SetDoubleBuffered(true);
}

void rectangle::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this); // Prevent flickering
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        // Define border properties
        double borderRadius = 2.0; // Radius for rounded corners
        wxColour borderColor = color;
        double borderWidth = size;

        // Get panel dimensions
        wxSize size = GetSize();
        double width = size.GetWidth();
        double height = size.GetHeight();

        // Draw a rounded rectangle
        gc->SetPen(wxPen(borderColor, borderWidth));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);
        gc->DrawRoundedRectangle(0, 0, width, height, borderRadius);

        delete gc;
    }
}
