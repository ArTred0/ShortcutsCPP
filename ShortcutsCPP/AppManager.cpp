#include "AppManager.h"
#include <fstream>
#include <sstream>
#include <format>

AppManager::AppManager(const wxString& title) :
	wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxDefaultSize)
{
	//this->Bind(wxEVT_CHAR_HOOK, [this](wxKeyEvent& ev) { if (ev.GetKeyCode() == WXK_ESCAPE) { this->Close(true);/*this->Show(false); delete this;*/ /*this->Destroy();*/ }});
	panel = new wxPanel(this);
	wxGridSizer* border = new wxGridSizer(1, 1, wxDefaultSize);
	wxBoxSizer* outerSizer = new wxBoxSizer(wxHORIZONTAL);
	shortcuts = LoadShortcuts();

	//const int len = shortcuts.shortcuts.size();
	//std::vector<wxString> choices(len);
	list = new wxListBox(panel, wxID_ANY);
	list->Bind(wxEVT_LISTBOX, &AppManager::OnClickOnList, this);
	list->Bind(wxEVT_KEY_DOWN, &AppManager::OnKeyDown, this);
	if ((int)shortcuts.shortcuts.size() < 25)
		list->Insert("<Add new>", 0);

	for (Shortcut s : shortcuts.shortcuts) {
		list->Append(s.title);
	}

	wxBoxSizer* innerSizer = new wxBoxSizer(wxVERTICAL);


	wxFont headerFont(wxFontInfo(wxSize(0, 20)));
	wxStaticText* lbl = new wxStaticText(panel, wxID_ANY, "Shortcut editing");
	lbl->SetFont(headerFont);


	wxBoxSizer* titleRow = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* lblT = new wxStaticText(panel, wxID_ANY, "Title:");
	titleInp = new wxTextCtrl(panel, wxID_ANY);
	titleInp->Bind(wxEVT_TEXT, &AppManager::OnTitleEnter, this);
	titleRow->Add(lblT);
	titleRow->AddSpacer(10);
	titleRow->Add(titleInp, wxSizerFlags().Proportion(1));


	wxBoxSizer* cmdRow = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* lblC = new wxStaticText(panel, wxID_ANY, "Command:");
	cmdInp = new wxTextCtrl(panel, wxID_ANY);
	cmdRow->Add(lblC);
	cmdRow->AddSpacer(10);
	cmdRow->Add(cmdInp, wxSizerFlags().Proportion(1));


	wxBoxSizer* wdRow = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText* lblW = new wxStaticText(panel, wxID_ANY, "Working directory:");
	wdInp = new wxTextCtrl(panel, wxID_ANY);
	wdRow->Add(lblW);
	wdRow->AddSpacer(10);
	wdRow->Add(wdInp, wxSizerFlags().Proportion(1));


	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);

	saveBtn = new wxButton(panel, wxID_ANY, "Save");
	saveBtn->Enable(false);
	saveBtn->Bind(wxEVT_BUTTON, &AppManager::SaveShortcut, this);

	delBtn = new wxButton(panel, wxID_ANY, "Delete");
	delBtn->Enable(false);
	delBtn->Bind(wxEVT_BUTTON, &AppManager::DeleteShortcut, this);

	btnSizer->Add(saveBtn);
	btnSizer->AddSpacer(20);
	btnSizer->Add(delBtn);


	innerSizer->Add(lbl, wxSizerFlags().Center());
	innerSizer->AddSpacer(20);
	innerSizer->Add(titleRow, wxSizerFlags().Proportion(1).Expand());
	innerSizer->AddSpacer(10);
	innerSizer->Add(cmdRow, wxSizerFlags().Proportion(1).Expand());
	innerSizer->AddSpacer(10);
	innerSizer->Add(wdRow, wxSizerFlags().Proportion(1).Expand());
	innerSizer->AddSpacer(10);
	innerSizer->Add(btnSizer, wxSizerFlags().Center());


	outerSizer->Add(list, wxSizerFlags().Proportion(1).Expand());
	outerSizer->AddSpacer(20);
	outerSizer->Add(innerSizer, wxSizerFlags().Proportion(3).Expand());

	border->Add(outerSizer, wxSizerFlags().Border(wxALL, 20).Expand());
	panel->SetSizer(border);
	border->SetSizeHints(this);
}

void AppManager::OnClickOnList(wxCommandEvent& ev)
{
	if (list->GetStringSelection() != "<Add new>") {
		saveBtn->Enable(true);
		delBtn->Enable(true);
	}
	else {
		saveBtn->Enable(true);
		delBtn->Enable(false);
	}

	titleInp->Clear();
	cmdInp->Clear();
	wdInp->Clear();
	for (Shortcut s : shortcuts.shortcuts) {
		if (list->GetStringSelection() == s.title && list->GetStringSelection() != "<Add new>") {
			titleInp->SetValue(s.title);
			cmdInp->SetValue(s.command);
			wdInp->SetValue(s.wd);
		}
	}
}

