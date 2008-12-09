//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file splash_wnd.hpp
/// cockpit definition


#if !defined(__GUI_SPLASH_WND_HPP__)
#define __GUI_SPLASH_WND_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <atlmisc.h>


namespace gui {


class splash_wnd :
	public CWindowImpl<splash_wnd, CWindow, CWinTraits<WS_POPUP|WS_VISIBLE, WS_EX_TOOLWINDOW> >
{
private:
	enum
	{
		DEF_TIMER_ID		= 1001,
		DEF_TIMER_ELAPSE	= 2500,
	};
private:
	CBitmap m_bmp;
	int m_nTimeout;
	HWND m_hParent;
public:
	splash_wnd(UINT nBitmapID, int nTimeout = DEF_TIMER_ELAPSE, HWND hParent = NULL)
		: m_nTimeout(nTimeout)
		, m_hParent(hParent)
	{
		// Load the bitmap
		if (!m_bmp.LoadBitmap(nBitmapID))
		{
			ATLTRACE(_T("Failed to load spash bitmap\n"));
			return;
		}
		// Get the bitmap size
		CSize size;
		if (!m_bmp.GetSize(size))
		{
			ATLTRACE(_T("Failed to get spash bitmap size\n"));
			return;
		}
		// Create the window rect (we will centre the window later)
		CRect rect(0, 0, size.cx, size.cy);
		// Create the window
		if (!Create(NULL, rect))
		{
			ATLTRACE(_T("Failed to create splash window\n"));
			return;
		}
		UpdateWindow();
	}

	/// Called when the window is destroyed
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	BEGIN_MSG_MAP(splash_wnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnButtonDown)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(m_hParent);
		// Set the timer
		SetTimer(DEF_TIMER_ID, m_nTimeout);
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// Draw the bmp
		CPaintDC dc(m_hWnd);
		
		CDC dcImage;
		if (dcImage.CreateCompatibleDC(dc.m_hDC))
		{
			CSize size;
			if (m_bmp.GetSize(size))
			{
				HBITMAP hBmpOld = dcImage.SelectBitmap(m_bmp);
				dc.BitBlt(0, 0, size.cx, size.cy, dcImage, 0, 0, SRCCOPY);
				dcImage.SelectBitmap(hBmpOld);
			}
		}

		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		KillTimer(DEF_TIMER_ID);
		PostMessage(WM_CLOSE);
		return 0;
	}
	
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// No need to paint a background
		return TRUE;
	}

	LRESULT OnButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (wParam == VK_ESCAPE)
			PostMessage(WM_CLOSE);
		return 0;
	}
};


} //namespace gui


#endif //__COCKPIT_HPP__