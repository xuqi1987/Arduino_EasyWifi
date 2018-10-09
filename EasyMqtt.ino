#include "EasyWifi-Globals.h"

void recv(char* topic, byte* payload, unsigned int length)
{
    LOG(topic);
    String strJsondata = (char*)payload;

    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(strJsondata);

    if (!json.success()) {
        LOG("Failed to parse recv json data");
        return;
    } 

    if (json[DEV_NAME].asString() == )   

}


void send(String topic,String data)
{   
    MQTTclient.publish(topic.c_str(), data.c_str());
}