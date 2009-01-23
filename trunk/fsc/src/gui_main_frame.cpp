//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


#include "precompiled_header.h"
#include <gui/main_frame.hpp>


using namespace gui;


BOOL main_frame::PreTranslateMessage(MSG* pMsg)
{
	return CFrameWindowImpl<main_frame>::PreTranslateMessage(pMsg);
}

BOOL main_frame::OnIdle()
{
	return FALSE;
}


LRESULT main_frame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    WTL::CIcon icon(AtlLoadIcon(IDR_MAINFRAME));

    //Since we are creating the first tray icon initially hidden
    UIEnable(SW_SHOW, TRUE);

	// Register object for message filtering and idle updates
    WTL::CMessageLoop* loop = _Module.GetMessageLoop();
	ATLASSERT(loop);
	loop->AddMessageFilter(this);
	loop->AddIdleHandler(this);

    if (! tni_.Create(this, IDR_MAINFRAME, _T("Static Tray Icon"), _T("Demo Tray Application"), _T("Static Tray Icon"), 10, taskbar_notify_icon::User, icon, WM_TASKBAR_NOTIFY, 0, TRUE, FALSE, FALSE, NULL, FALSE)) //For testing purposes initally creating a tray icon hidden!
    //if (!m_TrayIcon1.Create(this, IDR_TRAYPOPUP, _T("First Tray Icon"), m_hIcons[0], WM_TRAYNOTIFY)) //Uncomment this line and comment out line above if you want to start with a tray icon without animations
    {
      ATLTRACE(_T("Failed to create tray icon 1\n"));
      return -1;
    }

    return 0;
}

LRESULT main_frame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    return 0;
}

LRESULT main_frame::OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    return 0;
}

LRESULT main_frame::OnTaskbarNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
    //Delegate all the work back to the default implementation in taskbar_notify_icon
    tni_.OnTrayNotification(wParam, lParam);

    return 0;
}