#include "EasyWifi-Globals.h"

String getServiceByName(String strName,int index)
{
    if(strName.equals(String("Ws2812b-Lightbulb")))
    {
        return String("Lightbulb");
    }
    if (strName.equals(String("DHT11")) && index == 0)
    {
        return String("HumiditySensor");
    }
    if (strName.equals(String("DHT11")) && index == 1)
    {
        return String("TemperatureSensor");
    }
    return strName;
}

String getName(String strName,int index = 0)
{
    String strNewName;

    if (index > 0)
    {
        strNewName = String(strName + String(index));
    }
    else
    {
        strNewName = strName;
    }
    //LOG(strNewName);
    return strNewName;
}

int getAccessoryNum(Config &config)
{
    // 如果一个esp设备要加两个传感器
    if (config.strServiceName.equals(String("DHT11")))
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void initAccessory(Config &config)
{
    if (config.strServiceName.equals(String("Switch")))
    {
        pinMode(SWITCH_PIN, OUTPUT);
    }
    if(config.strServiceName.equals(String("Ws2812b-Lightbulb")))
    {
        LOG("init Ws2812b Accessory");
        ws2812fx.init();
        ws2812fx.setBrightness(128);
        ws2812fx.setSegment(0,  0,  7, FX_MODE_BLINK, 0xFF0000, 1000, false); // segment 0 is leds 0 - 9
    }
    if (config.strServiceName.equals(String("DHT11")))
    {
        LOG("init DHT 11");
        dht.begin();
    }
}

void addAccessory()
{
    Config config;
    loadConfig(config);

    // 判断ServcieName需要追加几个Accessory
    int num = getAccessoryNum(config);

    for (int index =0; index < num; index++)
    {
        String addTopic = String(config.strTopicPrefix + "/to/add");
        String data;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        String name = getName(config.strDeviceName,index);
        json["name"] = name.c_str();
        json["service_name"] = config.strServiceName.c_str();

        String strService = getServiceByName(config.strServiceName ,index);
        json["service"] = strService.c_str();


        if (strService.equals(String("Lightbulb")))
        {
            json["Brightness"] = "default";
        }
        json.printTo(data);

        Serial.print("addAccessory: ");
        LOG(data);
        MQTTclient.publish(addTopic.c_str(), data.c_str());
    }
    // 初始化pin 脚模式
    initAccessory(config);

}

void removeAccessory()
{
    Config config;
    loadConfig(config);
   // 判断ServcieName需要追加几个Accessory
    int num = getAccessoryNum(config);

    for (int index =0; index < num; index++)
    {
        String addTopic = String(config.strTopicPrefix + "/to/remove");
        String data;
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        String name = getName(config.strDeviceName,index);
        json["name"] = name.c_str();
        json.printTo(data);
        LOG(name);

        Serial.print("removeAccessory: ");
        LOG(data);
        MQTTclient.publish(addTopic.c_str(), data.c_str());
    }
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

void setDHT11HumidityValue2Homebridge(float fHumidity,JsonObject& in_json)
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/set");
    String data;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = in_json["name"].asString();
    json["service_name"] = in_json["service_name"].asString();
    json["characteristic"] = in_json["characteristic"].asString();
    json["value"] = fHumidity;
    json.printTo(data);

    Serial.print("public Humidity status to Homebridge: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
}

void setDHT11TemperatureValue2Homebridge(float fTemperature,JsonObject& in_json)
{
    Config config;
    loadConfig(config);

    String addTopic = String(config.strTopicPrefix + "/to/set");
    String data;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["name"] = in_json["name"].asString();
    json["service_name"] = in_json["service_name"].asString();
    json["characteristic"] = in_json["characteristic"].asString();
    json["value"] = fTemperature;
    json.printTo(data);

    Serial.print("public Temperature status to Homebridge: ");
    LOG(data);
    MQTTclient.publish(addTopic.c_str(), data.c_str());
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
        if (String("DHT11").equals(json["service_name"].asString()))
        {
            getDHT11Sensor(json);
        }
    } 
    // homebrige设置值到其它设备
    else if (strTopic == String(config.strTopicPrefix + "/from/set"))
    {
        Serial.print("homebrige ==>Set: ");
        LOG(strJsondata);
        
        if (config.strDeviceName.equals(json["name"].asString()))
        {
            // 用Service Name 区分是什么功能
            if (String("Switch").equals(json["service_name"].asString()))
            {
                setSwitch(json);
            }
            else if (String("Lightbulb").equals(json["service_name"].asString()))
            {
                setLightbulb(json);
            }
            else if (String("Ws2812b-Lightbulb").equals(json["service_name"].asString()))
            {
                setWs2812bLightbulb(json);
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
