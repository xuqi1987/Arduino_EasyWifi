#include "EasyWifi.h"

EasyWifi::EasyWifi() {
    // 用于存放自定义参数
    _max_params = WIFI_MANAGER_MAX_PARAMS;
    _params = (EasyWifiParameter**)malloc(_max_params * sizeof(EasyWifiParameter*));
    
    // 设置作为Ap的时候的ip地址
    setAPStaticIPConfig(IPAddress(192,168,1,1),IPAddress(192,168,1,1),IPAddress(255,255,255,0));

    // 设置自动连接超时20秒
    setConnectTimeout(20);
}

EasyWifi::~EasyWifi()
{
    if (_params != NULL)
    {
        LOGD(F("freeing allocated params!"));
        free(_params);
    }
}

boolean EasyWifi::autoConnect() {
  // 使用芯片ID
  String ssid = "EasyWifi_" + String(ESP.getChipId());
  return autoConnect(ssid.c_str(), NULL);
}

boolean EasyWifi::autoConnect(char const *apName, char const *apPassword) {
  
  LOGD(F("AutoConnect"));
  // attempt to connect; should it fail, fall back to AP
  WiFi.mode(WIFI_STA);

  // 尝试自动连接
  if (connectWifi("", "") == WL_CONNECTED)   {
    LOG(F("IP Address:"));
    // 连接成功，打印当前的IP
    LOGD(WiFi.localIP());
    //connected
    return true;
  }
  // 自动连接失败，显示配置页面
  return startWebConfig(apName, apPassword);
}

boolean EasyWifi::startWebConfig() {
  String ssid = "EasyWifi_" + String(ESP.getChipId());
  return startWebConfig(ssid.c_str(), NULL);
}

