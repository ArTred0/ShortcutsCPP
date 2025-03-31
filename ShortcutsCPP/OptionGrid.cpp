#include "OptionGrid.h"
#include <cstdlib>
#include <windows.h>
#include <cmath>
#include <thread>

Option::Option(wxPanel* window, wxGridSizer* parent, OptGrid* grid, short col, short row, std::string title, std::string command, std::string wd)
	: window(window), grid(grid), col(col), row(row), title(title), command(command), wd(wd), isSelected(false)
{
	this->outerSizer = new wxBoxSizer(wxVERTICAL);
	this->backgroundPanel = new wxPanel(window, wxID_ANY);
	this->outerPanelSizer = new wxBoxSizer(wxVERTICAL);
	this->innerPanelSizer = new wxBoxSizer(wxVERTICAL);
	this->innerPanel = new wxPanel(this->backgroundPanel, wxID_ANY);
	this->innerPanel->SetBackgroundColour(wxColour(10, 10, 10));
	this->innerPanelInnerSizer = new wxBoxSizer(wxVERTICAL);

	this->text = new wxStaticText(this->innerPanel, wxID_ANY, title);
	this->text->SetFont(grid->fontNormal);
	this->text->SetForegroundColour(wxColour(160, 160, 160));
	this->text->SetBackgroundColour(wxColour(10, 10, 10));

	this->innerPanelInnerSizer->Add(this->text, wxSizerFlags().Center().Border(wxALL, 10));
	this->innerPanel->SetSizer(this->innerPanelInnerSizer);

	this->innerPanelSizer->Add(this->innerPanel);
	this->outerPanelSizer->Add(this->innerPanelSizer, wxSizerFlags().Center().Border(wxALL, 2));
	this->backgroundPanel->SetSizer(outerPanelSizer);

	this->outerSizer->Add(this->backgroundPanel, wxSizerFlags().Center());

	//parent->Add(this->outerFrame, wxSizerFlags().Center().Proportion(1).Border(wxALL, 25));
	parent->Add(this->outerSizer, wxSizerFlags().Proportion(1).Center().Border(wxALL, 25));

	this->text->Bind(wxEVT_ENTER_WINDOW,
		[this](wxMouseEvent& ev) {
			this->OnEnter(ev);
		}
	);
	this->innerPanel->Bind(wxEVT_ENTER_WINDOW,
		[this](wxMouseEvent& ev) {
			this->OnEnter(ev);
		}
	);
}

void Option::BindExecuting(HWND previousForegroundWindow)
{
	this->text->Bind(wxEVT_LEFT_DOWN, [this, previousForegroundWindow](wxMouseEvent& ev) {this->grid->ExecuteCommand(previousForegroundWindow);});
	this->innerPanel->Bind(wxEVT_LEFT_DOWN, [this, previousForegroundWindow](wxMouseEvent& ev) {this->grid->ExecuteCommand(previousForegroundWindow);});
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
	this->selected->Select(false);

	Option* toSelect = nullptr;
	for (Option* opt : this->children) {
		if (opt->col == col && opt->row == row) {
			toSelect = opt;
			break;
		}
	}
	if (toSelect) {
		this->selected = toSelect;
	}

	this->selected->Select(true);
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
	if (this->isSelected)
		return;
	if (ev.Entering()) {
		this->grid->SelectOption(this->col, this->row);
	}
}

