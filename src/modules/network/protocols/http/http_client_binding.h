/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _HTTP_CLIENT_BINDING_H_
#define _HTTP_CLIENT_BINDING_H_

#include <tide/tide.h>

#include <Poco/Net/NameValueCollection.h>
#include <Poco/URI.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <curl/curl.h>

#include "http_cookie.h"

namespace ti
{
    class HTTPClientBinding : public EventObject, public Poco::Runnable
    {
    public:
        HTTPClientBinding(Host* host);
        virtual ~HTTPClientBinding();

        size_t WriteRequestDataToBuffer(char* buffer, size_t bufferSize);
        void ParseHTTPStatus(std::string& header);
        void GotHeader(std::string& header);
        void DataReceived(char* buffer, size_t numberOfBytes);
        inline bool IsAborted() { return aborted; }
        void RequestDataSent(size_t sent, size_t total);

    private:
        Host* host;
        std::string url;
        std::string httpMethod;
        bool async;
        int timeout;
        long maxRedirects;

        CURL* curlHandle;
        std::string username;
        std::string password;
        Poco::Net::NameValueCollection requestCookies;
        std::map<std::string, TiObjectRef> responseCookies;
        Poco::Net::NameValueCollection responseHeaders;
        Poco::Net::NameValueCollection nextResponseHeaders;
        std::vector<std::string> requestHeaders;

        TiMethodRef outputHandler;
        TiMethodRef ondatastream;
        TiMethodRef onreadystate;
        TiMethodRef onsendstream;
        TiMethodRef onload;

        // This variables must be reset on each send()
        SharedPtr<Poco::Thread> thread;
        BytesRef requestBytes;
        SharedPtr<std::ostringstream> responseStream;
        int requestContentLength;
        bool aborted;
        bool dirty;
        size_t requestDataSent;
        size_t requestDataWritten;
        size_t responseDataReceived;;
        bool sawHTTPStatus;
        std::vector<BytesRef> responseData;
        std::vector<BytesRef> preservedPostData;
        struct curl_httppost* postData;
        ValueRef sendData;

        void run(); // Poco Thread implementation.
        bool BeginRequest(ValueRef sendData);
        void BeginWithPostDataObject(TiObjectRef object);
        void SetRequestData();
        void ChangeState(int readyState);
        void GetResponseCookie(std::string cookieLine);
        struct curl_slist* SetRequestHeaders(CURL* handle);
        void ExecuteRequest();
        bool FireEvent(std::string& eventName);
        void HandleCurlResult(CURLcode result);
        void SetupCurlMethodType();
        void CleanupCurl(curl_slist* headers);
        void AddScalarValueToCurlForm(SharedString propertyName, ValueRef value, curl_httppost** last);

        void Abort(const ValueList& args, ValueRef result);
        void Open(const ValueList& args, ValueRef result);
        void SetCredentials(const ValueList& args, ValueRef result);
        void Send(const ValueList& args, ValueRef result);
        void Receive(const ValueList& args, ValueRef result);
        void SetRequestHeader(const ValueList& args, ValueRef result);
        void GetResponseHeader(const ValueList& args, ValueRef result);
        void GetResponseHeaders(const ValueList& args, ValueRef result);
        void SetCookie(const ValueList& args, ValueRef result);
        void ClearCookies(const ValueList& args, ValueRef result);
        void GetCookie(const ValueList& args, ValueRef result);
        void GetTimeout(const ValueList& args, ValueRef result);
        void SetTimeout(const ValueList& args, ValueRef result);
        void GetMaxRedirects(const ValueList& args, ValueRef result);
        void SetMaxRedirects(const ValueList& args, ValueRef result);
    };
}

#endif
