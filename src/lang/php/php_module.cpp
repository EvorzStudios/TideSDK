/**
 * Copyright (c) 2012 - 2014 TideSDK contributors
 * http://www.tidesdk.org
 * Includes modified sources under the Apache 2 License
 * Copyright (c) 2008 - 2012 Appcelerator Inc
 * Refer to LICENSE for details of distribution and use.
 **/

#include <signal.h>
#include "php_module.h"
#include <Poco/Path.h>

#ifdef OS_WIN32
#include <tideutils/win/win32_utils.h>
#else
#include <tideutils/posix/posix_utils.h>
#endif
#include <tideutils/file_utils.h>

extern "C"
{
    int php_load_extension(char *filename, int type, int start_now TSRMLS_DC);

    EXPORT PHPModule* CreateModule(Host *host, const char* path)
    {
        return new PHPModule(host, path);
    }
}

#ifdef ZTS
void ***tsrm_ls;
#endif

namespace tide
{
    static Logger* logger = Logger::Get("PHPModule");
    const static std::string phpSuffix("module.php");
    static bool buffering = false;

    PHPModule* PHPModule::instance_ = NULL;
    std::ostringstream PHPModule::buffer;
    std::string PHPModule::mimeType("text/html");

    static int UnbufferedWrite(const char *, unsigned int TSRMLS_DC);
    static void SetIniDefault(HashTable*, const char*, const char*);
    static void IniDefaults(HashTable*);
    static void LogMessage(char*);
    static int HeaderHandler(sapi_header_struct*, sapi_header_op_enum, 
        sapi_headers_struct* TSRMLS_DC);
    static void RegisterServerVariables(zval *tracks_var_array TSRMLS_DC);

    void PHPModule::Initialize()
    {
        PHPModule::instance_ = this;
        int argc = 1;
        char *argv[2] = { "php_tide", NULL };

        php_embed_module.ub_write = UnbufferedWrite;
        php_embed_module.log_message = LogMessage;
        php_embed_module.ini_defaults = IniDefaults;
        php_embed_module.header_handler = HeaderHandler;
        php_embed_module.register_server_variables = RegisterServerVariables;
        php_embed_module.phpinfo_as_text = 1;
        php_embed_init(argc, argv PTSRMLS_CC);

        PHPUtils::InitializePHPTideClasses();
        this->InitializeBinding();
        host->AddModuleProvider(this);

        std::string resourcesPath(host->GetApplication()->GetResourcesPath());
        logger->Debug("PHPModule::Initialize called: " + resourcesPath);
        if(zend_alter_ini_entry("include_path", sizeof("include_path"),
            (char*) resourcesPath.c_str(), resourcesPath.size(),
            ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME) == FAILURE)
        {
            logger->Error("PHPUtils::Initialize failed modifying include_path for php");
        }

#ifdef OS_WIN32
        // Manually load some PHP extensions for Windows.
        TSRMLS_FETCH();
        std::string phpPath(UTF8ToSystem(this->GetPath()));
        phpPath = FileUtils::Join(phpPath.c_str(), "ext", 0);

        std::vector<std::string> extensions;
        extensions.push_back("php_bz2");
        extensions.push_back("php_com_dotnet");
        extensions.push_back("php_curl");
        extensions.push_back("php_enchant");
        extensions.push_back("php_mbstring");
        extensions.push_back("php_exif");
        extensions.push_back("php_fileinfo");
        extensions.push_back("php_gd2");
        extensions.push_back("php_gettext");
        extensions.push_back("php_gmp");
        extensions.push_back("php_imap");
        //extensions.push_back("php_interbase");
        extensions.push_back("php_intl");
        extensions.push_back("php_ldap");
        extensions.push_back("php_mysql");
        extensions.push_back("php_mysqli");
        //extensions.push_back("php_oci8");
        //extensions.push_back("php_oci8_11g");
        extensions.push_back("php_openssl");
        //extensions.push_back("php_pdo_firebird");
        extensions.push_back("php_pdo_mysql");
        //extensions.push_back("php_pdo_oci");
        extensions.push_back("php_pdo_odbc");
        extensions.push_back("php_pdo_pgsql");
        extensions.push_back("php_pdo_sqlite");
        extensions.push_back("php_pgsql");
        extensions.push_back("php_shmop");
        //extensions.push_back("php_snmp");
        extensions.push_back("php_soap");
        extensions.push_back("php_sockets");
        extensions.push_back("php_sqlite");
        extensions.push_back("php_sqlite3");
        //extensions.push_back("php_sybase_ct");
        extensions.push_back("php_tidy");
        extensions.push_back("php_xmlrpc");
        extensions.push_back("php_xsl");

        for(std::vector<std::string>::const_iterator
            iter = extensions.begin();
            iter != extensions.end();
            ++iter)
        {
	    //logger->Debug("Loading php module: " + *iter);
	    std::string modDllName = *iter + ".dll";
            std::string modPath(FileUtils::Join(phpPath.c_str(), modDllName.c_str(), 0));
            php_load_extension((char*) modPath.c_str(), 1, 1 TSRMLS_CC);
        }
#endif
    }

