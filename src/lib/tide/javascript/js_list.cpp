/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include "javascript_module.h"

namespace tide
{
    KKJSList::KKJSList(JSContextRef context, JSObjectRef jsobject) :
        TiList("JavaScript.KKJSList"),
        context(NULL),
        jsobject(jsobject)
    {
        /* KJS methods run in the global context that they originated from
         * this seems to prevent nasty crashes from trying to access invalid
         * contexts later. Global contexts need to be registered by all modules
         * that use a KJS context. */
        JSObjectRef globalObject = JSContextGetGlobalObject(context);
        JSGlobalContextRef globalContext = JSUtil::GetGlobalContext(globalObject);

        // This context hasn't been registered. Something has gone pretty
        // terribly wrong and TideSDK will likely crash soon. Nonetheless, keep
        // the user up-to-date to keep their hopes up.
        if (globalContext == NULL)
            std::cerr << "Could not locate global context for a KJS method."  <<
                " One of the modules is misbehaving." << std::endl;
        this->context = globalContext;

        JSUtil::ProtectGlobalContext(this->context);
        JSValueProtect(this->context, this->jsobject);

        this->tiObject = new KKJSObject(this->context, this->jsobject);
    }

    KKJSList::~KKJSList()
    {
        JSValueUnprotect(this->context, this->jsobject);
        JSUtil::UnprotectGlobalContext(this->context);
    }

    unsigned int KKJSList::Size()
    {
        ValueRef length_val = this->tiObject->Get("length");
        if (length_val->IsInt())
            return (unsigned int) length_val->ToInt();
        else
            return 0;
    }

    ValueRef KKJSList::At(unsigned int index)
    {
        std::string name = TiList::IntToChars(index);
        ValueRef value = this->tiObject->Get(name.c_str());
        return value;
    }

    void KKJSList::SetAt(unsigned int index, ValueRef value)
    {
        std::string name = TiList::IntToChars(index);
        this->tiObject->Set(name.c_str(), value);
    }

    void KKJSList::Append(ValueRef value)
    {
        ValueRef push_method = this->tiObject->Get("push");

        if (push_method->IsMethod())
        {
            ValueList list;
            list.push_back(value);
            push_method->ToMethod()->Call(list);
        }
        else
        {
            throw ValueException::FromString("Could not find push method on KJS array.");
        }
    }

    bool KKJSList::Remove(unsigned int index)
    {
        if (index >= 0 && index < this->Size())
        {
            ValueRef spliceMethod = this->tiObject->Get("splice");
            spliceMethod->ToMethod()->Call(
                Value::NewInt(index),
                Value::NewInt(1));
            return true;
        }
        return false;
    }


    ValueRef KKJSList::Get(const char* name)
    {
        return tiObject->Get(name);
    }

    void KKJSList::Set(const char* name, ValueRef value)
    {
        return tiObject->Set(name, value);
    }

    bool KKJSList::Equals(TiObjectRef other)
    {
        return this->tiObject->Equals(other);
    }

    SharedStringList KKJSList::GetPropertyNames()
    {
         return tiObject->GetPropertyNames();
    }

    bool KKJSList::HasProperty(const char* name)
    {
        return tiObject->HasProperty(name);
    }

    bool KKJSList::SameContextGroup(JSContextRef c)
    {
        return tiObject->SameContextGroup(c);
    }

    JSObjectRef KKJSList::GetJSObject()
    {
        return this->jsobject;
    }
}
