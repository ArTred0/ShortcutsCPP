#pragma once
#include <wx/wx.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Shortcut
{
	std::string title;
	std::string command;
	std::string wd;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Shortcut, title, command, wd);
};

struct ShortcutsList
{
	std::vector<Shortcut> shortcuts;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ShortcutsList, shortcuts);
};

class AppManager : public wxFrame
{
public:
	AppManager(const wxString& title);
	void OnClickOnList(wxCommandEvent& ev);
	void UpdateList();
	void OnKeyDown(wxKeyEvent& ev);
	void SwapShortcuts(short offset);
	bool WriteShortcuts();
	void ClearInputs();
	void SaveShortcut(wxCommandEvent& ev);
	void DeleteShortcut(wxCommandEvent& ev);

	void OnTitleEnter(wxCommandEvent& event);


	ShortcutsList LoadShortcuts();

	wxPanel* panel;
	ShortcutsList shortcuts;
	wxListBox* list;
	wxTextCtrl* titleInp;
	wxTextCtrl* cmdInp;
	wxTextCtrl* wdInp;
	wxButton* saveBtn;
	wxButton* delBtn;
};

//class AppManager : public wxApp
//{
//private:
//	bool OnInit();
//};