void Option::Select(bool state)
{
	if (state) {
		this->isSelected = true;
		//this->text->SetFont(this->grid->fontSelected);
		this->text->SetForegroundColour(wxColour(255, 255, 255));

		this->backgroundPanel->SetBackgroundColour(wxColour(230, 230, 230));
		this->backgroundPanel->Refresh();
		this->backgroundPanel->Update();

		/*this->backgroundPanel->SetWindowStyleFlag(this->backgroundPanel->GetWindowStyleFlag() | wxBORDER_SIMPLE);
		this->backgroundPanel->Refresh();*/
	}
	else {
		this->isSelected = false;
		//this->text->SetFont(this->grid->fontNormal);
		this->text->SetForegroundColour(wxColour(150, 150, 150));

		this->text->SetBackgroundColour(wxColour(10, 10, 10));

		this->backgroundPanel->SetBackgroundColour(wxColour(10, 10, 10));
		this->backgroundPanel->Refresh();
		this->backgroundPanel->Update();
		
		/*this->backgroundPanel->SetWindowStyleFlag(this->backgroundPanel->GetWindowStyleFlag() & ~wxBORDER_SIMPLE);
		this->backgroundPanel->Refresh();*/
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
	//STARTUPINFO si = { sizeof(STARTUPINFO) };
	//PROCESS_INFORMATION pi;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_HIDE;

	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string cmdS = this->selected->command;
	cmdS = "/C \"" + cmdS;
	cmdS += "\"";
	std::wstring cmd = ToUTF16(cmdS);
	//std::wstring c(this->selected->command.begin(), this->selected->command.end());
	//LPWSTR cmd = const_cast<wchar_t*>(cmd_utf16.c_str());


	std::wstring wd = ToUTF16(this->selected->wd);
	////CreateProcess process = CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	//if (CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, wd.empty() ? NULL : wd.c_str(), &si, &pi)) {
	//	//wxMessageBox("Process started successfully!");
	//	CloseHandle(pi.hProcess);
	//	CloseHandle(pi.hThread);
	//}

	HINSTANCE code = ShellExecute(NULL, L"open", L"cmd.exe", cmd.c_str(), wd.c_str(), SW_HIDE);

	switch ((int)code) {
	case 0:
		wxMessageBox("The operating system is out of memory or resources.", "Error occured while executing command", wxICON_ERROR);
		break;
	case ERROR_FILE_NOT_FOUND | SE_ERR_FNF:
		wxMessageBox("The specified file was not found.", "Error occured while executing command", wxICON_ERROR);
		break;
	case ERROR_PATH_NOT_FOUND | SE_ERR_PNF:
		wxMessageBox("	The specified path was not found.", "Error occured while executing command", wxICON_ERROR);
		break;
	case ERROR_BAD_FORMAT:
		wxMessageBox("The .exe file is invalid (non-Win32 .exe or error in .exe image).", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_ACCESSDENIED:
		wxMessageBox("The operating system denied access to the specified file.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_ASSOCINCOMPLETE:
		wxMessageBox("The file name association is incomplete or invalid.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_DDEBUSY:
		wxMessageBox("The DDE transaction could not be completed because other DDE transactions were being processed.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_DDEFAIL:
		wxMessageBox("The DDE transaction failed.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_DDETIMEOUT:
		wxMessageBox("The DDE transaction could not be completed because the request timed out.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_DLLNOTFOUND:
		wxMessageBox("The specified DLL was not found.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_NOASSOC:
		wxMessageBox("There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_OOM:
		wxMessageBox("There was not enough memory to complete the operation.", "Error occured while executing command", wxICON_ERROR);
		break;
	case SE_ERR_SHARE:
		wxMessageBox("A sharing violation occurred.", "Error occured while executing command", wxICON_ERROR);
		break;
	default:
		if ((int)code <= 32) {
			wxMessageBox("An unexpected error occured. Code: " + std::to_string((int)code), "Error occured while executing command", wxICON_ERROR);
		}
	}
	//ShellExecute(NULL, L"open", L"cmd.exe", cmd.c_str(), wd.c_str(), SW_SHOW);
	//ShellExecute()
	//else {
		//std::string errorCode = std::to_string(GetLastError());

		/*int code = 0;
		for (short i = errorCode.length()-1, p = 0; i >= 0; i--, p++) {
			code += (errorCode[i] - '0') * pow(10, p);
		}*/

		/*switch ((int)code) {
		case 0:
			break;
		case 2:
			wxMessageBox("ERROR 2\n\nThe system cannot find the file specified.", "System error occured", wxICON_ERROR);
			break;
		case 4:
			wxMessageBox("ERROR 4\n\nThe system cannot open the file.", "System error occured", wxICON_ERROR);
			break;
		case 5:
			wxMessageBox("ERROR 5\n\nAccess is denied.", "System error occured", wxICON_ERROR);
			break;
		case 123:
			wxMessageBox("ERROR 123\n\nThe filename, directory name, or volume label syntax is incorrect.", "System error occured", wxICON_ERROR);
			break;
		case 267:
			wxMessageBox("ERROR 267\n\nThe directory name is invalid.", "System error occured", wxICON_ERROR);
			break;
		default:
			wxMessageBox("ERROR " + std::to_string((int)code) +"\n\nTo find more information follow this link:\nhttps://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-", "System error occured", wxICON_ERROR);
		}*/
		
		
	//}
	SetForegroundWindow(previousForegroundWindow);
	this->window->Destroy();
}

//void OptGrid::ExecuteCommand() {
//	//c = this->selected->command;
//	std::thread(&GenerateThread).detach();
//}