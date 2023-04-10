
#include "framework.h"
#include "wind2d.h" // wind2d.cpp : Defines the entry point for the application.
#include <d2d1.h>

#include <wincodec.h>
#include <atlbase.h>

#pragma comment(lib, "d2d1.lib")

#include "direct2d.h"
graphics* gfx;

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIND2D, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIND2D));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    delete gfx;

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIND2D));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIND2D);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE:
        {
            gfx = new graphics();
            if (!gfx->init(hWnd)) {
                delete gfx;
                return -1;
            };
        }
    break;

    case WM_PAINT:
        {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
/*
        // Obtain the size of the drawing area.
        RECT rc;
        GetClientRect(hWnd, &rc);

        // Save the original object
        HGDIOBJ original = NULL;
        original = SelectObject(ps.hdc, GetStockObject(DC_PEN));

        // Create a pen.            
        HPEN blackPen = CreatePen(PS_SOLID, 3, 0);

        // Select the pen.
        SelectObject(ps.hdc, blackPen);

        // Draw a rectangle.
        Rectangle(
            ps.hdc,
            rc.left + 100,
            rc.top + 100,
            rc.right - 100,
            rc.bottom - 100);

        DeleteObject(blackPen);

        // Restore the original object
        SelectObject(ps.hdc, original);

        EndPaint(hWnd, &ps);
*/
        POINT mouloc;	GetCursorPos(&mouloc);	ScreenToClient(hWnd, &mouloc);

        gfx->begindraw();
        gfx->clear(.5f,.5f,0);
        gfx->circle(100,100,25,1.f,.5f,0.f,1.f);
        gfx->fillcircle(200, 100, 25, 1.f, .5f, 0.f, 1.f);
        gfx->fillrectangle(300,300,600,350, .5f,1.f,1.f,.1f);
        gfx->line(300, 300, 600, 250, 25.f, 1.f, .5f, 1.f, 1.f);
        gfx->rectangle(300, 300, 600, 250, 5.f, .5f, 0.f, 1.f, 1.f);
        gfx->triangle(600, 250, 700,100, 800,400, 1.f, 0.f, 0.f, 1.f);
        gfx->fillcircle((float)mouloc.x, (float)mouloc.y, 25, 1.f, .5f, 1.f, .5f);


		//ID2D1Bitmap* bitmap = nullptr;
		//gfx->factory->CreateBitmapFromFile(L"pic0.jpeg", D2D1::BitmapProperties(), &bitmap);

/* hmm don't seem to work real good
		ID2D1Bitmap* bitmap = gfx->LoadBitmapFromFile(L"pic0.jpeg");
		gfx->rendertarget->BeginDraw();
		gfx->rendertarget->SetOpacity(0.5f);
		D2D1_RECT_F destRect = D2D1::RectF(0, 0, 512, 512);
		gfx->rendertarget->DrawBitmap(bitmap, &destRect);
		gfx->rendertarget->EndDraw();
		SafeRelease(&bitmap);
*/
        gfx->enddraw();

        EndPaint(hWnd, &ps);

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
