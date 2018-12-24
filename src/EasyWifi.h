#ifndef EasyWifi_h
#define EasyWifi_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>
#include "template.h"
#include "WebStaticData.h"
#include <ArduinoJson.h>
#include <FS.h>

extern "C" {
#include "user_interface.h"
}


#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

#ifndef WIFI_LIST_MAX_ITEM
#define WIFI_LIST_MAX_ITEM 10
#endif

typedef struct WifiItemInfo
{
    String ssid;
    String rssiQ;
    bool encryptionType;
};



class EasyWifiParameter {
public:
    /**
        Create custom parameters that can be added to the EasyWifi setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
    EasyWifiParameter(const char *custom);
    EasyWifiParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
    EasyWifiParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
    ~EasyWifiParameter();

    const char *getID();
    const char *getValue();
    const char *getPlaceholder();
    int         getValueLength();
    const char *getCustomHTML();

private:
    const char *_id;
    const char *_placeholder;
    char       *_value;
    int         _length;
    const char *_customHTML;

    void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);


    template <typename Generic>
    void          LOGD(Generic text);


    template <typename Generic>
    void          LOG(Generic text);

    
    friend class EasyWifi;
};

class EasyWifi
{
public:
    EasyWifi();
    ~EasyWifi();
    
    // 先尝试连接，如果无法连接，则启动Web配置页，apName为热点名称
    boolean       autoConnect();
    boolean       autoConnect(char const *apName, char const *apPassword = NULL);

    //if you want to always start the config portal, without trying to connect first
    boolean       startWebConfig();
    boolean       startWebConfig(char const *apName, char const *apPassword = NULL);

    // get the AP name of the config portal, so it can be used in the callback
    String        getwebConfigSSID();

    void          resetSettings();

    bool          loadCustomParameter();
    bool          saveCustomParameter();

    //sets timeout before webserver loop ends and exits even if there has been no setup.
    //useful for devices that failed to connect at some point and got stuck in a webserver loop
    //in seconds setwebConfigTimeout is a new name for setTimeout
    void          setwebConfigTimeout(unsigned long seconds);
    void          setTimeout(unsigned long seconds);

    //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
    void          setConnectTimeout(unsigned long seconds);

    void          setDebug(boolean debug);
    //defaults to not showing anything under 8% signal quality if called
    void          setMinimumSignalQuality(int quality = 8);
    //sets a custom ip /gateway /subnet configuration
    void          setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
    //called when AP mode and config portal is started
    void          setAPCallback( void (*func)(EasyWifi*) );
    //called when settings have been changed and connection was successful
    void          setSaveConfigCallback( void (*func)(void) );
    //adds a custom parameter, returns false on failure
    bool          addParameter(EasyWifiParameter *p);
    //if this is set, it will exit after config, even if connection is unsuccessful.
    void          setBreakAfterConfig(boolean shouldBreak);
    //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
    //TODO
    //if this is set, customise style
    void          setCustomHeadElement(const char* element);
    //if this is true, remove duplicated Access Points - defaut true
    void          setRemoveDuplicateAPs(boolean removeDuplicates);


private:
    std::unique_ptr<DNSServer>        dnsServer;
    std::unique_ptr<ESP8266WebServer> server;

    void          setupWebConfig();
    void          startWPS();

    int           connectWifi(String ssid, String pass);
    uint8_t       waitForConnectResult();

    void          hdRoot();
    void          hdWifiSave();

    void          handleRoot();
    void          handleWifi(boolean scan);
    void          handleWifiSave();
    void          handleInfo();
    void          handleReset();
    void          handleNotFound();
    void          handle204();
    boolean       captivePortal();
    boolean       webConfigHasTimeout();

    void          scan4getApList();

    //helpers
    int           getRSSIasQuality(int RSSI);
    boolean       isIp(String str);
    String        toStringIp(IPAddress ip);

    void (*_apcallback)(EasyWifi*) = NULL;
    void (*_savecallback)(void) = NULL;


    template <typename Generic>
    void          LOGD(Generic text);


    template <typename Generic>
    void          LOG(Generic text);

    template <class T>
    auto optionalIPFromString(T *obj, const char *s) -> decltype(  obj->fromString(s)  ) {
        return  obj->fromString(s);
    }
    auto optionalIPFromString(...) -> bool {
        LOGD("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
        return false;
    }


    // DNS server
    const byte    DNS_PORT = 53;

    const char*   _apName                 = "no-net";
    const char*   _apPassword             = NULL;
    String        _ssid                   = "";
    String        _pass                   = "";
    unsigned long _webConfigTimeout    = 0;
    unsigned long _connectTimeout         = 0;
    unsigned long _webConfigStart      = 0;

    IPAddress     _ap_static_ip;
    IPAddress     _ap_static_gw;
    IPAddress     _ap_static_sn;

    int           _paramsCount            = 0;
    int           _minimumQuality         = -1;
    boolean       _removeDuplicateAPs     = true;
    boolean       _shouldBreakAfterConfig = false;

    const char*   _customHeadElement      = "";

    int           status = WL_IDLE_STATUS;

    boolean       connect;
    boolean       _debug = true;

    int                    _max_params;
    EasyWifiParameter** _params;

    int           _wifiListCount           = 0;
    WifiItemInfo _wifiList[WIFI_LIST_MAX_ITEM];


};

#endif
