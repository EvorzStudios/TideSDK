/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include "../notification.h"
#include "SnarlInterface.h"

#include <tide/url_utils.h>

namespace ti
{

/*static*/
bool Notification::InitializeImpl()
{
    Snarl::SnarlInterface snarlInterface;
    WORD major, minor;
    return snarlInterface.GetVersion(&major, &minor);
}

/*static*/
void Notification::ShutdownImpl()
{
}

static UINT snarlWindowMessage = ::RegisterWindowMessageA("TideSDKSnarlMessage");
static bool MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message != snarlWindowMessage)
        return false;
        
    return true;
}

void Notification::CreateImpl()
{
    this->notification = -1;
}

void Notification::DestroyImpl()
{
}

bool Notification::ShowImpl()
{
    Snarl::SnarlInterface snarlInterface;

    std::string iconPath;
    if (!iconURL.empty())
        iconPath = URLUtils::URLToPath(iconURL);

    HWND replyWindow = Host::GetInstance()->AddMessageHandler(&MessageHandler);
    long id = snarlInterface.ShowMessage(::UTF8ToWide(this->title).c_str(),
        ::UTF8ToWide(this->message).c_str(), this->timeout,
         ::UTF8ToWide(iconPath).c_str(), replyWindow, snarlWindowMessage);

    return true;
}

bool Notification::HideImpl()
{
    if (this->notification == -1)
        return true;

    Snarl::SnarlInterface snarlInterface;
    return snarlInterface.HideMessage(this->notification);
}
}
