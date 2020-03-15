#include "StdAfx.h"
#include "resource.h"
#include "deskword.h"
#include <ShellAPI.h>
//////////////////////////////////////////////////////////////////////////
///
#include <process.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <time.h>
#include "words.h"
#include <list>
#include <set>
#include <string>
#include <fstream>

//these two headers are already included in the <Windows.h> header

DUI_BEGIN_MESSAGE_MAP(CDeskWord, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_END_MESSAGE_MAP()

wstring flushWord;
wstring flushSpell;
wstring flushMeans;
wstring flushPhrase;
wstring flushExample;

// 是否暂停
bool isStop = false;

CDeskWord::CDeskWord(void)
{
}

CDeskWord::~CDeskWord(void)
{
}

void CDeskWord::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CDeskWord::GetSkinFile()
{
	return _T("popup.xml");
}

LPCTSTR CDeskWord::GetWindowClassName(void) const
{
	return _T("PopWnd");
}

void CDeskWord::OnClick(TNotifyUI& msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();

	//if (msg.pSender == m_pCloseBtn) {
	//	Close(0);
	//	return;
	//}
	//else if (msg.pSender == m_pMinBtn) {
	//	//CMsgWnd::MessageBox(m_hWnd, NULL, _T("子子窗口"));
	//	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
	//}
	//else if (msg.pSender == m_pMaxBtn) {
	//	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return;
	//}
	//else if (msg.pSender == m_pRestoreBtn) {
	//	SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return;
	//}
	//else if (msg.pSender == m_pMenuBtn) {
	//}
	//else if (sName.CompareNoCase(_T("homepage_btn")) == 0)
	//{
	//	ShellExecute(NULL, _T("open"), _T("https://github.com/qdtroy"), NULL, NULL, SW_SHOW);
	//}
}

void CDeskWord::OnSelectChanged(TNotifyUI& msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();

}

void CDeskWord::OnItemSelect(TNotifyUI& msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
}

LRESULT CDeskWord::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_FLUSH)
	{
		m_pWord->SetText(flushWord.data());
		m_pSpell->SetText(flushSpell.data());
		m_pMeans->SetText(flushMeans.data());
		m_pPhrase->SetText(flushPhrase.data());
		m_pExample->SetText(flushExample.data());
	}
	else if (uMsg == UIMSG_TRAYICON) {
		UINT uIconMsg = (UINT)lParam;
		if (uIconMsg == WM_LBUTTONDBLCLK) {
			::ShowWindow(m_hWnd,  SW_SHOW );
		}
	}
	// 关闭窗口，退出程序
	else if (uMsg == WM_DESTROY)
	{
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	
	return 0;
}

void CDeskWord::Notify(TNotifyUI& msg)
{
	return WindowImplBase::Notify(msg);
}

LRESULT CDeskWord::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if (wParam == SC_CLOSE) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(*this) != bZoomed) {
		if (!bZoomed) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if (pControl) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if (pControl) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if (pControl) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if (pControl) pControl->SetVisible(false);
		}
	}
	return lRes;
}

void CDeskWord::InitWindow()
{
	SetIcon(IDR_ICON);

	m_pWord = static_cast<CLabelUI*>(m_pm.FindControl(_T("word")));
	m_pSpell = static_cast<CLabelUI*>(m_pm.FindControl(_T("spell")));
	m_pMeans = static_cast<CTextUI*>(m_pm.FindControl(_T("mean")));
	m_pPhrase = static_cast<CTextUI*>(m_pm.FindControl(_T("phrase")));
	m_pExample = static_cast<CTextUI*>(m_pm.FindControl(_T("example")));
	// 自动换行
	if (m_pMeans)
	{
		DWORD dwStyle = m_pMeans->GetTextStyle();
		m_pMeans->SetTextStyle(dwStyle | DT_EDITCONTROL);
	}
	if (m_pPhrase)
	{
		DWORD dwStyle = m_pPhrase->GetTextStyle();
		m_pPhrase->SetTextStyle(dwStyle | DT_EDITCONTROL);
	}
	if (m_pExample)
	{
		DWORD dwStyle = m_pExample->GetTextStyle();
		m_pExample->SetTextStyle(dwStyle | DT_EDITCONTROL);
	}
	unsigned uiThread1ID = 0;
	_beginthreadex(NULL,         // security
		0,            // stack size
		CDeskWord::MP3Proc,
		m_hWnd,           // arg list
		0,  // so we can later call ResumeThread()
		&uiThread1ID);

	m_trayIcon.CreateTrayIcon(m_hWnd, IDR_ICON, _T("桌面记"));
}



