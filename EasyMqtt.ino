#include "EasyWifi-Globals.h"

void addAccessory()
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/add");
    String data;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = config.strDeviceName.c_str();
    json["service_name"] = config.strServiceName.c_str();
    json["service"] = config.strServiceName.c_str();

    if (config.strServiceName.equals(String("Lightbulb")))
    {
        json["Brightness"] = "default";
    }
    json.printTo(data);

    Serial.print("addAccessory: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void removeAccessory()
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/remove");
    String data;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = config.strDeviceName.c_str();
    json.printTo(data);

    Serial.print("removeAccessory: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void setSwitchValue2Homebridge(bool value)
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/set");
    String data;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = config.strDeviceName.c_str();
    json["service_name"] = config.strServiceName.c_str();
    json["characteristic"] = "On";
    json["value"] = value;
    json.printTo(data);

    Serial.print("public Switch status to Homebridge: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void setSwitch(bool On)
{
    LOG("setSwitch");
    if (On)
    {
        digitalWrite(SWITCH_PIN,HIGH);
    }
    else
    {
        digitalWrite(SWITCH_PIN,LOW);
    }
}

void setLightbulbOn(bool On)
{
    LOG("setLightbulbOn:");
    LOG(On);
}

void setLightbulbBrightness(int Brightness)
{
    LOG("setLightbulbBrightness:");
    LOG(Brightness);
}

void setFan()
{

}

void recv(char* topic, byte* payload, unsigned int length)
{
    Config config;
    loadConfig(config);

    LOG(topic);
    String strTopic = topic;
    String strJsondata = (char*)payload;

    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(strJsondata);

    if (!json.success()) {
        LOG("Failed to parse recv json data");
        return;
    } 
    
    if (strTopic == String(config.strTopicPrefix + "/from/get"))
    {
        Serial.print("homebrige ==>Get: ");
        LOG(strJsondata);
    } 
    // homebrige设置值到其它设备
    else if (strTopic == String(config.strTopicPrefix + "/from/set"))
    {
        Serial.print("homebrige ==>Set: ");
        LOG(strJsondata);
        
        if (config.strDeviceName.equals(json["name"].asString()))
        {
            if (String("Switch").equals(json["service_name"].asString()))
            {
                setSwitch(json["value"].as<bool>());
            }
            else if (String("Lightbulb").equals(json["service_name"].asString()))
            {
                if (String("On").equals(json["characteristic"].asString()))
                {
                    setLightbulbOn(json["value"].as<bool>());
                }
                if (String("Brightness").equals(json["characteristic"].asString()))
                {
                    setLightbulbBrightness(json["value"].as<int>());
                }
            }
            else if (String("Fan").equals(json["service_name"].asString()))
            {
                
            }
            else
            {
                LOG(json["service_name"].asString());
            }
        }
    }
    // 收到homebrige的response
    else if(strTopic == String(config.strTopicPrefix + "/from/response"))
    {
        Serial.print("homebrige ==>response: ");
        LOG(strJsondata);


    }
    else
    {

    }
    

}


void send(String topic,String data)
{   
    MQTTclient.publish(topic.c_str(), data.c_str());
}