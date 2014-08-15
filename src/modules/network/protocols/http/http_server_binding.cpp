/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifdef OS_OSX    //For some reason, 10.5 was fine with Cocoa headers being last, but 10.4 balks.
#import <Cocoa/Cocoa.h>
#endif

#include <tide/tide.h>
#include "http_server_binding.h"
#include "http_server_request_factory.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../../network_binding.h"
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketAddress.h>

namespace ti
{
    HTTPServerBinding::HTTPServerBinding(Host* host) :
        StaticBoundObject("Network.HTTPServer"),
        host(host),
        global(host->GetGlobalObject()),
        callback(0),
        socket(0),
        connection(0)
    {
        /**
         * @tiapi(method=True,name=Network.HTTPServer.bind,since=0.3) bind this server to a port on a specific interface
         * @tiarg(for=Network.HTTPServer.bind,name=port,type=Number) port to bind on
         * @tiarg(for=Network.HTTPServer.bind,name=address,type=String,optional=True) address to bind to
         * @tiarg(for=Network.HTTPServer.bind,name=callback,type=Method) callback for server logic (in seperate thread)
         */
        SetMethod("bind",&HTTPServerBinding::Bind);
        
        /**
         * @tiapi(method=True,name=Network.HTTPServer.close,since=0.3) close this server
         */
        SetMethod("close",&HTTPServerBinding::Close);
        
        /**
         * @tiapi(method=True,name=Network.HTTPServer.isClosed,since=0.3) check to see if this server socket is closed
         * @tiresult(for=Network.HTTPServer.isClosed,type=Boolean) return whether or not this server socket is closed
         */
        SetMethod("isClosed",&HTTPServerBinding::IsClosed);
    }
    HTTPServerBinding::~HTTPServerBinding()
    {
        TIDE_DUMP_LOCATION
        Close();
    }
    void HTTPServerBinding::Bind(const ValueList& args, ValueRef result)
    {
        Close();
        
        // port, callback
        // port, ipaddress, callback
        int port = args.at(0)->ToInt();
        std::string ipaddress = "127.0.0.1";
        
        if (args.at(1)->IsString())
        {
            ipaddress = args.at(1)->ToString();
        }
        else if (args.at(1)->IsMethod())
        {
            callback = args.at(1)->ToMethod();
        }
        if (args.size()==3)
        {
            callback = args.at(2)->ToMethod();
        }
        
        Poco::Net::SocketAddress addr(ipaddress,port);
        this->socket = new Poco::Net::ServerSocket(addr);        
        
        connection = new Poco::Net::HTTPServer(new HttpServerRequestFactory(host,callback), *socket, new Poco::Net::HTTPServerParams);
        connection->start();
    }
    void HTTPServerBinding::Close()
    {
        if (this->connection!=NULL)
        {
            this->connection->stop();
            delete this->connection;
            connection = NULL;
        }
        if (this->socket!=NULL)
        {
            delete this->socket;
            this->socket = NULL;
        }
        this->callback = NULL;
    }
    void HTTPServerBinding::Close(const ValueList& args, ValueRef result)
    {
        Close();
    }
    void HTTPServerBinding::IsClosed(const ValueList& args, ValueRef result)
    {
        result->SetBool(this->connection==NULL);
    }
}
