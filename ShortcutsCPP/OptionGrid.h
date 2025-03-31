#pragma once
#include <wx/wx.h>
#include <vector>

//class TextWithAFrame : public wxStaticText {
//public:
//	TextWithAFrame(wxWindow* parent, wxWindowID id, const wxString& label)
//		: wxStaticText(parent, id, label), showBorder(false) {
//		Bind(wxEVT_PAINT, &TextWithAFrame::OnPaint, this); // Привязываем событие рисования
//	}
//
//	// Метод для включения/выключения рамки
//	void ShowBorder(bool enable) {
//		showBorder = enable;
//		Refresh(); // Перерисовываем виджет
//	}
//
//protected:
//	void OnPaint(wxPaintEvent& event) {
//		wxPaintDC dc(this); // Создаём контекст рисования
//
//		if (showBorder) {
//
//
//			// Получаем текущую область виджета
//			wxRect rect = GetClientRect();
//
//			// Увеличиваем область рамки
//			rect.Inflate(10, 10); // Увеличиваем размер рамки на 10 пикселей со всех сторон
//
//			// Настраиваем параметры рамки
//			dc.SetPen(wxPen(*wxWHITE, 2)); // Белая рамка толщиной 2px
//			dc.SetBrush(*wxTRANSPARENT_BRUSH); // Прозрачный фон внутри рамки
//
//			// Рисуем увеличенную рамку
//			dc.DrawRectangle(rect);
//
//			// Вызов стандартной отрисовки текста
//		}
//			wxStaticText::OnPaint(event);
//	}
//
//
//private:
//	bool showBorder; // Флаг для отображения рамки
//
//};

class OptGrid;

class Option {
public:
	Option(wxPanel* window, wxGridSizer* parent, OptGrid* grid, short col, short row, std::string title, std::string command, std::string wd);
	void BindExecuting(HWND previousForegroundWindow);
	void OnEnter(wxMouseEvent& ev);
	void Select(bool state);

	wxWindow* window;

	wxBoxSizer* outerSizer;
	wxPanel* backgroundPanel;
	wxBoxSizer* outerPanelSizer;
	wxBoxSizer* innerPanelSizer;
	wxPanel* innerPanel;
	wxBoxSizer* innerPanelInnerSizer;
	wxStaticText* text;

	OptGrid* grid;
	std::string title;
	std::string command;
	std::string wd;
	short col;
	short row;
	boolean isSelected;
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

