//  (c) Copyright 2008 Samuel Debionne.
//
//  Distributed under the MIT Software License. (See accompanying file
//  license.txt) or copy at http://www.opensource.org/licenses/mit-license.php)
//
//  See http://code.google.com/p/fsc-sdk/ for the library home page.
//
//	$Revision: $
//	$History: $


/// \file taskbar_notify_icon.hpp
/// Windows taskbar notify icon


#if !defined(__GUI_TASKBAR_NOTIFY_ICON_HPP__)
#define __GUI_TASKBAR_NOTIFY_ICON_HPP__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include <Windows.h>
#include <ShellApi.h>
#include <atlmisc.h>


namespace gui {


//the actual tray notification class wrapper
class taskbar_notify_icon : public CWindowImpl<taskbar_notify_icon>
{
public:
    enum BalloonStyle
    {
        Warning,
        Error,
        Info,
        None,
        User
    };

    typedef WTL::CString tooltip_string;

    DECLARE_WND_CLASS(_T("TrayIconClass"))

    //Constructors / Destructors
    taskbar_notify_icon();
    virtual ~taskbar_notify_icon();

    //Create the tray icon
    BOOL Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON hIcon, UINT nNotifyMessage, UINT uMenuID = 0, BOOL bShow = TRUE);
    BOOL Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, CBitmap* pBitmap, UINT nNotifyMessage, UINT uMenuID = 0, BOOL bShow = TRUE);
    BOOL Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON* phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage, UINT uMenuID = 0, BOOL bShow = TRUE);
    BOOL Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, UINT nTimeout, BalloonStyle style, HICON hIcon, UINT nNotifyMessage, UINT uMenuID = 0, BOOL bNoSound = FALSE, BOOL bLargeIcon = FALSE, BOOL bRealtime = FALSE, HICON hBalloonIcon = NULL, BOOL bShow = TRUE);                  
    BOOL Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, UINT nTimeout, BalloonStyle style, CBitmap* pBitmap, UINT nNotifyMessage, UINT uMenuID = 0, BOOL bNoSound = FALSE, BOOL bLargeIcon = FALSE, BOOL bRealtime = FALSE, HICON hBalloonIcon = NULL, BOOL bShow = TRUE);
    BOOL Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, UINT nTimeout, BalloonStyle style, HICON* phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage, UINT uMenuID = 0, BOOL bNoSound = FALSE, BOOL bLargeIcon = FALSE, BOOL bRealtime = FALSE, HICON hBalloonIcon = NULL, BOOL bShow = TRUE);

    //Sets or gets the Tooltip text
    BOOL SetTooltipText(LPCTSTR pszTooltipText);
    BOOL SetTooltipText(UINT nID);
    tooltip_string GetTooltipText() const;

    //Sets or gets the icon displayed
    BOOL SetIcon(HICON hIcon);
    BOOL SetIcon(CBitmap* pBitmap);
    BOOL SetIcon(LPCTSTR lpIconName);
    BOOL SetIcon(UINT nIDResource);
    BOOL SetIcon(HICON* phIcons, int nNumIcons, DWORD dwDelay);
    BOOL SetStandardIcon(LPCTSTR lpIconName);
    BOOL SetStandardIcon(UINT nIDResource);
    HICON GetIcon() const;
    BOOL UsingAnimatedIcon() const;

    //Sets or gets the window to send notification messages to
    BOOL SetNotificationWnd(CWindow* pNotifyWnd);
    CWindow* GetNotificationWnd() const;

    //Modification of the tray icons
    BOOL Delete(BOOL bCloseHelperWindow = TRUE);
    BOOL Create(BOOL bShow = TRUE);
    BOOL Hide();
    BOOL Show();

    //Dynamic tray icon support
    HICON BitmapToIcon(CBitmap* pBitmap);
    static BOOL GetDynamicDCAndBitmap(CDC* pDC, CBitmap* pBitmap);

    //Modification of the menu to use as the context menu
    void SetMenu(HMENU hMenu);
    CMenu& GetMenu();
    void SetDefaultMenuItem(UINT uItem, BOOL fByPos);
    void GetDefaultMenuItem(UINT& uItem, BOOL& fByPos) { uItem = m_nDefaultMenuItem; fByPos = m_bDefaultMenuItemByPos; };

    //Default handler for tray notification message
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

    //Status information
    BOOL IsShowing() const { return !IsHidden(); };
    BOOL IsHidden() const { return m_bHidden; };

    //Sets or gets the Balloon style tooltip settings
    BOOL                  SetBalloonDetails(LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, BalloonStyle style, UINT nTimeout, HICON hUserIcon = NULL, BOOL bNoSound = FALSE, BOOL bLargeIcon = FALSE, BOOL bRealtime = FALSE, HICON hBalloonIcon = NULL);
    tooltip_string GetBalloonText() const;
    tooltip_string GetBalloonCaption() const;
    BalloonStyle          GetBalloonStyle() const;
    UINT                  GetBalloonTimeout() const;

    //Other functionality
    BOOL SetVersion(UINT uVersion);
    BOOL SetFocus();

    //Helper functions to load tray icon from resources
    static HICON LoadIcon(LPCTSTR lpIconName, BOOL bLargeIcon = FALSE);
    static HICON LoadIcon(UINT nIDResource, BOOL bLargeIcon = FALSE);
    static HICON LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName, BOOL bLargeIcon = FALSE);
    static HICON LoadIcon(HINSTANCE hInstance, UINT nIDResource, BOOL bLargeIcon = FALSE);

protected:
    //Methods
    BOOL         CreateHelperWindow();
    void         StartAnimation(HICON* phIcons, int nNumIcons, DWORD dwDelay);
    void         StopAnimation();
    HICON        GetCurrentAnimationIcon() const;
    virtual BOOL ProcessWindowMessage(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID);
    LRESULT      OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
    void         OnTimer(UINT_PTR nIDEvent);
    void         OnDestroy();

    //Enums
    enum ShellVersion
    {
        Version4     = 0, //PreWin2k
        Version5     = 1, //Win2k
        Version6     = 2, //XP
        VersionVista = 3  //Vista
    };

    //Member variables
    NOTIFYICONDATA   m_NotifyIconData;
    BOOL             m_bCreated;
    BOOL             m_bHidden;
    CWindow*         m_pNotificationWnd;
    CMenu            m_Menu;
    UINT             m_nDefaultMenuItem;
    BOOL             m_bDefaultMenuItemByPos;
    ShellVersion     m_ShellVersion;
    HICON            m_hDynamicIcon; //Our cached copy of the last icon created with BitmapToIcon
    HICON*           m_phIcons;
    int              m_nNumIcons;
    UINT_PTR         m_nTimerID;
    int              m_nCurrentIconIndex;
};


} //namespace gui

#endif //__GUI_TASKBAR_NOTIFY_ICON_HPP__
