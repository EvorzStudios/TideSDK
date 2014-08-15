/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/


#include <tideutils/file_utils.h>
#include <tideutils/boot_utils.h>
using namespace TideUtils;

#include "network_module.h"
#include <Poco/Mutex.h>

using namespace tide;
using namespace ti;
namespace ti
{
    TIDE_MODULE(NetworkModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
    static std::string modulePath;
    static CURLSH* curlShareHandle = 0;

    static Poco::Mutex* SharedResourceMutex(curl_lock_data data)
    {
        static Poco::Mutex cookieMutex;
        static Poco::Mutex dnsMutex;
        static Poco::Mutex shareMutex;

        switch (data) {
            case CURL_LOCK_DATA_COOKIE:
                return &cookieMutex;
            case CURL_LOCK_DATA_DNS:
                return &dnsMutex;
            case CURL_LOCK_DATA_SHARE:
                return &shareMutex;
            default:
                return NULL;
        }
    }

    static void CurlLockCallback(CURL* handle, curl_lock_data data, curl_lock_access, void*)
    {
        if (Poco::Mutex* mutex = SharedResourceMutex(data))
            mutex->lock();
    }

    static void CurlUnlockCallback(CURL* handle, curl_lock_data data, void* userPtr)
    {
        if (Poco::Mutex* mutex = SharedResourceMutex(data))
            mutex->unlock();
    }

    void NetworkModule::Initialize()
    {
        curl_global_init(CURL_GLOBAL_ALL);

        modulePath = GetPath();

        this->networkBinding = new NetworkBinding(host);
        this->analyticsBinding = new AnalyticsBinding();
        GlobalObject::GetInstance()->SetObject("Network", this->networkBinding);
        GlobalObject::GetInstance()->SetObject("Analytics", this->analyticsBinding);

        curlShareHandle = curl_share_init();
        curl_share_setopt(curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
        curl_share_setopt(curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
        curl_share_setopt(curlShareHandle, CURLSHOPT_LOCKFUNC, CurlLockCallback);
        curl_share_setopt(curlShareHandle, CURLSHOPT_UNLOCKFUNC, CurlUnlockCallback);
    }

    void NetworkModule::Stop()
    {
        analyticsBinding->Shutdown();
    }

    /*static*/
    std::string& NetworkModule::GetRootCertPath()
    {
        static std::string path;
        if (path.empty())
        {
            SharedApplication app(Host::GetInstance()->GetApplication());
            path = FileUtils::Join(app->runtime->path.c_str(), "rootcert.pem", 0);
        }
        return path;
    }

    /*static*/
    CURLSH* NetworkModule::GetCurlShareHandle()
    {
        return curlShareHandle;
    }
}
