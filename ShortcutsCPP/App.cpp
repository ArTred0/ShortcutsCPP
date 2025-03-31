#include "App.h"
#include <windows.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <cmath>

//class OptGrid;
//
//class Option {
//public:
//	Option(wxWindow* window, wxGridSizer* parent, OptGrid* grid, short col, short row, std::string title, std::string command)
//		: window(window), grid(grid), col(col), row(row), title(title), command(command)
//	{
//		this->widget = new wxStaticText(window, wxID_ANY, title);
//		this->widget->SetFont(grid->fontNormal);
//		parent->Add(this->widget, wxSizerFlags().Center().Proportion(1));
//	}
//
//	wxWindow* window;
//	wxStaticText* widget;
//	OptGrid* grid;
//	std::string title;
//	std::string command;
//	short col;
//	short row;
//	bool isSelected;
//};
//
//class OptGrid {
//public:
//	OptGrid(wxWindow* window, wxGridSizer* parent, wxFont fontNormal, wxFont fontSelected, short cols, short rows)
//		: window(window), parent(parent), fontNormal(fontNormal), fontSelected(fontSelected), cols(cols), rows(rows), selected(nullptr)
//	{
//
//	}
//
//	void AddOption(Option* option) {
//		children.push_back(option);
//	}
//
//	void SelectOption(short col, short row) {
//		Option* toSelect;
//		for (Option* opt : this->children) {
//			if (opt->col == col && opt->row == row)
//				toSelect = opt;
//		}
//		this->selected = toSelect;
//	}
//
//	Option* GetOption(short col, short row) {
//		for (Option* opt : this->children) {
//			if (opt->col == col && opt->row == row) {
//				return opt;
//			}
//		}
//
//		return nullptr;
//	}
//
//	wxWindow* window;
//	wxGridSizer* parent;
//	wxFont fontNormal;
//	wxFont fontSelected;
//	short cols;
//	short rows;
//	std::vector<Option*> children;
//	Option* selected;
//
//};

MainFrame::MainFrame(const wxString& title, BGWin* bgwin) :
	wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR | wxFRAME_SHAPED | wxSTAY_ON_TOP)
{
	this->bgwin = bgwin;
	this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);
	this->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyDown, this);
	this->Bind(wxEVT_ACTIVATE, &MainFrame::OnActivate, this);
	CreateControls();
	panel->Layout();
	this->Fit();
}

ShortcutsList MainFrame::LoadShortcuts()
{
	try {
		std::ifstream file("shortcuts.json");

		std::stringstream buffer;
		buffer << file.rdbuf();

		json jsonObj = json::parse(buffer.str());

		ShortcutsList cmdLst = jsonObj.get<ShortcutsList>();

		return cmdLst;

	}
	catch (...) {
		wxMessageBox("Error occured while reading \"shortcuts.json\" file. Probably it doesn't exist or damaged.");
		this->Destroy();
		return ShortcutsList{};
	}
};

void MainFrame::CreateControls()
{
	panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	panel->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyDown, this);
	panel->SetBackgroundColour(wxColour(10, 10, 10));
	//panel->SetForegroundColour(wxColour(230, 230, 230));

	wxFont fNorm(wxFontInfo(wxSize(0, 28)));
	wxFont fSelect(wxFontInfo(wxSize(0, 27)).Bold());

	ShortcutsList shortcuts = LoadShortcuts();
	std::size_t numOfShortcuts = shortcuts.shortcuts.size();
	int colsNum = 0;
	if (numOfShortcuts <= 4) {
		colsNum = 2;
	}
	else if (numOfShortcuts <= 9) {
		colsNum = 3;
	}
	else if (numOfShortcuts <= 16) {
		colsNum = 4;
	}
	else if (numOfShortcuts <= 25) {
		colsNum = 5;
	}
	int rowsNum = ceil(static_cast<float>(numOfShortcuts) / colsNum);

	wxBoxSizer* border = new wxBoxSizer(wxVERTICAL);
	wxGridSizer* outerSizer = new wxGridSizer(rowsNum, colsNum, 0, 0);
	grid = new OptGrid(this, outerSizer, fNorm, fSelect, colsNum, rowsNum);


	short increment = 0;
	for (short r = 0; r < rowsNum; r++) {
		for (short c = 0; c < colsNum; c++) {
			try {
				Option* opt = new Option(panel, outerSizer, grid, (short)c, (short)r, shortcuts.shortcuts.at(c + increment).title, shortcuts.shortcuts.at(c + increment).command, shortcuts.shortcuts.at(c + increment).wd);
				opt->BindExecuting(this->previousForegroundWindow);
				grid->AddOption(opt);

			}
			catch (const std::out_of_range) {
				break;
			}
		}
		increment += colsNum;
	}
	switch (colsNum) {
	case 1:
	case 2:
		grid->SelectOption(0, 0);
		break;
	case 3:
	case 4:
		grid->SelectOption(1, 1);
		break;
	}
	//grid->SelectOption(0, 0);

	border->Add(outerSizer, wxSizerFlags().Border(wxALL, 25));
	panel->SetSizer(border);
	//outerSizer->Fit(panel);

	border->SetSizeHints(this);


}

//void MainFrame::BindOnKeyDownAndEndingProcess(BGWin* win)
//{
//	
//	this->panel->Bind(wxEVT_KEY_DOWN, &MainFrame::OnKeyDown, this);
//}

void MainFrame::OnKeyDown(wxKeyEvent& ev) {
	if (ev.GetKeyCode() == 'S') {
		this->Close(true);
		LaunchShortcutsManager();
		return;
	}
	switch (ev.GetKeyCode()) {

	case WXK_PAUSE:
		this->Close(true);
		this->bgwin->Close(true);
		break;
	case WXK_ESCAPE:
		SetForegroundWindow(previousForegroundWindow);
		this->Close(true);
		break;
	case WXK_UP:
		grid->KUp();
		break;
	case WXK_DOWN:
		grid->KDown();
		break;
	case WXK_LEFT:
		grid->KLeft();
		break;
	case WXK_RIGHT:
		grid->KRight();
		break;
	case WXK_RETURN:
		grid->ExecuteCommand(previousForegroundWindow);
		break;
	default:
		ev.Skip();
	}
}

void MainFrame::OnActivate(wxActivateEvent& event) {
	if (!event.GetActive()) {
		//this->Destroy();
		this->Close(true);
		//this->Show(false);
		//delete this;
	}
	else {
		SetForegroundWindow((HWND)GetHandle());
		SetFocus();
	}
	event.Skip();
}

void MainFrame::LaunchShortcutsManager()
{
	AppManager* app = new AppManager("Shortcuts Manager");
	app->Show(true);
	app->Center();
}

void MainFrame::OnClose(wxCloseEvent& ev)
{
	Destroy();
}


//wxIMPLEMENT_APP(App);
//bool App::OnInit()
//{
	//MainFrame* app = new MainFrame("Shortcuts");
	//app->previousForegroundWindow = GetForegroundWindow();
	//app->Show(true);
	//app->Center();
	//DWORD currentThreadId = GetCurrentThreadId();
	//HWND foregroundWindow = GetForegroundWindow(); 
	//DWORD foregroundThreadId = GetWindowThreadProcessId(foregroundWindow, NULL);
	//AttachThreadInput(foregroundThreadId, currentThreadId, TRUE);

	//SetForegroundWindow((HWND)app->GetHandle());
	//app->SetFocus();

	//AttachThreadInput(foregroundThreadId, currentThreadId, FALSE);
//
//
//	return true;
//}

