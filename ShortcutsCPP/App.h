#pragma once
#include <wx/wx.h>
#include "OptionGrid.h"
#include "AppManager.h"
#include "BGWin.h"

//using json = nlohmann::json;
//
//struct Shortcut
//{
//	std::string title;
//	std::string command;
//	std::string wd;
//
//	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Shortcut, title, command, wd)
//};
//
//struct ShortcutsList
//{
//	std::vector<Shortcut> shortcuts;
//
//	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ShortcutsList, shortcuts)
//};

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title, BGWin* bgwin);
	HWND previousForegroundWindow = NULL;
private:
	ShortcutsList LoadShortcuts();
	void CreateControls();
	//void BindOnKeyDownAndEndingProcess(BGWin* win);
	void OnKeyDown(wxKeyEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnActivate(wxActivateEvent& event);
	void LaunchShortcutsManager();

	BGWin* bgwin;
	wxPanel* panel;
	OptGrid* grid;
};


//class App : public wxApp
//{
//public:
//	bool OnInit();
//};