unsigned WINAPI CDeskWord::MP3Proc(_In_ void* lpParameter) //lpParameter can be a pointer to a structure that store data that you cannot access outside of this function. You can prepare this structure before `CreateThread` and give it's address in the `lpParameter`
{
	HWND hwnd = (HWND)lpParameter;
	if (hwnd == NULL) {
		::MessageBox(NULL, L"初始化失败请重启！", L"error", MB_OK);
		exit(0);
		return 0;
	}

	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);

	wstring strPath = exeFullPath;
	int pos = strPath.rfind(L"\\");
	if (pos == wstring::npos) {
		::MessageBox(NULL, L"初始化路径失败请重启！", L"error", MB_OK);
		exit(0);
		return 0;
	}
	strPath = strPath.substr(0, pos+1);
	list<Data> listData;
	set<wstring> rwords;
	srand((int)time(0));
	
	pos = 0;
	for (int i = 0; i < 100; i++) {
		pos = rand() % 3343;
		rwords.insert(const_words4[pos]);
		if (rwords.size() == 30) {
			break;
		}
	}
	
	for (auto wstr : rwords)
	{
	
		ifstream fin;
		fin.open(strPath+L"/newmeans/"+wstr+L".txt");
		string line;
		Data data = Data{};
		data.word = wstr;
		string state = strpronunciation;
		while (getline(fin, line))
		{
			if (line.find(strpronunciation) != -1) {
				continue;
			}
			if (line.find(strmeans) !=-1)
			{
				state = strmeans;
				continue;
			}
			else if (line.find(strphrase) !=-1) 
			{
				state = strphrase;
				continue;
			}
			else if (line.find(strexample) != -1)
			{
				state = strexample;
				continue;
			}

			int len = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), line.size(), NULL, 0);
			WCHAR* buf = new WCHAR[len+1];
			MultiByteToWideChar(CP_UTF8, 0, line.c_str(), line.size(), buf, len);
			buf[len] = L'\0';
			if (state.compare(strpronunciation) == 0) 
			{
				data.spell += buf;
			}
			else if (state.compare(strmeans) == 0)
			{
				data.means += buf;
			}
			else if (state.compare(strphrase) == 0)
			{
				data.phrase += buf;
			}
			else if (state.compare(strexample) == 0)
			{
				data.example += buf;
			}
			delete[] buf;
		}
		listData.push_back(data);
		fin.close();
	}


	while (true) {
		Sleep(200);
		while (isStop)
		{
			Sleep(100);
		}
		for (auto item : listData) {
			flushWord = item.word;
			flushMeans = item.means;
			flushSpell = item.spell;
			flushPhrase = item.phrase;
			flushExample = item.example;
			::SendMessage(hwnd, WM_FLUSH, NULL, NULL);
			wstring mp3 = L"open "+strPath+L"/mp3/" + item.word + L".mp3 type mpegvideo alias aa";
			mciSendString(mp3.data(), NULL, 0, NULL);
			mciSendString(L"play aa wait ", NULL, 0, NULL);
			//Data* data = (Data*)lpParameter; //If you call this structure Data, but you can call it whatever you want.
			for (int i = 0; i < 4;i++)
			{
				while (isStop)
				{
					Sleep(100);
				}
				mciSendString(L"play aa from 0 wait", NULL, 0, NULL);
				Sleep(400);
				//Do here what you want to do when the mp3 playback is over
				//SuspendThread(GetCurrentThread()); //or the handle of this thread that you keep in a static variable instead
			}

			mciSendString(L"close aa", NULL, 0, NULL);
			}
	}

	
	
	
	return 0;
}
