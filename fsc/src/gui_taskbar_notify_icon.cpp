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
#include <gui/taskbar_notify_icon.hpp>


using namespace gui;


const UINT WM_TASKBAR_CREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));


taskbar_notify_icon::taskbar_notify_icon() :
    m_bCreated(FALSE),
    m_bHidden(FALSE),
    m_pNotificationWnd(NULL),
    m_bDefaultMenuItemByPos(TRUE),
    m_nDefaultMenuItem(0),
    m_hDynamicIcon(NULL),
    m_ShellVersion(Version4), //Assume version 4 of the shell
    m_phIcons(NULL),
    m_nNumIcons(0),
    m_nTimerID(0),
    m_nCurrentIconIndex(0)
{
    typedef HRESULT (CALLBACK DLLGETVERSION)(DLLVERSIONINFO*);
    typedef DLLGETVERSION* LPDLLGETVERSION;

    //check to see if we are running on Vista
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    if (GetVersionEx(&osvi) && osvi.dwMajorVersion >= 6)
        m_ShellVersion = VersionVista;
    else
    {
        //Try to get the details with DllGetVersion
        HMODULE hShell32 = GetModuleHandle(_T("SHELL32.DLL"));
        if (hShell32)
        {
            LPDLLGETVERSION lpfnDllGetVersion = reinterpret_cast<LPDLLGETVERSION>(GetProcAddress(hShell32, "DllGetVersion"));
            if (lpfnDllGetVersion)
            {
                DLLVERSIONINFO vinfo;
                vinfo.cbSize = sizeof(DLLVERSIONINFO);
                if (SUCCEEDED(lpfnDllGetVersion(&vinfo)))
                {
                    if (vinfo.dwMajorVersion >= 6)
                        m_ShellVersion = Version6;
                    else if (vinfo.dwMajorVersion >= 5)
                        m_ShellVersion = Version5;
                }
            }
        }
    }

    memset(&m_NotifyIconData, 0, sizeof(m_NotifyIconData));
    m_NotifyIconData.cbSize = sizeof(m_NotifyIconData);
}

taskbar_notify_icon::~taskbar_notify_icon()
{
    //Delete the tray icon
    Delete(TRUE);

    //Free up any dynamic icon we may have
    if (m_hDynamicIcon)
    {
        DestroyIcon(m_hDynamicIcon);
        m_hDynamicIcon = NULL;
    }
}

BOOL taskbar_notify_icon::Delete(BOOL bCloseHelperWindow)
{
    //What will be the return value from this function (assume the best)
    BOOL bSuccess = TRUE;

    if (m_bCreated)
    {
        m_NotifyIconData.uFlags = 0;
        bSuccess = Shell_NotifyIcon(NIM_DELETE, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
        m_bCreated = FALSE;
    }

    //Close the helper window if requested to do so
    if (bCloseHelperWindow)
        SendMessage(WM_CLOSE);

    return bSuccess;
}

BOOL taskbar_notify_icon::Create(BOOL bShow)
{
    m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    if (bShow == FALSE)
    {
        ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
        m_NotifyIconData.uFlags |= NIF_STATE;
        m_NotifyIconData.dwState = NIS_HIDDEN;
        m_NotifyIconData.dwStateMask = NIS_HIDDEN;
    }

    BOOL bSuccess = Shell_NotifyIcon(NIM_ADD, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
    if (bSuccess)
    {
        m_bCreated = TRUE;

        if (bShow == FALSE)
            m_bHidden = TRUE;
    }
    return bSuccess;
}

BOOL taskbar_notify_icon::Hide()
{
    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
    ATLASSERT(!m_bHidden); //Only makes sense to hide the icon if it is not already hidden

    m_NotifyIconData.uFlags = NIF_STATE;
    m_NotifyIconData.dwState = NIS_HIDDEN;
    m_NotifyIconData.dwStateMask = NIS_HIDDEN; 
    BOOL bSuccess = Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
    if (bSuccess)
        m_bHidden = TRUE;
    return bSuccess;
}

BOOL taskbar_notify_icon::Show()
{
    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
    ATLASSERT(m_bHidden); //Only makes sense to show the icon if it has been previously hidden

    ATLASSERT(m_bCreated);
    m_NotifyIconData.uFlags = NIF_STATE;
    m_NotifyIconData.dwState = 0;
    m_NotifyIconData.dwStateMask = NIS_HIDDEN;
    BOOL bSuccess = Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
    if (bSuccess)
        m_bHidden = FALSE;
    return bSuccess;
}

void taskbar_notify_icon::SetMenu(HMENU hMenu)
{
    //Validate our parameters
    ATLASSERT(hMenu);

    m_Menu.DestroyMenu();
    m_Menu.Attach(hMenu);

    CMenuHandle subMenu = m_Menu.GetSubMenu(0);
    ATLASSERT(subMenu.IsMenu()); //Your menu resource has been designed incorrectly

    //Make the specified menu item the default (bold font)
    subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
}

CMenu& taskbar_notify_icon::GetMenu()
{
    return m_Menu;
}

void taskbar_notify_icon::SetDefaultMenuItem(UINT uItem, BOOL fByPos) 
{ 
    m_nDefaultMenuItem = uItem; 
    m_bDefaultMenuItemByPos = fByPos; 

    //Also update in the live menu if it is present
    if (m_Menu.operator HMENU())
    {
        CMenuHandle subMenu = m_Menu.GetSubMenu(0);
        ATLASSERT(subMenu.IsMenu()); //Your menu resource has been designed incorrectly

        subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);
    }
}

BOOL taskbar_notify_icon::Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON hIcon, UINT nNotifyMessage, UINT uMenuID, BOOL bShow)
{
    //Validate our parameters
    ATLASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->operator HWND()));
