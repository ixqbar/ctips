## version
```
v0.0.1
```

## config
```
[General]
url="ws://127.0.0.1:8899/sock?proxy=0"

[http]
minThreads=1
port=7878
```
* cat ~/.config/caoxianjie/ctips.ini


## by local http server
```
curl --data "this is a test message" 'http://localhost:7878/'
```

## by remote websocket server
```
<?php

$redis_handle = new Redis();
$redis_handle->connect('127.0.0.1', 6899);
$redis_handle->set("*", '{"noticeData":{"message":"nice"}}');
```
* https://github.com/jonnywang/fcgiProxy

![](screenshot/1524046586901.png)
![](screenshot/1524046912758.png)
![](screenshot/1524046959081.png)


## thanks
* qt
* http://stefanfrings.de/qtwebapp/index.html
