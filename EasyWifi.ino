#include "EasyWifi-Globals.h"

void setup() {
  initSerial();
  delay(500);
  initPin();
  delay(500);
  initFS();
  delay(1000);

  if (checkResetBtnPress())
  {
      LOG("===============Reset Button Pressed===========");
  }
  Config config;
  initConfig(config);

  if (config.eState == Ap_state)
  {
      setWifiMode(WIFI_STA);
      scanWifi(g_arrayList);
      printScanList(g_arrayList);

      setWifiApServer(config.strApName);
  }
  else if (config.eState == Station_state)
  {
      setWifiMode(WIFI_OFF);
      delay(1000);
      setWifiMode(WIFI_STA);

      connectStation(config.strConnectSsid,config.strConnectPwd);
      connectMqttServer(config.strMqttServiceAddress);
  }
  else
  {

  }

}

void loop() {
  static bool bLoadFinish = false;
  static Config config;

  if (!bLoadFinish)
  {
      loadConfig(config);
      bLoadFinish = true;
  }

  if (config.eState == Ap_state)
  {
    WebServer.handleClient();
  }
  else if (config.eState == Station_state)
  {
    if (!MQTTclient.connected()) 
    {
      //digitalWrite(LED_BUILTIN, LOW);
      reconnect();
      //digitalWrite(LED_BUILTIN, HIGH);
    }
    
    MQTTclient.loop();

    if (checkResetBtnPress())
    {
      LOG("===============Reset Button Pressed===========");
      SPIFFS.remove(CONFIG_FILE);
      config.eState = Ap_state;
      setWifiMode(WIFI_STA);
      scanWifi(g_arrayList);
      printScanList(g_arrayList);
      setWifiApServer(config.strApName);
      
    }
  }


  

}
