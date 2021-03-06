/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include "ruby_module_instance.h"

namespace tide
{
    // TODO: Implement real method metadata and lifecycle events for
    // scripting language-based modules
    RubyModuleInstance::RubyModuleInstance(Host* host, std::string path, std::string dir, std::string name) :
        Module(host, dir.c_str(), name.c_str(), "0.1"), path(path)
    {
    }

    RubyModuleInstance::~RubyModuleInstance()
    {
    }

    void RubyModuleInstance::Initialize () 
    {
    }

    void RubyModuleInstance::Destroy () 
    {
    }
}