boolean  EasyWifi::startWebConfig(char const *apName, char const *apPassword) {
  
  if(!WiFi.isConnected()){
    WiFi.persistent(false);
    // disconnect sta, start ap
    WiFi.disconnect(); //  this alone is not enough to stop the autoconnecter
    WiFi.mode(WIFI_AP);
    WiFi.persistent(true);
  } 
  else {
    //setup AP
    WiFi.mode(WIFI_AP_STA);
    LOGD(F("SET AP STA"));
  }

  _apName = apName;
  _apPassword = apPassword;

  //notify we entered AP mode
  if ( _apcallback != NULL) {
    LOGD(F("ap_callback"));
    _apcallback(this);
  }

  connect = false;
  setupWebConfig();

  while(1){

    // check if timeout
    if(webConfigHasTimeout()) break;

    //DNS
    dnsServer->processNextRequest();
    //HTTP
    server->handleClient();


    if (connect) {
      connect = false;
      delay(2000);
      LOGD(F("Connecting to new AP"));

      // using user-provided  _ssid, _pass in place of system-stored ssid and pass
      if (connectWifi(_ssid, _pass) != WL_CONNECTED) {
        LOGD(F("Failed to connect."));
      } else {
        //connected
        WiFi.mode(WIFI_STA);
        //notify that configuration has changed and any optional parameters should be saved
        if ( _savecallback != NULL) {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
        break;
      }

      if (_shouldBreakAfterConfig) {
        //flag set to exit after config after trying to connect
        //notify that configuration has changed and any optional parameters should be saved
        if ( _savecallback != NULL) {
          //todo: check if any custom parameters actually exist, and check if they really changed maybe
          _savecallback();
        }
        break;
      }
    }
    yield();
  }

  server.reset();
  dnsServer.reset();

  return  WiFi.status() == WL_CONNECTED;
}

void EasyWifi::setupWebConfig() {
  dnsServer.reset(new DNSServer());
  server.reset(new ESP8266WebServer(80));

  _webConfigStart = millis();

  LOG(F("Configuring access point... "));
  LOG(F("AP_Name:"));
  LOG(_apName);

  if (_apPassword != NULL) {
    if (strlen(_apPassword) < 8 || strlen(_apPassword) > 63) {
      // fail passphrase to short or long!
      LOGD(F("Invalid AccessPoint password. Ignoring"));
      _apPassword = NULL;
    }
    LOG(F(" AP_Password:"));
    LOGD(_apPassword);
  }

  //optional soft ip config
  if (_ap_static_ip) {
    LOGD(F("\nCustom AP IP/GW/Subnet"));
    WiFi.softAPConfig(_ap_static_ip, _ap_static_gw, _ap_static_sn);
  }

  if (_apPassword != NULL) {
    WiFi.softAP(_apName, _apPassword);//password option
  } else {
    WiFi.softAP(_apName);
  }

  delay(500); // Without delay I've seen the IP address blank
  LOG(F("AP IP address: "));
  LOGD(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server->on(String(F("/")), std::bind(&EasyWifi::handleRoot, this));
  server->on(String(F("/wifi")), std::bind(&EasyWifi::handleWifi, this, true));
  server->on(String(F("/0wifi")), std::bind(&EasyWifi::handleWifi, this, false));
  server->on(String(F("/wifisave")), std::bind(&EasyWifi::handleWifiSave, this));
  server->on(String(F("/i")), std::bind(&EasyWifi::handleInfo, this));
  server->on(String(F("/r")), std::bind(&EasyWifi::handleReset, this));
  //server->on("/generate_204", std::bind(&EasyWifi::handle204, this));  //Android/Chrome OS captive portal check.
  server->on(String(F("/fwlink")), std::bind(&EasyWifi::handleRoot, this));  //Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server->onNotFound (std::bind(&EasyWifi::handleNotFound, this));
  server->begin(); // Web server start
  LOGD(F("HTTP server started"));
}


bool EasyWifi::addParameter(EasyWifiParameter *p) {
  if(_paramsCount + 1 > _max_params)
  {
    // rezise the params array
    _max_params += WIFI_MANAGER_MAX_PARAMS;
    LOGD(F("Increasing _max_params to:"));
    LOGD(_max_params);
    EasyWifiParameter** new_params = (EasyWifiParameter**)realloc(_params, _max_params * sizeof(EasyWifiParameter*));
    if (new_params != NULL) {
      _params = new_params;
    } else {
      LOGD(F("ERROR: failed to realloc params, size not increased!"));
      return false;
    }
  }

  _params[_paramsCount] = p;
  _paramsCount++;
  LOGD(F("Adding parameter"));
  LOGD(p->getID());
  return true;
}




boolean EasyWifi::webConfigHasTimeout(){
    if(_webConfigTimeout == 0 || wifi_softap_get_station_num() > 0){
      _webConfigStart = millis(); // kludge, bump webConfig start time to skew timeouts
      return false;
    }
    return (millis() > _webConfigStart + _webConfigTimeout);
}

int EasyWifi::connectWifi(String ssid, String pass) {
  LOGD(F("Connecting as wifi client..."));

  //fix for auto connect racing issue
  if (WiFi.status() == WL_CONNECTED) {
    LOGD(F("Already connected. Bailing out."));
    return WL_CONNECTED;
  }
  //check if we have ssid and pass and force those, if not, try with last saved values
  if (ssid != "") {
    WiFi.begin(ssid.c_str(), pass.c_str());
  } else {
    if (WiFi.SSID()) {
      LOGD(F("Using last saved values, should be faster"));
      //trying to fix connection in progress hanging
      ETS_UART_INTR_DISABLE();
      wifi_station_disconnect();
      ETS_UART_INTR_ENABLE();

      WiFi.begin();
    } else {
      LOGD(F("No saved credentials"));
    }
  }

  int connRes = waitForConnectResult();

  LOG("Connection result: ");
  LOGD(connRes );
  return connRes;
}

uint8_t EasyWifi::waitForConnectResult() {
  if (_connectTimeout == 0) {
    return WiFi.waitForConnectResult();
  } else {
    LOG(F("ssid:"));
    LOGD(WiFi.SSID());
    LOGD (F("Waiting for connection result with time out"));
    
    unsigned long start = millis();
    boolean keepConnecting = true;
    uint8_t status;
    while (keepConnecting) {
      status = WiFi.status();
      LOG(F("."));
      if (millis() > start + _connectTimeout) {
        keepConnecting = false;
        LOGD (F("\nConnection timed out"));
      }
      if (status == WL_CONNECTED ) {
        keepConnecting = false;
        LOGD (F("\nConnection OK"));
      }
      if (status == WL_CONNECT_FAILED) {
        keepConnecting = false;
        LOGD (F("\nConnection NG"));       
      }
      delay(100);
    }
    return status;
  }
}

void EasyWifi::startWPS() {
  LOGD(F("START WPS"));
  WiFi.beginWPSConfig();
  LOGD(F("END WPS"));
}
/*
  String EasyWifi::getSSID() {
  if (_ssid == "") {
    LOGD(F("Reading SSID"));
    _ssid = WiFi.SSID();
    LOGD(F("SSID: "));
    LOGD(_ssid);
  }
  return _ssid;
  }

  String EasyWifi::getPassword() {
  if (_pass == "") {
    LOGD(F("Reading Password"));
    _pass = WiFi.psk();
    LOGD("Password: " + _pass);
    //LOGD(_pass);
  }
  return _pass;
  }
*/
String EasyWifi::getwebConfigSSID() {
  return _apName;
}

void EasyWifi::resetSettings() {
  LOGD(F("THIS MAY CAUSE AP NOT TO START UP PROPERLY. YOU NEED TO COMMENT IT OUT AFTER ERASING THE DATA."));
  LOGD(F("resetSettings"));
  WiFi.disconnect(true);
  //delay(200);
}
void EasyWifi::setTimeout(unsigned long seconds) {
  setwebConfigTimeout(seconds);
}

void EasyWifi::setwebConfigTimeout(unsigned long seconds) {
  _webConfigTimeout = seconds * 1000;
}

void EasyWifi::setConnectTimeout(unsigned long seconds) {
  _connectTimeout = seconds * 1000;
}

void EasyWifi::setDebugOutput(boolean debug) {
  _debug = debug;
}

void EasyWifi::setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn) {
  _ap_static_ip = ip;
  _ap_static_gw = gw;
  _ap_static_sn = sn;
}

void EasyWifi::setMinimumSignalQuality(int quality) {
  _minimumQuality = quality;
}

void EasyWifi::setBreakAfterConfig(boolean shouldBreak) {
  _shouldBreakAfterConfig = shouldBreak;
}

/** Handle root or redirect to captive portal */
void EasyWifi::handleRoot() {
  LOGD(F("Handle root"));
  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Options");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += String(F("<h1>"));
  page += _apName;
  page += String(F("</h1>"));
  page += String(F("<h3>EasyWifi</h3>"));
  page += FPSTR(HTTP_PORTAL_OPTIONS);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

}

/** Wifi config page handler */
void EasyWifi::handleWifi(boolean scan) {

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Config ESP");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);

  if (scan) {
    int n = WiFi.scanNetworks();
    LOGD(F("Scan done"));
    if (n == 0) {
      LOGD(F("No networks found"));
      page += F("No networks found. Refresh to scan again.");
    } else {

      //sort networks
      int indices[n];
      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

      // RSSI SORT

      // old sort
      for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
          if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
            std::swap(indices[i], indices[j]);
          }
        }
      }

      /*std::sort(indices, indices + n, [](const int & a, const int & b) -> bool
        {
        return WiFi.RSSI(a) > WiFi.RSSI(b);
        });*/

      // remove duplicates ( must be RSSI sorted )
      if (_removeDuplicateAPs) {
        String cssid;
        for (int i = 0; i < n; i++) {
          if (indices[i] == -1) continue;
          cssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) {
            if (cssid == WiFi.SSID(indices[j])) {
              LOGD("DUP AP: " + WiFi.SSID(indices[j]));
              indices[j] = -1; // set dup aps to index -1
            }
          }
        }
      }

      //display networks in page
      for (int i = 0; i < n; i++) {
        if (indices[i] == -1) continue; // skip dups
        LOGD(WiFi.SSID(indices[i]));
        LOGD(WiFi.RSSI(indices[i]));
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));

        if (_minimumQuality == -1 || _minimumQuality < quality) {
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) {
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //LOGD(item);
          page += item;
          delay(0);
        } else {
          LOGD(F("Skipping due to quality"));
        }

      }
      page += "<br/>";
    }
  }

  page += FPSTR(HTTP_FORM_START);
  char parLength[5];
  // add the extra parameters to the form
  for (int i = 0; i < _paramsCount; i++) {
    if (_params[i] == NULL) {
      break;
    }

    String pitem = FPSTR(HTTP_FORM_PARAM);
    if (_params[i]->getID() != NULL) {
      pitem.replace("{i}", _params[i]->getID());
      pitem.replace("{n}", _params[i]->getID());
      pitem.replace("{p}", _params[i]->getPlaceholder());
      snprintf(parLength, 5, "%d", _params[i]->getValueLength());
      pitem.replace("{l}", parLength);
      pitem.replace("{v}", _params[i]->getValue());
      pitem.replace("{c}", _params[i]->getCustomHTML());
    } else {
      pitem = _params[i]->getCustomHTML();
    }

    page += pitem;
  }
  if (_params[0] != NULL) {
    page += "<br/>";
  }

  page += FPSTR(HTTP_FORM_END);
  page += FPSTR(HTTP_SCAN_LINK);

  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);


  LOGD(F("Sent config page"));
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void EasyWifi::handleWifiSave() {
  LOGD(F("WiFi save"));

  //SAVE/connect here
  _ssid = server->arg("s").c_str();
  _pass = server->arg("p").c_str();

  //parameters
  for (int i = 0; i < _paramsCount; i++) {
    if (_params[i] == NULL) {
      break;
    }
    //read parameter
    String value = server->arg(_params[i]->getID()).c_str();
    //store it in array
    value.toCharArray(_params[i]->_value, _params[i]->_length + 1);
    LOGD(F("Parameter"));
    LOGD(_params[i]->getID());
    LOGD(value);
  }

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Credentials Saved");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += FPSTR(HTTP_SAVED);
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  LOGD(F("Sent wifi save page"));

  connect = true; //signal ready to connect/reset
}

