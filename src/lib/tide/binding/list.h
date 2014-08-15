/**
 * Copyright (c) 2012 - 2014 TideSDK contributors 
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#ifndef _BOUND_LIST_H_
#define _BOUND_LIST_H_

#include <string>
#include <cctype>

namespace tide
{
    /**
     * A binding that represents a list�
     */
    class TIDE_API TiList : public TiObject
    {
    public:
        TiList(const char *type = "TiList") : TiObject(type) {}
        virtual ~TiList() {}

        /**
         * Append a value to this list
         *  Errors will result in a thrown ValueException
         */
        virtual void Append(ValueRef value) = 0;

        /**
         * Get the length of this list.
         */
        virtual unsigned int Size() = 0;

        /**
         * @return the value at the given index.
         * Errors will result in a thrown ValueException
         */
        virtual ValueRef At(unsigned int index) = 0;

        /**
         * Set the value at the given index. If the index is greater
         * than the current list length, the list will be lengenthed
         * by appending Value::Undefined;
         * Errors will result in a thrown ValueException
         */
        virtual void SetAt(unsigned int index, ValueRef value) = 0;

        /**
         * Remove the list entry at the given index. Return true
         * if found and removed.
         * Errors will result in a thrown ValueException
         */
        virtual bool Remove(unsigned int index) = 0;

        /**
         * Set a property on this object to the given value
         * Errors will result in a thrown ValueException
         */
        virtual void Set(const char *name, ValueRef value) = 0;

        /**
         * @return the property with the given name or Value::Undefined
         * if the property is not found.
         * Errors will result in a thrown ValueException
         */
        virtual ValueRef Get(const char *name) = 0;

        /**
         * @return a list of this object's property names.
         */
        virtual SharedStringList GetPropertyNames() = 0;

        /**
         * Modify this size of this list based. If the size is larger
         * than the current length the list will be padded with Undefined.
         * If the size is smaller than the current list the list will be
         * truncated. If the size is invalid, nothing will happen.
         * @arg size the new size of the list.
         */
        void ResizeTo(unsigned int size);

        /**
         * @return a string representation of this object
         */
        SharedString DisplayString(int levels=1);

        /**
         * @return whether or not the passed-in string is an integer
         */
        static bool IsInt(const std::string& name);

        /**
         * @return the passed-in value as a string
         */
        static std::string IntToChars(unsigned int value);

        /**
         * Convert the given string into a TiList index.
         */
        static unsigned int ToIndex(const std::string& str);

        /**
         * Return the unwrapped version of this object
         */
        static TiListRef Unwrap(TiListRef);

    private:
        DISALLOW_EVIL_CONSTRUCTORS(TiList);
    };
}

#endif
