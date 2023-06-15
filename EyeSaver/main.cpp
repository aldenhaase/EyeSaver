#include <Windows.h>
#include <string>
#define IDT_TIMER1 1
#define SHORT_INTERVAL 3000
#define LONG_INTERVAL 5000
#define MINIMIZED 0
#define MAXIMIZED 1

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateFullScreenWindow(HINSTANCE);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hwnd;
	MSG msg;

	hwnd = CreateFullScreenWindow(hInstance);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	SetWindowLong(hwnd, GWL_EXSTYLE,
	GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

	SetLayeredWindowAttributes(hwnd, 0, 230, LWA_ALPHA);
	UpdateWindow(hwnd);
	SetTimer(hwnd,              
		IDT_TIMER1,            
		LONG_INTERVAL,                  
		(TIMERPROC)NULL);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT	ps;
	RECT		rect;
	static boolean screenSize = 0;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, CreateHatchBrush(HS_BDIAGONAL, RGB(0, 0, 0)));
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_TIMER:
		switch(screenSize)
		{
			case MINIMIZED:
				PostMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
				SetTimer(hwnd,
					IDT_TIMER1,
					LONG_INTERVAL,
					(TIMERPROC)NULL);
				screenSize = MAXIMIZED;
				break;
			case MAXIMIZED:
				PostMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				SetTimer(hwnd,
					IDT_TIMER1,
					SHORT_INTERVAL,
					(TIMERPROC)NULL);
				screenSize = MINIMIZED;
				break;
		return 0;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND CreateFullScreenWindow(HINSTANCE hInstance)
{
	static TCHAR szAppName[] = TEXT("EYE SAVER");

	HWND		hwnd = GetDesktopWindow();
	WNDCLASS	wndclass;

	HWND hDesktop = GetDesktopWindow();
	RECT desktop;
	GetWindowRect(hDesktop, &desktop);

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = MainWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	RegisterClass(&wndclass);

	HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hMon, &mi))
		return NULL;

	return CreateWindow(szAppName,
		NULL,
		WS_POPUP | WS_VISIBLE,
		mi.rcMonitor.right,
		mi.rcMonitor.bottom,
		mi.rcMonitor.right - mi.rcMonitor.left,
		mi.rcMonitor.bottom - mi.rcMonitor.top,
		hwnd, NULL, hInstance, 0);
}