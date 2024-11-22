//#include <wx/wx.h>
//
////------------------------------------------------------------------------
//class ImagePanel : public wxPanel
//	//------------------------------------------------------------------------
//{
//public:
//	ImagePanel(wxWindow* parent, wxWindowID, const wxPoint& pos, const wxSize& size);
//	~ImagePanel();
//	void LoadImage(wxString fileName);
//	void SaveImage(wxString fileName);
//	void ProcessImage();
//	void BestSize();
//
//private:
//	int m_imageWidth;
//	int m_imageHeight;
//	wxBitmap m_imageBitmap;	// used to display the image
//	wxImage* m_imageRGB;		// used to load the image
//	unsigned char* m_myImage;	// used to process the image
//
//	void OnPaint(wxPaintEvent& event);
//
//	DECLARE_EVENT_TABLE()
//};
//
//BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
//EVT_PAINT(ImagePanel::OnPaint)
//END_EVENT_TABLE()
