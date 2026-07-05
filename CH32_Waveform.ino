#include <ESP8266WiFi.h>

// ========== 配置（手机热点）==========
const char *WIFI_SSID = "testttt";
const char *WIFI_PASSWORD = "12345678";

// 端口固定
const uint16_t SERVER_PORT = 41002;

// 目标 IP 将自动设置为网关（手机热点 IP）
IPAddress serverIP;

WiFiClient tcpClient;

void connectToWiFi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    // 自动获取网关 IP
    serverIP = WiFi.gatewayIP();
}

bool connectToServer() {
    if (!tcpClient.connected()) {
        if (tcpClient.connect(serverIP, SERVER_PORT)) {
            return true;
        } else {
            delay(2000);
            return false;
        }
    }
    return true;
}

void setup() {
    Serial.begin(2000000);          // 与 CH32H417 串口波特率一致
    connectToWiFi();
}

void loop() {
    // 保持 TCP 连接
    if (!connectToServer()) return;
    
    // 读取串口全部可用数据，直接转发到 TCP
    while (Serial.available()) {
        size_t len = Serial.available();
        uint8_t buf[len];
        Serial.readBytes(buf, len);
        tcpClient.write(buf, len);
    }

    // 如果连接意外断开，主循环会重连
    if (!tcpClient.connected()) {
        delay(100);
    }
}