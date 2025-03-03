#pragma once
#include <wx/wx.h>
#include <vector>

class OptGrid;

class Option {
public:
	Option(wxWindow* window, wxGridSizer* parent, OptGrid* grid, short col, short row, std::string title, std::string command, std::string wd);
	void OnEnter(wxMouseEvent& ev);

	wxWindow* window;
	wxStaticText* widget;
	OptGrid* grid;
	std::string title;
	std::string command;
	std::string wd;
	short col;
	short row;
};

class OptGrid {
public:
	OptGrid(wxWindow* window, wxGridSizer* parent, wxFont fontNormal, wxFont fontSelected, short cols, short rows);

	void AddOption(Option* option);
	void SelectOption(short col, short row);
	Option* GetOption(short col, short row);

	void KUp();
	void KDown();
	void KLeft();
	void KRight();

	std::wstring ToUTF16(std::string& str);
	void ExecuteCommand(HWND previousForegroundWindow);

	wxWindow* window;
	wxGridSizer* parent;
	wxFont fontNormal;
	wxFont fontSelected;
	short cols;
	short rows;
	std::vector<Option*> children;
	Option* selected;

};

