#include "OptionGrid.h"
#include <cstdlib>
#include <thread>

Option::Option(wxWindow* window, wxGridSizer* parent, OptGrid* grid, short col, short row, std::string title, std::string command, std::string wd)
	: window(window), grid(grid), col(col), row(row), title(title), command(command), wd(wd)
{
	this->widget = new wxStaticText(window, wxID_ANY, title);
	this->widget->SetFont(grid->fontNormal);
	this->widget->SetForegroundColour(wxColour(160, 160, 160));
	parent->Add(this->widget, wxSizerFlags().Center().Proportion(1).Border(wxALL, 25));
}

OptGrid::OptGrid(wxWindow* window, wxGridSizer* parent, wxFont fontNormal, wxFont fontSelected, short cols, short rows)
	: window(window), parent(parent), fontNormal(fontNormal), fontSelected(fontSelected), cols(cols), rows(rows), selected(nullptr)
{

}

void OptGrid::AddOption(Option* option) {
	children.push_back(option);
}

void OptGrid::SelectOption(short col = 0, short row = 0) {
	if (!this->selected) {
		this->selected = this->children[0];
	}

	//if (!(this->selected->col == col) && !(this->selected->row == row)) {}
	this->selected->widget->SetFont(this->fontNormal);
	this->selected->widget->SetForegroundColour(wxColour(220, 220, 220));
	Option* toSelect = nullptr;
	for (Option* opt : this->children) {
		if (opt->col == col && opt->row == row)
			toSelect = opt;
	}
	if (toSelect) {
		this->selected = toSelect;
	}
	this->selected->widget->SetFont(this->fontSelected);
	this->selected->widget->SetForegroundColour(wxColour(255, 255, 255));
	
}

Option* OptGrid::GetOption(short col, short row) {
	Option* toReturn = nullptr;
	for (Option* opt : this->children) {
		if (opt->col == col && opt->row == row)
			toReturn = opt;
	}
	return toReturn;

}

void OptGrid::KUp() {
	SelectOption(this->selected->col, this->selected->row - 1);
}
void OptGrid::KDown() {
	SelectOption(this->selected->col, this->selected->row + 1);
}
void OptGrid::KLeft() {
	SelectOption(this->selected->col - 1, this->selected->row);
}
void OptGrid::KRight() {
	SelectOption(this->selected->col + 1, this->selected->row);
}
void Option::OnEnter(wxMouseEvent& ev)
{
	if (ev.Entering()) {
		this->grid->SelectOption(this->col, this->row);
	}
}

std::wstring OptGrid::ToUTF16(std::string& str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (len == 0) {
		throw std::runtime_error("Error in MultiByteToWideChar");
	}
	std::wstring utf16(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &utf16[0], len);
	return utf16;
}

void OptGrid::ExecuteCommand(HWND previousForegroundWindow) {
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring cmd_utf16 = ToUTF16(this->selected->command);
	//std::wstring c(this->selected->command.begin(), this->selected->command.end());
	LPWSTR cmd = const_cast<wchar_t*>(cmd_utf16.c_str());


	std::wstring wd = ToUTF16(this->selected->wd);
	//CreateProcess process = CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, wd.empty() ? NULL : wd.c_str(), &si, &pi)) {
		//wxMessageBox("Process started successfully!");
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {
		wxMessageBox(std::to_string(GetLastError()));
	}
	SetForegroundWindow(previousForegroundWindow);
	this->window->Destroy();
}

//void OptGrid::ExecuteCommand() {
//	//c = this->selected->command;
//	std::thread(&GenerateThread).detach();
//}