## Summary

没有找到合适的简单解决方案，将Esp8266控制的设备连接到HomeKit.
所以参照EspEasy实现 HomeKit和Esp8266连接。

### 连接方式：

![](./img/4.png)

##### Raspberry Zero

Raspberry 安装homebridge-mqtt：
[https://www.npmjs.com/package/homebridge-mqtt](https://www.npmjs.com/package/homebridge-mqtt)

##### 阿里云服务器

云服务器上跑以下Docker
[https://store.docker.com/images/eclipse-mosquitto](https://store.docker.com/images/eclipse-mosquitto)

##### Esp8266

Esp8266上使用Arduino烧写本项目的EasyWifi.

### 测试的设备：

+ [NodeMcu](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.70762e8dRulKV9&id=533795236140&_u=911aik5dcf48) 
+ [ESP-01S](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.70762e8dRulKV9&id=560359232453&_u=911aik5d6366)  
+ [mini D1 wifi](https://item.taobao.com/item.htm?spm=a1z10.3-c-s.w4002-14787471870.9.6cd06865Y9ewEe&id=533672579285) 


## Features

* 可通过浏览器访问192.168.4.1,配置以下信息
 + 设备名称
 + MQTT服务器
 + 需要连接的ssid和密码
 + 发布和订阅的主题
 + 设备连接后，自动添加到HomeBridge

![](./img/2.png)
![](./img/3.png)

## Quickstart

#### 树莓派安装homebridge

**~/.homebridge/config.json**

```
{
    "bridge": {
    "name": "Homebridge",
    "username": "B8:E8:56:17:E3:58",
    "port": 51825,
    "pin": "123-11-122"
    },
    
    "description": "This is an example configuration file with pilight plugin.",

    
  "platforms": [
    {
        "platform": "mqtt",
  	"name": "mqtt",
  	"url": "mqtt://lot-xu.top",
  	"port": "1883",
  	"topic_type": "multiple",
  	"topic_prefix": "homebridge",
  	"username": "",
  	"password": "",
  	"qos": 1
    }]
}

```
![](./img/6.jpg)
![](./img/5.jpg)


***