void AppManager::UpdateList()
{
	list->Clear();
	if ((int)shortcuts.shortcuts.size() < 25)
		list->Append("<Add new>");

	for (Shortcut s : shortcuts.shortcuts) {
		list->Append(s.title);
	}
}

void AppManager::OnKeyDown(wxKeyEvent& ev)
{
	switch (ev.GetKeyCode()) {
	case WXK_ESCAPE:
		this->Close(true);
		break;
	case WXK_UP:
		SwapShortcuts(-1);
		break;
	case WXK_DOWN:
		SwapShortcuts(1);
		break;
	}
}

void AppManager::SwapShortcuts(short offset)
{
	if (list->GetStringSelection() == wxString("<Add new>")) return;

	short shift = (list->GetString(0) == wxString("<Add new>")) ? -1 : 0;
	int targetInd = list->GetSelection() + shift;
	int newInd = targetInd + offset;

	if (newInd >= 0 && newInd < list->GetCount() + shift) {

		Shortcut targetEl = shortcuts.shortcuts[targetInd];
		shortcuts.shortcuts[targetInd] = shortcuts.shortcuts[newInd];
		shortcuts.shortcuts[newInd] = targetEl;

		UpdateList();
		list->Select(newInd - shift);
	}
	else {
		return;
	}

}

bool AppManager::WriteShortcuts()
{
	UpdateList();

	json jsonObj = shortcuts;
	std::ofstream file("shortcuts.json");

	if (file.is_open()) {
		file << jsonObj.dump(4);
		file.close();
		return true;
	}
	else {
		return false;
	}

}

void AppManager::ClearInputs()
{
	saveBtn->Enable(false);
	delBtn->Enable(false);
	titleInp->Clear();
	cmdInp->Clear();
	wdInp->Clear();
}

void AppManager::SaveShortcut(wxCommandEvent& ev)
{
	if (!titleInp->GetValue()) {
		wxMessageBox("\"Title\" field cann't be empty. Please enter shortcut's title!", "Empty field", wxOK | wxICON_ERROR);
		return;
	}
	else if (!cmdInp->GetValue()) {
		wxMessageBox("\"Command\" field cann't be empty. Please enter shortcut's command!", "Empty field", wxOK | wxICON_ERROR);
		return;
	}

	if (list->GetStringSelection() != "<Add new>") {
		int id = list->GetSelection() - 1;
		shortcuts.shortcuts[id].title = std::string((titleInp->GetValue()).mb_str(wxConvUTF8));
		shortcuts.shortcuts[id].command = std::string((cmdInp->GetValue()).mb_str(wxConvUTF8));
		shortcuts.shortcuts[id].wd = std::string((wdInp->GetValue()).mb_str(wxConvUTF8));

		if (WriteShortcuts()) {
			ClearInputs();
			wxMessageBox(std::format("Successfully saved shortcut {}", shortcuts.shortcuts[id].title), "Success");
		}
		else {
			wxMessageBox("Failed to open \"shortcuts.json\"");
		}
	}
	else {
		Shortcut temp;
		temp.title = std::string((titleInp->GetValue()).mb_str(wxConvUTF8));
		temp.command = std::string((cmdInp->GetValue()).mb_str(wxConvUTF8));
		temp.wd = std::string((wdInp->GetValue()).mb_str(wxConvUTF8));
		shortcuts.shortcuts.push_back(temp);

		if (WriteShortcuts()) {
			ClearInputs();
			wxMessageBox(std::format("Successfully added shortcut {}", temp.title), "Success");
		}
		else {
			wxMessageBox("Failed to open \"shortcuts.json\"", "Error");
		}
	}

}

void AppManager::DeleteShortcut(wxCommandEvent& ev)
{
	short shift = (list->GetStringSelection() == wxString("<Add new>")) ? -1 : 0;
	int id = list->GetSelection() + shift;
	std::string title = shortcuts.shortcuts[id].title;
	shortcuts.shortcuts.erase(shortcuts.shortcuts.begin() + id);

	if (WriteShortcuts()) {
		ClearInputs();
		wxMessageBox(std::format("Successfully deleted shortcut {}", title), "Success");
	}
	else {
		wxMessageBox("Failed to open \"shortcuts.json\"", "Error");
	}
}

void AppManager::OnTitleEnter(wxCommandEvent& event)
{
	wxString value = titleInp->GetValue();

	const size_t maxLength = 30;
	if (value.Length() > maxLength) {
		titleInp->ChangeValue(value.SubString(0, maxLength - 1));
		titleInp->SetInsertionPointEnd();
	}
}

ShortcutsList AppManager::LoadShortcuts()
{
	std::ifstream file("shortcuts.json");

	std::stringstream buffer;
	buffer << file.rdbuf();

	json jsonObj = json::parse(buffer.str());

	ShortcutsList cmdLst = jsonObj.get<ShortcutsList>();

	return cmdLst;
}
