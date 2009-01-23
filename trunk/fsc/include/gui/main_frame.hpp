//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file main_frame.hpp
/// The main frame


#if !defined(__GUI_MAINFRAME_HPP__)
#define __GUI_MAINFRAME_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ATL headers are included in the precompiled header
#include "resource.h"
#include <gui/taskbar_notify_icon.hpp>


namespace gui {


#define WM_TASKBAR_NOTIFY WM_USER + 1


class main_frame :
    public WTL::CFrameWindowImpl<main_frame>, 
    public WTL::CUpdateUI<main_frame>,
	public WTL::CMessageFilter, 
	public WTL::CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(main_frame)

	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(main_frame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_TASKBAR_NOTIFY, OnTaskbarNotification)        
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnExit)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    LRESULT OnTaskbarNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);

private:
    taskbar_notify_icon tni_;
    HICON icon;
};


} //namespace gui

#endif //__GUI_MAINFRAME_HPP__