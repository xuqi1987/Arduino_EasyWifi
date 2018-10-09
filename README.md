## Summary

没有找到合适的简单解决方案，将Esp8266控制的设备连接到HomeKit.
所以参照EspEasy项目，设计了一个解决方案。

![](./img/4.png)


我测试的设备  | 
------------- | 
[NodeMcu](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.70762e8dRulKV9&id=533795236140&_u=911aik5dcf48) | 
[ESP-01S](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.70762e8dRulKV9&id=560359232453&_u=911aik5d6366) | 
[mini D1 wifi](https://item.taobao.com/item.htm?spm=a1z10.3-c-s.w4002-14787471870.9.6cd06865Y9ewEe&id=533672579285) |


## Features

* 可通过浏览器访问192.168.4.1,配置以下信息
 + 设备名称
 + MQTT服务器
 + 需要连接的ssid和密码
 + 发布和订阅的主题

![](./img/2.png)
![](./img/3.png)

## Quickstart


***