    /*static*/
    void PHPModule::SetBuffering(bool newBuffering)
    {
        if (buffering)
        {
            buffer.str("");
        }
        buffering = newBuffering;
    }

    void PHPModule::Stop()
    {
        PHPModule::instance_ = NULL;

        TiObjectRef global = this->host->GetGlobalObject();
        Script::GetInstance()->RemoveScriptEvaluator(this->binding);
        global->Set("PHP", Value::Undefined);
        this->binding->Set("evaluate", Value::Undefined);

        this->binding = 0;
        PHPModule::instance_ = 0;

        php_embed_shutdown(TSRMLS_C);
    }

    void PHPModule::InitializeBinding()
    {
        PHPModule::mimeType = SG(default_mimetype);

        TiObjectRef global = this->host->GetGlobalObject();
        this->binding = new PHPEvaluator();
        global->Set("PHP", Value::NewObject(this->binding));
        Script::GetInstance()->AddScriptEvaluator(this->binding);

        zval *tideValue = PHPUtils::ToPHPValue(Value::NewObject(global));
        ZEND_SET_SYMBOL(&EG(symbol_table), PRODUCT_NAME, tideValue);
    }


    bool PHPModule::IsModule(std::string& path)
    {
        return (path.substr(path.length()-phpSuffix.length()) == phpSuffix);
    }

    Module* PHPModule::CreateModule(std::string& path)
    {
        zend_first_try
        {
            std::string includeScript = "include '" + path + "';";
            if (SUCCESS != zend_eval_string((char *) includeScript.c_str(),
                NULL, (char *) path.c_str() TSRMLS_CC))
                logger->Error("Error evaluating module at path: %s", path.c_str());
        }
        zend_catch
        {
            logger->Error("Error evaluating module at path: %s", path.c_str());
        }
        zend_end_try();

        Poco::Path p(path);
        std::string name(p.getBaseName());
        std::string moduledir(p.makeParent().toString());
        logger->Info("Loading PHP module name=%s path=%s", name.c_str(), path.c_str());

        return new PHPModuleInstance(host, path, moduledir, name);
    }

    static int UnbufferedWrite(const char *str, unsigned int length TSRMLS_DC)
    {
        std::string string(str, length);
        std::ostringstream& buffer = PHPModule::GetBuffer();

        // This shouldn't need to be thread safe right?
        if (buffering)
        {
            buffer << string;
        }
        else
        {
            // Other language modules ship their output straight to stdout
            // so we might as well do the same here, rather than sending
            // it through the Logger. 
            std::cout << string;
        }
        return length;
    }

    static void SetIniDefault(HashTable* config, const char* name, const char* value)
    {
        // Forgive me Martin, borrowed from php_cli.c line 409
        // Thou are forgiven.
        zval tmp;
        Z_SET_REFCOUNT(tmp, 0);
        Z_UNSET_ISREF(tmp);
        ZVAL_STRINGL(&tmp, zend_strndup(value, sizeof(value)-1), sizeof(value)-1, 0);
        zend_hash_update(config, name, sizeof(name), &tmp, sizeof(zval), NULL);
    }

    static void IniDefaults(HashTable* configuration)
    {
        SetIniDefault(configuration, "display_errors", "1");
    }

    static void LogMessage(char* message)
    {
        logger->Debug(message);
    }

    static int HeaderHandler(sapi_header_struct* sapiHeader,
        sapi_header_op_enum op, sapi_headers_struct* sapiHeaders TSRMLS_DC)
    {
        if (sapiHeaders && sapiHeaders->mimetype)
        {
            std::string& mimeType = PHPModule::GetMimeType();
            mimeType = sapiHeaders->mimetype;
        }
        return op;
    }
    
    static void RegisterServerVariables(zval *tracks_var_array TSRMLS_DC)
    {
        Poco::URI* uri = PHPModule::Instance()->GetURI();
        if (uri)
        {
            php_register_variable("SCRIPT_NAME", (char*)uri->getPath().c_str(), tracks_var_array TSRMLS_CC);
            php_register_variable("REQUEST_URI", (char*)uri->getPathEtc().c_str(), tracks_var_array TSRMLS_CC);
        }
    }
}
