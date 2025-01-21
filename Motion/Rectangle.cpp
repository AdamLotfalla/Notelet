#include "Rectangle.h"
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include "MainFrame.h"

rectangle::rectangle(wxWindow* PARENT, wxPoint STARTPOS, wxSize defaultSize, int THICKNESS, wxColor FCOLOR, wxColor BCOLOR, MainFrame* FRAME) : wxWindow(PARENT, wxID_ANY, STARTPOS) {

	size = THICKNESS;
	startPos = STARTPOS;
	fcolor = FCOLOR;
    bcolor = BCOLOR;
    mainframe = FRAME;
    parent = PARENT;
	//this->SetBackgroundColour(color);
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->SetDoubleBuffered(true);

    sizerKnob = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(15, 15));
    sizerKnob->SetBackgroundColour(wxColor(135, 135, 135));
    sizerKnob->Show(false);
    sizerKnob->Raise();

    Bind(wxEVT_LEFT_DOWN, &rectangle::OnPanelLeftDown, this);
    Bind(wxEVT_LEFT_UP, &rectangle::OnPanelLeftUp, this);
    Bind(wxEVT_MOTION, &rectangle::OnPanelMotion, this);
    Bind(wxEVT_PAINT, &rectangle::OnPanelPaint, this);
    Bind(wxEVT_ENTER_WINDOW, &rectangle::OnPanelHover, this);
    Bind(wxEVT_KEY_DOWN, &rectangle::ShortCuts, this);

    sizerKnob->Bind(wxEVT_LEFT_DOWN, &rectangle::OnKnobLeftDown, this);
    sizerKnob->Bind(wxEVT_LEFT_UP, &rectangle::OnKnobLeftUp, this);
    sizerKnob->Bind(wxEVT_MOTION, &rectangle::OnKnobMotion, this);
    sizerKnob->Bind(wxEVT_ENTER_WINDOW, &rectangle::OnKnobHover, this);
    //sizerKnob->Bind(wxEVT_PAINT, &rectangle::OnKnobPaint, this);

    this->SetDoubleBuffered(true);
}

void rectangle::OnPanelPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this); // Prevent flickering
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        // Define border properties
        double borderRadius = 2.0; // Radius for rounded corners
        //wxColour borderColor = fcolor;
        wxColour borderColor = *wxBLACK;
        double borderWidth = size;

        // Get panel dimensions
        wxSize size = GetSize();
        double width = size.GetWidth();
        double height = size.GetHeight();

        // Draw a rounded rectangle
        gc->SetPen(wxPen(borderColor, borderWidth));
        gc->SetBrush(wxBrush(bcolor));
        gc->DrawRoundedRectangle(0, 0, width, height, borderRadius);

        delete gc;
    }
}

void rectangle::OnPanelLeftDown(wxMouseEvent& evt)
{
    if (mainframe->activeRectangle != nullptr) { mainframe->activeRectangle->sizerKnob->Show(false); }
    mainframe->activeRectangle = this;
    mainframe->activeNote = nullptr;
    active = true;

    SetCursor(wxCURSOR_CLOSED_HAND);

    this->CaptureMouse();
    this->SetFocus();

    offset = wxGetMousePosition() - this->GetPosition();

    if (!sizerKnob->IsShown()) {
        sizerKnob->Show(true);
    }
    Refresh();
}

void rectangle::OnPanelLeftUp(wxMouseEvent& evt)
{
    if (this->HasCapture()) {
        this->ReleaseMouse();
    }

    startPos = this->GetPosition();

    Refresh();
}

void rectangle::OnPanelHover(wxMouseEvent& evt)
{
    SetCursor(wxCURSOR_OPEN_HAND);
    Refresh();
}

void rectangle::OnPanelMotion(wxMouseEvent& evt)
{
    if (evt.LeftIsDown() && evt.Dragging()) {
        SetCursor(wxCURSOR_CLOSED_HAND);
        auto mPos = wxGetMousePosition();
        mainframe->activeRectangle->SetPosition(mPos - offset);
    }
}

void rectangle::OnKnobPaint(wxPaintEvent& evt)
{
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);

    if (gc && active) {
        sizerKnob->Show(true);
        gc->SetPen(*wxBLACK_PEN);
        gc->SetBrush(*wxWHITE_BRUSH);
        gc->DrawEllipse(10, 10, 20, 20);
    }
    if (mainframe->activeRectangle != this) {
        sizerKnob->Show(true);
    }
}

void rectangle::OnKnobLeftDown(wxMouseEvent& evt)
{
    wxLogStatus("Knob Left Down");
    SetCursor(wxCURSOR_SIZENWSE);
    sizerKnob->CaptureMouse();
    bufferPoint = this->GetPosition();
    initialSize = this->GetSize();
}

void rectangle::OnKnobLeftUp(wxMouseEvent& evt)
{
    SetCursor(wxCURSOR_DEFAULT);
    sizerKnob->ReleaseMouse();
    sizerKnob->SetPosition(wxPoint(this->GetSize().GetWidth() - 15, this->GetSize().GetHeight() - 15));
    sizerKnob->Show(true);
    Refresh();
}

void rectangle::OnKnobMotion(wxMouseEvent& evt)
{
    if (evt.LeftIsDown() && evt.Dragging() && active) {
        SetCursor(wxCURSOR_SIZENWSE);

        int sizeX = std::max(initialSize.x + evt.GetPosition().x, 25);
        int sizeY = std::max(initialSize.y + evt.GetPosition().y, 25);

        this->SetSize(wxSize(sizeX, sizeY));

        sizerKnob->Show(false);
        Refresh();
    }
}

void rectangle::OnKnobHover(wxMouseEvent& evt)
{
    SetCursor(wxCURSOR_SIZENWSE);
}

void rectangle::ShortCuts(wxKeyEvent& evt)
{
    wxLogStatus("shortcuts");
    if (evt.GetKeyCode() == WXK_DELETE) {
        wxLogStatus("Delete");
        mainframe->activeRectangle = nullptr;
        this->Destroy();
    }
    else if (evt.GetKeyCode() == WXK_PAGEUP) {
        this->Raise();
    }
    else if (evt.GetKeyCode() == WXK_PAGEDOWN) {
        this->Lower();
    }

}
