//#include "CustomButton.h"
//#include <wx/wx.h>
//#include <wx/dcbuffer.h>
//
//bool mHover = false;
//bool mPress = false;
//
//
//CustomButton::CustomButton(wxWindow* parent, wxString LABEL, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize) {
//
//	SetBackgroundStyle(wxBG_STYLE_PAINT);
//	body = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 25));
//	label = new wxStaticText(body, wxID_ANY, LABEL);
//
//
//	sizer->Add(label, 1, wxALIGN_CENTER | wxALL, 0.5);
//	body->SetSizerAndFit(sizer);
//
//	body->Bind(wxEVT_ENTER_WINDOW, &CustomButton::OnMouseEnter);
//	body->Bind(wxEVT_LEAVE_WINDOW, &CustomButton::OnMouseLeave);
//	body->Bind(wxEVT_LEFT_DOWN, &CustomButton::OnMouseDown);
//	body->Bind(wxEVT_LEFT_UP, &CustomButton::OnMouseUp);
//	body->Bind(wxEVT_PAINT, &CustomButton::OnPaint);
//
//}
//
//
////void OnPaint(wxPaintEvent& event) {
////	wxAutoBufferedPaintDC dc(this);
////	dc.SetBackground(wxBrush(mPress ? wxColor("#CCCCCC") : mHover ? wxColor("#DDDDDD") : wxColor("#EEEEEE")));
////	dc.Clear();
////
////	dc.SetTextForeground(wxColor("#333333"));
////	wxSize textSize = dc.GetTextExtent(label);
////	//dc.DrawText(label, (GetSize().x - textSize.x) / 2, (GetSize().y - textSize.y) / 2);
////}
//
//void OnMouseEnter(wxMouseEvent& event) {
//	mHover = true;
//	//Refresh();
//}
//
//void OnMouseLeave(wxMouseEvent& event) {
//	mHover = false;
//	//Refresh();
//}
//
//void OnMouseDown(wxMouseEvent& event) {
//	mPress = true;
//	//Refresh();
//}
//
//void OnMouseUp(wxMouseEvent& event) {
//	mPress = false;
//	//Refresh();
//}
