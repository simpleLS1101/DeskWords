#pragma once

//////////////////////////////////////////////////////////////////////////
///

struct Data {
	wstring word;
	wstring means;
	wstring phrase;
	wstring spell;
	wstring example;
};

const string strpronunciation = "::pronunciation";
const string strmeans = "::means";
const string strphrase = "::phrase";
const string strexample = "::example";

extern bool isStop;

class CDeskWord : public WindowImplBase
{
public:
	CDeskWord(void);
	~CDeskWord(void);

public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI& msg);
	virtual void OnItemSelect(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
	{
		if (uMsg == WM_KEYDOWN)
		{
			switch (wParam)
			{
			case VK_ESCAPE:
				::PostQuitMessage(0L);
				break;
			case VK_SPACE:
				isStop = !isStop;
				break;
			default:
				break;
			}
		}
		return FALSE;
	}

	static unsigned  WINAPI MP3Proc(_In_ void* lpParameter);
private:
	CLabelUI*	m_pWord;
	CLabelUI*	m_pSpell;
	CTextUI*	m_pMeans;
	CTextUI*	m_pPhrase;
	CTextUI*	m_pExample;
	CTrayIcon	m_trayIcon;
};
