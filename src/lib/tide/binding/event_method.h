/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _EVENT_METHOD_H_
#define _EVENT_METHOD_H_

namespace tide
{
    class TIDE_API EventMethod : public EventObject, public TiMethod
    {
        public:
        EventMethod(const char* name = "") :
            EventObject(name),
            count(1) {}

        // @see TiMethod::Call
        virtual ValueRef Call(const ValueList& args) = 0;

        // @see TiMethod::Set
        virtual void Set(const char *name, ValueRef value)
        {
            EventObject::Set(name, value);
        }

        // @see TiMethod::Get
        virtual ValueRef Get(const char *name)
        {
            return EventObject::Get(name);
        }

        // @see TiMethod::GetPropertyNames
        virtual SharedStringList GetPropertyNames()
        {
            return EventObject::GetPropertyNames();
        }

        // @see TiMethod::HasProperty
        virtual bool HasProperty(const char *name)
        {
            return EventObject::HasProperty(name);
        }
        
        // @see TiMethod::DisplayString
        SharedString DisplayString(int levels)
        {
            return EventObject::DisplayString(levels);
        }

        /**
         * Set a property on this object to the given method. When an error
         * occurs will throw an exception of type ValueException.
         */
        template <typename T>
        void SetMethod(const char *name, void (T::*method)(const ValueList&, ValueRef))
        {
            MethodCallback* callback = NewCallback<T, const ValueList&, ValueRef>(static_cast<T*>(this), method);

            TiMethodRef bound_method = new StaticBoundMethod(callback);
            ValueRef method_value = Value::NewMethod(bound_method);
            EventObject::Set(name, method_value);
        }

        virtual void duplicate()
        {
            ++count;
        }

        virtual void release()
        {
            int value = --count;
            if (value <= 0) {
                delete this;
            }
        }

        virtual int referenceCount() const
        {
            return count.value();
        }

        private:
        Poco::AtomicCounter count;

    };
}

#endif