#ifndef NDEBUG  
    if (m_ShellVersion >= Version5) //If on Shell v5 or higher, then use the larger size tooltip
    {
        NOTIFYICONDATA dummy;
        ATLASSERT(_tcslen(pszTooltipText) < sizeof(dummy.szTip)/sizeof(TCHAR));
        DBG_UNREFERENCED_LOCAL_VARIABLE(dummy);
    }
    else
    {
        NOTIFYICONDATA dummy;
        ATLASSERT(_tcslen(pszTooltipText) < sizeof(dummy.szTip)/sizeof(TCHAR));
        DBG_UNREFERENCED_LOCAL_VARIABLE(dummy);
    }
#endif
    ATLASSERT(hIcon); 
    ATLASSERT(nNotifyMessage >= WM_USER); //Make sure we avoid conflict with other messages

    //Load up the menu resource which is to be used as the context menu
    if (!m_Menu.LoadMenu(uMenuID == 0 ? uID : uMenuID))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    CMenuHandle subMenu = m_Menu.GetSubMenu(0);
    if (!subMenu.IsMenu())
    {
        ATLASSERT(FALSE); //Your menu resource has been designed incorrectly
        return FALSE;
    }
    subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);

    //Create the helper window
    if (!CreateHelperWindow())
        return FALSE;

    //Call the Shell_NotifyIcon function
    m_pNotificationWnd = pNotifyWnd;
    m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_NotifyIconData.hWnd = pNotifyWnd->operator HWND();
    m_NotifyIconData.uID = uID;
    m_NotifyIconData.uCallbackMessage = nNotifyMessage;
    m_NotifyIconData.hIcon = hIcon;
    _tcscpy_s(m_NotifyIconData.szTip, sizeof(m_NotifyIconData.szTip)/sizeof(TCHAR), pszTooltipText);

    if (bShow == FALSE)
    {
        ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
        m_NotifyIconData.uFlags |= NIF_STATE;
        m_NotifyIconData.dwState = NIS_HIDDEN;
        m_NotifyIconData.dwStateMask = NIS_HIDDEN;
    }
    
    m_bCreated = Shell_NotifyIcon(NIM_ADD, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
    if (m_bCreated)
    {
        if (bShow == FALSE)
            m_bHidden = TRUE;

        //Turn on Shell v5 style behaviour if supported
        if (m_ShellVersion >= Version5)
            SetVersion(NOTIFYICON_VERSION);
    }

    return m_bCreated;
}