/** Handle the info page */
void EasyWifi::handleInfo() {
  LOGD(F("Info"));

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("<dl>");
  page += F("<dt>Chip ID</dt><dd>");
  page += ESP.getChipId();
  page += F("</dd>");
  page += F("<dt>Flash Chip ID</dt><dd>");
  page += ESP.getFlashChipId();
  page += F("</dd>");
  page += F("<dt>IDE Flash Size</dt><dd>");
  page += ESP.getFlashChipSize();
  page += F(" bytes</dd>");
  page += F("<dt>Real Flash Size</dt><dd>");
  page += ESP.getFlashChipRealSize();
  page += F(" bytes</dd>");
  page += F("<dt>Soft AP IP</dt><dd>");
  page += WiFi.softAPIP().toString();
  page += F("</dd>");
  page += F("<dt>Soft AP MAC</dt><dd>");
  page += WiFi.softAPmacAddress();
  page += F("</dd>");
  page += F("<dt>Station MAC</dt><dd>");
  page += WiFi.macAddress();
  page += F("</dd>");
  page += F("</dl>");
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  LOGD(F("Sent info page"));
}

/** Handle the reset page */
void EasyWifi::handleReset() {
  LOGD(F("Reset"));

  String page = FPSTR(HTTP_HEAD);
  page.replace("{v}", "Info");
  page += FPSTR(HTTP_SCRIPT);
  page += FPSTR(HTTP_STYLE);
  page += _customHeadElement;
  page += FPSTR(HTTP_HEAD_END);
  page += F("Module will reset in a few seconds.");
  page += FPSTR(HTTP_END);

  server->sendHeader("Content-Length", String(page.length()));
  server->send(200, "text/html", page);

  LOGD(F("Sent reset page"));
  delay(5000);
  ESP.reset();
  delay(2000);
}

