/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include <tide/tide.h>
#include "process_module.h"
#include "process_binding.h"

using namespace tide;
using namespace ti;

namespace ti
{
    TIDE_MODULE(ProcessModule, STRING(MODULE_NAME), STRING(MODULE_VERSION));
    
    void ProcessModule::Initialize()
    {
        // load our variables
        this->binding = new ProcessBinding();

        // set our ti.Process
        ValueRef value = Value::NewObject(this->binding);
        host->GetGlobalObject()->Set("Process", value);

// simple test to make the app crash
#ifdef TEST_CRASH_DETECTION        
        char *s = NULL;
        (*s)++;
#endif
    }

    void ProcessModule::Stop()
    {
    }
    
}