BOOL taskbar_notify_icon::SetVersion(UINT uVersion)
{
    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later

    //Call the Shell_NotifyIcon function
    m_NotifyIconData.uVersion = uVersion;
    return Shell_NotifyIcon(NIM_SETVERSION, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

HICON taskbar_notify_icon::BitmapToIcon(CBitmap* pBitmap)
{
    //Validate our parameters
    ATLASSERT(pBitmap);

    //Get the width and height of a small icon
    int w = GetSystemMetrics(SM_CXSMICON);
    int h = GetSystemMetrics(SM_CYSMICON);

    //Create a 0 mask
    int nMaskSize = h*(w/8);
    unsigned char* pMask = new unsigned char[nMaskSize];
    memset(pMask, 0, nMaskSize);

    //Create a mask bitmap
    CBitmap maskBitmap;
    maskBitmap.CreateBitmap(w, h, 1, 1, pMask);
    BOOL bSuccess = !maskBitmap.IsNull();

    //Free up the heap memory now that we have created the mask bitmap
    delete [] pMask;

    //Handle the error
    if (!bSuccess)
        return NULL;

    //Create an ICON base on the bitmap just created
    ICONINFO iconInfo;
    iconInfo.fIcon = TRUE;
    iconInfo.xHotspot = 0;
    iconInfo.yHotspot = 0;
    iconInfo.hbmMask = maskBitmap;
    iconInfo.hbmColor = *pBitmap; 
    return CreateIconIndirect(&iconInfo); 
}

BOOL taskbar_notify_icon::Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, CBitmap* pBitmap, UINT nNotifyMessage, UINT uMenuID, BOOL bShow)
{
    //Convert the bitmap to an Icon
    if (m_hDynamicIcon)
        DestroyIcon(m_hDynamicIcon);
    m_hDynamicIcon = BitmapToIcon(pBitmap);

    //Pass the buck to the other function to do the work
    return Create(pNotifyWnd, uID, pszTooltipText, m_hDynamicIcon, nNotifyMessage, uMenuID, bShow);
}

BOOL taskbar_notify_icon::Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, HICON* phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage, UINT uMenuID, BOOL bShow)
{
    //Validate our parameters
    ATLASSERT(phIcons);
    ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
    ATLASSERT(dwDelay);

    //let the normal Create function do its stuff
    BOOL bSuccess = Create(pNotifyWnd, uID, pszTooltipText, phIcons[0], nNotifyMessage, uMenuID, bShow);
    if (bSuccess)
    {
        //Start the animation
        StartAnimation(phIcons, nNumIcons, dwDelay);
    }

    return bSuccess;
}

BOOL taskbar_notify_icon::Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, UINT nTimeout, BalloonStyle style, HICON hIcon, UINT nNotifyMessage, UINT uMenuID, BOOL bNoSound, BOOL bLargeIcon, BOOL bRealtime, HICON hBalloonIcon, BOOL bShow)
{
    //Validate our parameters
    ATLASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->operator HWND()));
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
#ifndef NDEBUG
    NOTIFYICONDATA dummy;
    DBG_UNREFERENCED_LOCAL_VARIABLE(dummy);
    ATLASSERT(_tcslen(pszTooltipText) < sizeof(dummy.szTip)/sizeof(TCHAR));
    ATLASSERT(_tcslen(pszBalloonText) < sizeof(dummy.szInfo)/sizeof(TCHAR));
    ATLASSERT(_tcslen(pszBalloonCaption) < sizeof(dummy.szInfoTitle)/sizeof(TCHAR));
    ATLASSERT(hIcon); 
    ATLASSERT(nNotifyMessage >= WM_USER); //Make sure we avoid conflict with other messages
