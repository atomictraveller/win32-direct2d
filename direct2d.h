#pragma once

class graphics
{
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* rendertarget;
	ID2D1SolidColorBrush* brush;
	ID2D1PathGeometry* pathgeometry;
	ID2D1GeometrySink* gsink;
	ID2D1Bitmap* LoadBitmapFromFile(const wchar_t* filename);

public:
	graphics();
	~graphics();
	bool init(HWND windowhandle);

	void begindraw() {rendertarget->BeginDraw();}
	void enddraw() {rendertarget->EndDraw();}

	void clear(float r, float g, float b);
	void circle(float x, float y, float radius, float r, float g, float b, float a);
	void fillcircle(float x, float y, float radius, float r, float g, float b, float a);
	void rectangle(float x, float y, float x0, float y0, float width, float r, float g, float b, float a);
	void fillrectangle(float x, float y, float x0, float y0, float r, float g, float b, float a);
	void line(float x, float y, float x0, float y0, float width, float r, float g, float b, float a);
	void triangle(float x, float y, float x0, float y0, float x1, float y1, float r, float g, float b, float a);
};

graphics::graphics() {
	factory = NULL;
	rendertarget = NULL;
	brush = NULL;
	pathgeometry = NULL;
	gsink = NULL;
}

graphics::~graphics() {
	if(factory) factory->Release();
	if (rendertarget) rendertarget->Release();
	if (brush) brush->Release();
	if(pathgeometry) pathgeometry->Release();
	if (gsink) gsink->Release();
}
//D2D1_FACTORY_TYPE_MULTI_THREADED,
bool::graphics::init(HWND windowhandle) {
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (result != S_OK) return false;

	RECT rect;
	GetClientRect(windowhandle, &rect);

	result = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(windowhandle, D2D1::SizeU(rect.right, rect.bottom)),
		&rendertarget);
	if (result != S_OK) return false;

	rendertarget->CreateSolidColorBrush(D2D1::ColorF(0,0,0,1), &brush);
	factory->CreatePathGeometry(&pathgeometry);

	return true;
}


void graphics::clear(float r, float g, float b) {
	rendertarget->Clear(D2D1::ColorF(r, g, b));
}

void graphics::circle(float x, float y, float radius, float r, float g, float b, float a) {
	brush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush, 3.f);	
}

void graphics::fillcircle(float x, float y, float radius, float r, float g, float b, float a) {
	brush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), brush);
}

void graphics::rectangle(float x, float y, float x0, float y0, float width, float r, float g, float b, float a) {
	brush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->DrawRectangle(D2D1::RectF(x, y, x0, y0), brush, width);
}

void graphics::fillrectangle(float x, float y, float x0, float y0, float r, float g, float b, float a) {
	brush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->FillRectangle(D2D1::RectF(x, y, x0, y0), brush);
}

void graphics::line(float x, float y, float x0, float y0, float width, float r, float g, float b, float a) {
	brush->SetColor(D2D1::ColorF(r, g, b, a));
	rendertarget->DrawLine(D2D1::Point2F(x, y), D2D1::Point2F(x0, y0), brush, width);
}

void graphics::triangle(float x, float y, float x0, float y0, float x1, float y1, float r, float g, float b, float a) {
	brush->SetColor(D2D1::ColorF(r, g, b, a));

	rendertarget->SetTransform(D2D1::Matrix3x2F::Rotation(-15.f, D2D1::Point2F(468.0f, 331.5f)));

	HRESULT result = factory->CreatePathGeometry(&pathgeometry);
	result = pathgeometry->Open(&gsink);
	gsink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
	gsink->AddLine(D2D1::Point2F(x0, y0));
	gsink->AddLine(D2D1::Point2F(x1, y1));
	gsink->EndFigure(D2D1_FIGURE_END_CLOSED);
	gsink->Close();

	rendertarget->FillGeometry(pathgeometry, brush);


	rendertarget->SetTransform(D2D1::Matrix3x2F::Identity());
}
/*
ID2D1Bitmap* graphics::LoadBitmapFromFile(const wchar_t* filename) {
	ID2D1Bitmap* bitmap = nullptr;
	HRESULT result = factory->CreateBitmapFromFile(filename, D2D1::BitmapProperties(), &bitmap);
	if (result != S_OK) {
		// Handle error, e.g., return nullptr or throw an exception
	}
	return bitmap;
}
*/