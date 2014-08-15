/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include "tcp_server_socket_binding.h"
#include "tcp_server_connection_binding.h"

#include <Poco/NObserver.h>
#include <Poco/Net/SocketNotification.h>

namespace ti
{
    
    //////////////////////////////////////////////////////////////////////////////////////////

    TCPServerSocketConnector::TCPServerSocketConnector(TiMethodRef callback_, Poco::Net::ServerSocket& socket_, Poco::Net::SocketReactor & reactor_) :
        callback(callback_),socket(socket_),reactor(reactor_)
    {
        reactor.addEventHandler(socket, Poco::Observer<TCPServerSocketConnector, Poco::Net::ReadableNotification>(*this, &TCPServerSocketConnector::onAccept));
    }
    TCPServerSocketConnector::~TCPServerSocketConnector()
    {
        reactor.removeEventHandler(socket, Poco::Observer<TCPServerSocketConnector, Poco::Net::ReadableNotification>(*this, &TCPServerSocketConnector::onAccept));
    }
    void TCPServerSocketConnector::onAccept(Poco::Net::ReadableNotification *n)
    {
        tide::Logger *logger = tide::Logger::Get("Network.TCPServerSocketConnector");

        n->release();
        Poco::Net::StreamSocket sock = socket.acceptConnection();
        AutoPtr<TCPServerConnectionBinding> conn = new TCPServerConnectionBinding(sock,reactor);

        ValueList args = ValueList();
        args.push_back(Value::NewObject(conn));
        try
        {
            RunOnMainThread(callback, args, false);
        }
        catch(ValueException& e)
        {
            SharedString ss = e.GetValue()->DisplayString();
            logger->Error("onAccept callback failed: %s", ss->c_str());
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////
    
    TCPServerSocketBinding::TCPServerSocketBinding(Host* ti_host, TiMethodRef create) :
        StaticBoundObject("Network.TCPServerSocket"),
        onCreate(create), 
        socket(0),
        acceptor(0),
        listenAdapter(new Poco::RunnableAdapter<TCPServerSocketBinding>(
            *this, &TCPServerSocketBinding::ListenThread)),
        listening(false)
    {
        /**
         * @tiapi(method=True,name=Network.TCPServerSocket.listen,since=1.2)
         * @tiapi start listening for incoming connections
         * @tiarg[int, port] port to bind server socket
         */
        this->SetMethod("listen", &TCPServerSocketBinding::Listen);

        /**
         * @tiapi(method=True,name=Network.TCPServerSocket.close,since=1.2)
         * @tiapi stop listening for incoming connections
         */
        this->SetMethod("close", &TCPServerSocketBinding::Close);
    }

    TCPServerSocketBinding::~TCPServerSocketBinding()
    {
        if (this->listening)
        {
            this->listening = false;
            this->reactor.stop();
            this->socket->close();
        }

        listenThread.join();
        delete listenAdapter;
    }
    
    void TCPServerSocketBinding::ListenThread()
    {
        this->acceptor = new TCPServerSocketConnector(this->onCreate,*this->socket,this->reactor);
        this->listening = true;

        while(this->listening)
        {
            this->reactor.run();
        }
    }
    
    void TCPServerSocketBinding::Listen(const ValueList& args, ValueRef result)
    {
        args.VerifyException("listen", "n");

        if (this->listening || listenThread.isRunning())
        {
            throw ValueException::FromString("Socket is already listening");
        }
        
        //TODO: add support for bind ipaddress
        
        int port = args.at(0)->ToInt();
        this->socket = new Poco::Net::ServerSocket(port);

        listenThread.start(*listenAdapter);
        result->SetBool(true);
    }
    
    void TCPServerSocketBinding::Close(const ValueList& args, ValueRef result)
    {
        if (this->listening)
        {
            this->listening = false;
            this->reactor.stop();
            this->socket->close();
            result->SetBool(true);
        }
        else
        {
            result->SetBool(false);
        }
    }
}