#endif

    //Load up the menu resource which is to be used as the context menu
    if (!m_Menu.LoadMenu(uMenuID == 0 ? uID : uMenuID))
    {
        ATLASSERT(FALSE);
        return FALSE;
    }

    CMenuHandle subMenu = m_Menu.GetSubMenu(0);
    if (!subMenu.IsMenu()) 
    {
        ATLASSERT(FALSE); //Your menu resource has been designed incorrectly
        return FALSE;
    }
    //Make the specified menu item the default (bold font)
    subMenu.SetMenuDefaultItem(m_nDefaultMenuItem, m_bDefaultMenuItemByPos);

    //Create the helper window
    if (!CreateHelperWindow())
        return FALSE;

    //Call the Shell_NotifyIcon function
    m_pNotificationWnd = pNotifyWnd;
    m_NotifyIconData.hWnd = pNotifyWnd->operator HWND();
    m_NotifyIconData.uID = uID;
    m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    m_NotifyIconData.uCallbackMessage = nNotifyMessage;
    m_NotifyIconData.hIcon = hIcon;
    _tcscpy_s(m_NotifyIconData.szTip, sizeof(m_NotifyIconData.szTip)/sizeof(TCHAR), pszTooltipText);
    _tcscpy_s(m_NotifyIconData.szInfo, sizeof(m_NotifyIconData.szInfo)/sizeof(TCHAR), pszBalloonText);
    _tcscpy_s(m_NotifyIconData.szInfoTitle, sizeof(m_NotifyIconData.szInfoTitle)/sizeof(TCHAR), pszBalloonCaption);
    m_NotifyIconData.uTimeout = nTimeout;
    switch (style)
    {
    case Warning:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_WARNING;
            break;
        }
    case Error:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_ERROR;
            break;
        }
    case Info:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_INFO;
            break;
        }
    case None:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_NONE;
            break;
        }
    case User:
        {
            if (hBalloonIcon)
            {
                ATLASSERT(m_ShellVersion >= VersionVista);
                m_NotifyIconData.hBalloonIcon = hBalloonIcon;
            }
            else
            {
                ATLASSERT(hIcon != NULL); //You forget to provide a user icon
            }
            m_NotifyIconData.dwInfoFlags = NIIF_USER;
            break;
        }
    default:
        {
            ATLASSERT(FALSE);
            break;
        }
    }
    if (bNoSound)
        m_NotifyIconData.dwInfoFlags |= NIIF_NOSOUND;
    if (bLargeIcon)
    {
        ATLASSERT(m_ShellVersion >= VersionVista); //Only supported on Vista Shell
        m_NotifyIconData.dwInfoFlags |= NIIF_LARGE_ICON;
    }
    if (bRealtime)
    {
        ATLASSERT(m_ShellVersion >= VersionVista); //Only supported on Vista Shell
        m_NotifyIconData.uFlags |= NIF_REALTIME;
    }
    if (bShow == FALSE)
    {
        ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
        m_NotifyIconData.uFlags |= NIF_STATE;
        m_NotifyIconData.dwState = NIS_HIDDEN;
        m_NotifyIconData.dwStateMask = NIS_HIDDEN;
    }
    m_bCreated = Shell_NotifyIcon(NIM_ADD, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
    if (m_bCreated)
    {
        if (bShow == FALSE)
            m_bHidden = TRUE;

        //Turn on Shell v5 tray icon behaviour
        SetVersion(NOTIFYICON_VERSION);
    }

    return m_bCreated;
}

BOOL taskbar_notify_icon::Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, UINT nTimeout, BalloonStyle style, CBitmap* pBitmap, UINT nNotifyMessage, UINT uMenuID, BOOL bNoSound, BOOL bLargeIcon, BOOL bRealtime, HICON hBalloonIcon, BOOL bShow)
{
    //Convert the bitmap to an ICON
    if (m_hDynamicIcon)
        DestroyIcon(m_hDynamicIcon);
    m_hDynamicIcon = BitmapToIcon(pBitmap);

    //Pass the buck to the other function to do the work
    return Create(pNotifyWnd, uID, pszTooltipText, pszBalloonText, pszBalloonCaption, nTimeout, style, m_hDynamicIcon, nNotifyMessage, uMenuID, bNoSound, bLargeIcon, bRealtime, hBalloonIcon, bShow);
}

BOOL taskbar_notify_icon::Create(CWindow* pNotifyWnd, UINT uID, LPCTSTR pszTooltipText, LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, UINT nTimeout, BalloonStyle style, HICON* phIcons, int nNumIcons, DWORD dwDelay, UINT nNotifyMessage, UINT uMenuID, BOOL bNoSound, BOOL bLargeIcon, BOOL bRealtime, HICON hBalloonIcon, BOOL bShow)
{
    //Validate our parameters
    ATLASSERT(phIcons);
    ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
    ATLASSERT(dwDelay);

    //let the normal Create function do its stuff
    BOOL bSuccess = Create(pNotifyWnd, uID, pszTooltipText, pszBalloonText, pszBalloonCaption, nTimeout, style, phIcons[0], nNotifyMessage, uMenuID, bNoSound, bLargeIcon, bRealtime, hBalloonIcon, bShow);
    if (bSuccess)
    {
        //Start the animation
        StartAnimation(phIcons, nNumIcons, dwDelay);
    }

    return bSuccess;
}

