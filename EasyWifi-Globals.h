#ifndef EASYWIFI_GLOBALS_H_
#define EASYWIFI_GLOBALS_H_

// --- Basic Config Settings ------------------------------------------------------------------------
#include <list>

#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer WebServer(80);

#include <PubSubClient.h>
// MQTT client
WiFiClient mqtt;
PubSubClient MQTTclient(mqtt);

#include <ArduinoJson.h>
#include "FS.h"

#define DEV_STATE "State"
#define DEV_NAME "dev_name"
#define DEV_AP_SSID "ap_name"
#define DEV_CONNECT_SSID "wifi_name"
#define DEV_CONNECT_PWD  "wifi_pass"
#define MQTT_SERVICE_ADDRESS "server_name"
#define PUB_TOPIC "pub_topic"
#define SUB_TOPIC "sub_topic"
#define TOPIC_PREFIX "topic_prefix"
#define SERVICE_NAME "service_name"

#define LOG Serial.println

typedef std::list<String> ScanList;

typedef enum State{
    Init_state = 0,
    Ap_state,
    Station_state,
};

typedef struct _Config
{
    State eState;
    String strDeviceName;
    String strApName;
    String strConnectSsid;
    String strConnectPwd;
    String strMqttServiceAddress;
    String strTopicPrefix;
    String strServiceName;
    // String strPubTopic;
    // String strSubTopic;

}Config;


ScanList g_arrayList;

const char* CONFIG_FILE = "/config.js";
const char* HOST = "EasyWiFi";
const int BTN_PIN = D8;     // the number of the pushbutton pin
const int LED_PIN =  2;      // the number of the LED pin

const int SWITCH_PIN = D2;

#endif
