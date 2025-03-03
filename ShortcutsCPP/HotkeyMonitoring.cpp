#define HOTKEY_ID 1
#include "App.h"
#include <Windows.h>
#include <thread>
#include <atomic>

std::atomic<bool> running(true);

class HotkeyWaiter : public wxApp
{
public:
	bool OnInit() {
		bgwin = new BGWin();
		bgwin->Show(false);

		std::thread hotkeyThread(&HotkeyWaiter::HotkeyThread, this);
		hotkeyThread.detach();
		return true;

		/*if (!RegisterHotKey(NULL, HOTKEY_ID, MOD_WIN | MOD_ALT, 'S'))
		{
			wxLogError("Не удалось зарегистрировать горячую клавишу");
			return false;
		}
		HWND hwnd = CreateWindowEx(0, L"STATIC", NULL, 0, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL);
		if (!hwnd)
		{
			wxLogError("Не удалось создать скрытое окно");
			return false;
		}

		SetWinEventHook(hwnd);
		return true;*/
	}
	int OnExit()
	{
		return wxApp::OnExit();
	}
private:
	BGWin* bgwin;
	void HotkeyThread()
	{
		if (!RegisterHotKey(NULL, HOTKEY_ID, MOD_WIN | MOD_ALT, 'S'))
		{
			wxLogError("Не удалось зарегистрировать горячую клавишу");
			return;
		}

		MSG msg;
		while (running && GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID)
			{
				
				wxTheApp->CallAfter([this]() {
					MainFrame* app = new MainFrame("Shortcuts", this->bgwin);
					app->previousForegroundWindow = GetForegroundWindow();
					app->Show(true);
					app->Center();
					DWORD currentThreadId = GetCurrentThreadId();
					HWND foregroundWindow = GetForegroundWindow();
					DWORD foregroundThreadId = GetWindowThreadProcessId(foregroundWindow, NULL);
					AttachThreadInput(foregroundThreadId, currentThreadId, TRUE);

					SetForegroundWindow((HWND)app->GetHandle());
					app->SetFocus();

					AttachThreadInput(foregroundThreadId, currentThreadId, FALSE);
					//app->Bind(wxEVT_KEY_DOWN, [this, app](wxKeyEvent& ev) { if (ev.GetKeyCode() == WXK_PAUSE) { app->Close(true); this->bgwin->Close(true); ev.Skip(true); } });
					});
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		UnregisterHotKey(NULL, HOTKEY_ID);
	}

	//void SetWinEventHook(HWND hwnd)
	//{

	//	MSG msg;
	//	while (GetMessage(&msg, NULL, 0, 0))
	//	{
	//		if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID)
	//		{
	//			/*std::thread shortcutsWin(&HotkeyWaiter::WinThread, this);
	//			shortcutsWin.detach();*/
	//			MainFrame* app = new MainFrame("Shortcuts");
	//			app->previousForegroundWindow = GetForegroundWindow();
	//			app->Show(true);
	//			app->Center();
	//			DWORD currentThreadId = GetCurrentThreadId();
	//			HWND foregroundWindow = GetForegroundWindow();
	//			DWORD foregroundThreadId = GetWindowThreadProcessId(foregroundWindow, NULL);
	//			AttachThreadInput(foregroundThreadId, currentThreadId, TRUE);

	//			SetForegroundWindow((HWND)app->GetHandle());
	//			app->SetFocus();

	//			AttachThreadInput(foregroundThreadId, currentThreadId, FALSE);
	//		}
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//}
};

wxIMPLEMENT_APP(HotkeyWaiter);
//wxIMPLEMENT_APP_NO_MAIN(HotkeyWaiter);
//int main() {
//	wxDISABLE_DEBUG_SUPPORT();
//	if (!wxInitialize()) {
//		return -1;
//	}
//	HotkeyWaiter* app = new HotkeyWaiter();
//	wxApp::SetInstance(app);
//	wxEntry();
//	return 0;
//}