BOOL taskbar_notify_icon::SetBalloonDetails(LPCTSTR pszBalloonText, LPCTSTR pszBalloonCaption, BalloonStyle style, UINT nTimeout, HICON hUserIcon, BOOL bNoSound, BOOL bLargeIcon, BOOL bRealtime, HICON hBalloonIcon)
{
    if (!m_bCreated)
        return FALSE;

    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later
#ifndef NDEBUG
    NOTIFYICONDATA dummy;
    DBG_UNREFERENCED_LOCAL_VARIABLE(dummy);
    ATLASSERT(_tcslen(pszBalloonText) < sizeof(dummy.szInfo)/sizeof(TCHAR));
    ATLASSERT(_tcslen(pszBalloonCaption) < sizeof(dummy.szInfoTitle)/sizeof(TCHAR));
#endif

    //Call the Shell_NotifyIcon function
    m_NotifyIconData.uFlags = NIF_INFO;
    _tcscpy_s(m_NotifyIconData.szInfo, sizeof(m_NotifyIconData.szInfo)/sizeof(TCHAR), pszBalloonText);
    _tcscpy_s(m_NotifyIconData.szInfoTitle, sizeof(m_NotifyIconData.szInfoTitle)/sizeof(TCHAR), pszBalloonCaption);
    m_NotifyIconData.uTimeout = nTimeout;
    switch (style)
    {
    case Warning:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_WARNING;
            break;
        }
    case Error:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_ERROR;
            break;
        }
    case Info:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_INFO;
            break;
        }
    case None:
        {
            m_NotifyIconData.dwInfoFlags = NIIF_NONE;
            break;
        }
    case User:
        {
            if (hBalloonIcon)
            {
                ATLASSERT(m_ShellVersion >= VersionVista);
                m_NotifyIconData.hBalloonIcon = hBalloonIcon;
            }
            else
            {
                ATLASSERT(hUserIcon != NULL); //You forget to provide a user icon
                m_NotifyIconData.uFlags |= NIF_ICON;
                m_NotifyIconData.hIcon = hUserIcon;
            }

            m_NotifyIconData.dwInfoFlags = NIIF_USER;
            break;
        }
    default:
        {
            ATLASSERT(FALSE);
            break;
        }
    }
    if (bNoSound)
        m_NotifyIconData.dwInfoFlags |= NIIF_NOSOUND;
    if (bLargeIcon)
        m_NotifyIconData.dwInfoFlags |= NIIF_LARGE_ICON;
    if (bRealtime)
        m_NotifyIconData.uFlags |= NIF_REALTIME;

    return Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

taskbar_notify_icon::tooltip_string taskbar_notify_icon::GetBalloonText() const
{
    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later

    tooltip_string sText;
    if (m_bCreated)
        sText = m_NotifyIconData.szInfo;

    return sText;
}

taskbar_notify_icon::tooltip_string taskbar_notify_icon::GetBalloonCaption() const
{
    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later

    tooltip_string sText;
    if (m_bCreated)
        sText = m_NotifyIconData.szInfoTitle;

    return sText;
}

UINT taskbar_notify_icon::GetBalloonTimeout() const
{
    //Validate our parameters
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or later

    UINT nTimeout = 0;
    if (m_bCreated)
        nTimeout = m_NotifyIconData.uTimeout;

    return nTimeout;
}

