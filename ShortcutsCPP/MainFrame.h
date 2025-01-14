//#pragma once
//#include "OptionGrid.h"
//#include <wx/wx.h>
//#include <nlohmann/json.hpp>
//
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
//
//class MainFrame : public wxFrame
//{
//public:
//	MainFrame(const wxString& title);
//	HWND previousForegroundWindow = NULL;
//private:
//	ShortcutsList LoadShortcuts();
//	void CreateControls();
//	void OnKeyDown(wxKeyEvent& event);
//	void OnClose(wxCloseEvent& event);
//	void OnActivate(wxActivateEvent& event);
//	void LaunchShortcutsManager();
//
//	//CommandList GetShortcutsFromFile();
//
//
//	wxPanel* panel;
//	OptGrid* grid;
//
//
//	//wxDECLARE_EVENT_TABLE();
//};
//