void EasyWifi::handleNotFound() {
  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += ( server->method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";

  for ( uint8_t i = 0; i < server->args(); i++ ) {
    message += " " + server->argName ( i ) + ": " + server->arg ( i ) + "\n";
  }
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->sendHeader("Content-Length", String(message.length()));
  server->send ( 404, "text/plain", message );
}


/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
boolean EasyWifi::captivePortal() {
  if (!isIp(server->hostHeader()) ) {
    LOGD(F("Request redirected to captive portal"));
    server->sendHeader("Location", String("http://") + toStringIp(server->client().localIP()), true);
    server->send ( 302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server->client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

//start up config portal callback
void EasyWifi::setAPCallback( void (*func)(EasyWifi* myEasyWifi) ) {
  _apcallback = func;
}

//start up save config callback
void EasyWifi::setSaveConfigCallback( void (*func)(void) ) {
  _savecallback = func;
}

//sets a custom element to add to head, like a new style tag
void EasyWifi::setCustomHeadElement(const char* element) {
  _customHeadElement = element;
}

//if this is true, remove duplicated Access Points - defaut true
void EasyWifi::setRemoveDuplicateAPs(boolean removeDuplicates) {
  _removeDuplicateAPs = removeDuplicates;
}

template <typename Generic>
void EasyWifi::LOGD(Generic text) {
  if (_debug) {
    Serial.println(text);
  }
}

template <typename Generic>
void EasyWifi::LOG(Generic text) {
  if (_debug) {
    Serial.print(text);
  }
}

int EasyWifi::getRSSIasQuality(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}

/** Is this an IP? */
boolean EasyWifi::isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String EasyWifi::toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