BOOL taskbar_notify_icon::SetTooltipText(LPCTSTR pszTooltipText)
{
    if (!m_bCreated)
        return FALSE;

    if (m_ShellVersion >= Version5) //Allow the larger size tooltip text if on Shell v5 or later
    {
#ifndef NDEBUG
        NOTIFYICONDATA dummy;
        DBG_UNREFERENCED_LOCAL_VARIABLE(dummy);
        ATLASSERT(_tcslen(pszTooltipText) < sizeof(dummy.szTip)/sizeof(TCHAR));
#endif
    }
    else 
    {
#ifndef NDEBUG
        NOTIFYICONDATA dummy;
        ATLASSERT(_tcslen(pszTooltipText) < sizeof(dummy.szTip)/sizeof(TCHAR));
        DBG_UNREFERENCED_LOCAL_VARIABLE(dummy);
#endif
    }

    //Call the Shell_NotifyIcon function
    m_NotifyIconData.uFlags = NIF_TIP;
    _tcscpy_s(m_NotifyIconData.szTip, sizeof(m_NotifyIconData.szTip)/sizeof(TCHAR), pszTooltipText);
    return Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

BOOL taskbar_notify_icon::SetTooltipText(UINT nID)
{
    tooltip_string sToolTipText;
    if (!sToolTipText.LoadString(nID))
        return FALSE;

    //Let the other version of the function handle the rest
    return SetTooltipText(sToolTipText);
}

BOOL taskbar_notify_icon::SetIcon(CBitmap* pBitmap)
{
    //Convert the bitmap to an ICON
    if (m_hDynamicIcon)
        DestroyIcon(m_hDynamicIcon);
    m_hDynamicIcon = BitmapToIcon(pBitmap);

    //Pass the buck to the other function to do the work
    return SetIcon(m_hDynamicIcon);
}

BOOL taskbar_notify_icon::SetIcon(HICON hIcon)
{
    //Validate our parameters
    ATLASSERT(hIcon);

    if (!m_bCreated)
        return FALSE;

    //Since we are going to use one icon, stop any animation
    StopAnimation();

    //Call the Shell_NotifyIcon function
    m_NotifyIconData.uFlags = NIF_ICON;
    m_NotifyIconData.hIcon = hIcon;
    return Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

BOOL taskbar_notify_icon::SetIcon(LPCTSTR lpIconName)
{
    return SetIcon(LoadIcon(lpIconName));
}

BOOL taskbar_notify_icon::SetIcon(UINT nIDResource)
{
    return SetIcon(LoadIcon(nIDResource));
}

BOOL taskbar_notify_icon::SetStandardIcon(LPCTSTR lpIconName)
{
    return SetIcon(::LoadIcon(NULL, lpIconName));
}

BOOL taskbar_notify_icon::SetStandardIcon(UINT nIDResource)
{
    return SetIcon(::LoadIcon(NULL, MAKEINTRESOURCE(nIDResource)));
}

BOOL taskbar_notify_icon::SetIcon(HICON* phIcons, int nNumIcons, DWORD dwDelay)
{
    //Validate our parameters
    ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
    ATLASSERT(phIcons);
    ATLASSERT(dwDelay);

    if (!SetIcon(phIcons[0]))
        return FALSE;

    //Start the animation
    StartAnimation(phIcons, nNumIcons, dwDelay);

    return TRUE;
}

HICON taskbar_notify_icon::LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName, BOOL bLargeIcon)
{
    return static_cast<HICON>(::LoadImage(hInstance, lpIconName, IMAGE_ICON, bLargeIcon ? GetSystemMetrics(SM_CXICON) : GetSystemMetrics(SM_CXSMICON), bLargeIcon ? GetSystemMetrics(SM_CYICON) : GetSystemMetrics(SM_CYSMICON), LR_SHARED));
}

HICON taskbar_notify_icon::LoadIcon(HINSTANCE hInstance, UINT nIDResource, BOOL bLargeIcon)
{
    return LoadIcon(hInstance, MAKEINTRESOURCE(nIDResource), bLargeIcon);
}

HICON taskbar_notify_icon::LoadIcon(LPCTSTR lpIconName, BOOL bLargeIcon)
{
    return LoadIcon(_Module.GetResourceInstance(), lpIconName, bLargeIcon);
}

HICON taskbar_notify_icon::LoadIcon(UINT nIDResource, BOOL bLargeIcon)
{
    return LoadIcon(MAKEINTRESOURCE(nIDResource), bLargeIcon);
}

BOOL taskbar_notify_icon::SetNotificationWnd(CWindow* pNotifyWnd)
{
    //Validate our parameters
    ATLASSERT(pNotifyWnd && ::IsWindow(pNotifyWnd->operator HWND()));

    if (!m_bCreated)
        return FALSE;

    //Call the Shell_NotifyIcon function
    m_pNotificationWnd = pNotifyWnd;
    m_NotifyIconData.hWnd = pNotifyWnd->operator HWND();
    m_NotifyIconData.uFlags = 0;
    return Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

taskbar_notify_icon::tooltip_string taskbar_notify_icon::GetTooltipText() const
{
    tooltip_string sText;
    if (m_bCreated)
        sText = m_NotifyIconData.szTip;

    return sText;
}

HICON taskbar_notify_icon::GetIcon() const
{
    HICON hIcon = NULL;
    if (m_bCreated)
    {
        if (UsingAnimatedIcon())
            hIcon = GetCurrentAnimationIcon();
        else
            hIcon = m_NotifyIconData.hIcon;
    }

    return hIcon;
}

CWindow* taskbar_notify_icon::GetNotificationWnd() const
{
    return m_pNotificationWnd;
}

BOOL taskbar_notify_icon::SetFocus()
{
    ATLASSERT(m_ShellVersion >= Version5); //Only supported on Shell v5 or greater

    //Call the Shell_NotifyIcon function
    return Shell_NotifyIcon(NIM_SETFOCUS, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

LRESULT taskbar_notify_icon::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
    //Pull out the icon id
    UINT nID = static_cast<UINT>(wParam);

    //Return quickly if its not for this tray icon
    if (nID != m_NotifyIconData.uID)
        return 0L;

    //Work out if we should show the context menu or handle the double click
    BOOL bShowMenu = (lParam == WM_RBUTTONUP);
    BOOL bDoubleClick = (lParam == WM_LBUTTONDBLCLK);
    if (bShowMenu || bDoubleClick)
    {
        CMenuHandle subMenu = m_Menu.GetSubMenu(0);
        ATLASSERT(subMenu.IsMenu());

        if (bShowMenu)
        {
            CPoint ptCursor;
            GetCursorPos(&ptCursor);
            ::SetForegroundWindow(m_NotifyIconData.hWnd);  
            ::TrackPopupMenu(subMenu, TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, 0, m_NotifyIconData.hWnd, NULL);
            ::PostMessage(m_NotifyIconData.hWnd, WM_NULL, 0, 0);
        }
        else if (bDoubleClick) //double click received, the default action is to execute first menu item 
        {
            ::SetForegroundWindow(m_NotifyIconData.hWnd);
            UINT nDefaultItem = subMenu.GetMenuDefaultItem(FALSE, GMDI_GOINTOPOPUPS);

            if (nDefaultItem != -1)
                ::SendMessage(m_NotifyIconData.hWnd, WM_COMMAND, nDefaultItem, 0);
        }
    }

    return 1; // handled
}

BOOL taskbar_notify_icon::GetDynamicDCAndBitmap(CDC* pDC, CBitmap* pBitmap)
{
    //Validate our parameters
    ATLASSERT(pDC);
    ATLASSERT(pBitmap);

    //Get the HWND for the desktop
    CWindow WndScreen(::GetDesktopWindow());
    if (!WndScreen.IsWindow())
        return FALSE;

    //Get the desktop HDC to create a compatible bitmap from
    CDC DCScreen(WndScreen.GetDC());
    if (DCScreen.IsNull())
        return FALSE;

    //Get the width and height of a small icon
    int w = GetSystemMetrics(SM_CXSMICON);
    int h = GetSystemMetrics(SM_CYSMICON);

    //Create an off-screen bitmap that the dynamic tray icon 
    //can be drawn into. (Compatible with the desktop DC).
    BOOL bSuccess = (pBitmap->CreateCompatibleBitmap(DCScreen.operator HDC(), w, h) != NULL);
    if (!bSuccess)
    {
        WndScreen.ReleaseDC(DCScreen);
        return FALSE;
    }

    //Get a HDC to the newly created off-screen bitmap
    bSuccess = (pDC->CreateCompatibleDC(DCScreen.operator HDC()) != NULL);
    if (!bSuccess)
    {
        //Release the Screen DC now that we are finished with it
        WndScreen.ReleaseDC(DCScreen);

        //Free up the bitmap now that we are finished with it
        pBitmap->DeleteObject();

        return FALSE;
    }

    //Select the bitmap into the offscreen DC
    pDC->SelectBitmap(pBitmap->operator HBITMAP());

    //Release the Screen DC now that we are finished with it
    WndScreen.ReleaseDC(DCScreen);

    return TRUE;
}


void taskbar_notify_icon::StartAnimation(HICON* phIcons, int nNumIcons, DWORD dwDelay)
{
    //Validate our parameters
    ATLASSERT(nNumIcons >= 2); //must be using at least 2 icons if you are using animation
    ATLASSERT(phIcons);        //array of icon handles must be valid
    ATLASSERT(dwDelay);        //must be non zero timer interval

    //Stop the animation if already started  
    StopAnimation();

    //Hive away all the values locally
    ATLASSERT(m_phIcons == NULL);
    m_phIcons = new HICON[nNumIcons];
    for (int i=0; i<nNumIcons; i++)
        m_phIcons[i] = phIcons[i];
    m_nNumIcons = nNumIcons;

    //Start up the timer 
    m_nTimerID = SetTimer(m_NotifyIconData.uID, dwDelay);
}

void taskbar_notify_icon::StopAnimation()
{
    //Kill the timer
    if (m_nTimerID)
    {
        if (::IsWindow(m_hWnd))
            KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

    //Free up the memory
    if (m_phIcons)
    {
        delete [] m_phIcons;
        m_phIcons = NULL;
    }

    //Reset the other animation related variables
    m_nCurrentIconIndex = 0;
    m_nNumIcons = 0;
}

BOOL taskbar_notify_icon::UsingAnimatedIcon() const
{
    return (m_nNumIcons != 0);
}

HICON taskbar_notify_icon::GetCurrentAnimationIcon() const 
{ 
    //Valiate our parameters
    ATLASSERT(UsingAnimatedIcon());
    ATLASSERT(m_phIcons);

    return m_phIcons[m_nCurrentIconIndex]; 
}

BOOL taskbar_notify_icon::ProcessWindowMessage(HWND /*hWnd*/, UINT nMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD /*dwMsgMapID*/)
{
    lResult = 0;
    BOOL bHandled = FALSE;

    if (nMsg == WM_TASKBAR_CREATED)
    {
        lResult = OnTaskbarCreated(wParam, lParam);
        bHandled = TRUE;
    }
    else if ((nMsg == WM_TIMER) && (wParam == m_NotifyIconData.uID))
    {
        OnTimer(m_NotifyIconData.uID); 
        bHandled = TRUE; //Do not allow this message to go any further because we have fully handled the message
    }
    else if (nMsg == WM_DESTROY)
    {
        OnDestroy();
        bHandled = TRUE;
    }

    return bHandled;
}

void taskbar_notify_icon::OnDestroy()
{
    StopAnimation();
}

LRESULT taskbar_notify_icon::OnTaskbarCreated(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //Refresh the tray icon if necessary
    BOOL bShowing = IsShowing();
    Delete(FALSE);
    Create(bShowing);

    return 0;
}

#ifndef NDEBUG
void taskbar_notify_icon::OnTimer(UINT_PTR nIDEvent)
#else
void taskbar_notify_icon::OnTimer(UINT_PTR /*nIDEvent*/)  //Just to avoid a compiler warning 
#endif                                                //when being built for release
{
    //Validate our parameters
    ATLASSERT(nIDEvent == m_nTimerID);

    //increment the icon index
    ++m_nCurrentIconIndex;
    m_nCurrentIconIndex = m_nCurrentIconIndex % m_nNumIcons;

    //update the tray icon
    m_NotifyIconData.uFlags = NIF_ICON;
    m_NotifyIconData.hIcon = m_phIcons[m_nCurrentIconIndex];
    Shell_NotifyIcon(NIM_MODIFY, reinterpret_cast<PNOTIFYICONDATA>(&m_NotifyIconData));
}

BOOL taskbar_notify_icon::CreateHelperWindow()
{
    //Let the base class do its thing
    return (CWindowImpl<taskbar_notify_icon>::Create(NULL, CWindow::rcDefault, _T("taskbar_notify_icon Helper Window"), WS_OVERLAPPEDWINDOW) != NULL);
}
