#include <ESP8266WiFi.h>
#include <EEPROM.h>

// ========== EEPROM WiFi配置存储 ==========
#define EEPROM_SIZE 512
#define WIFI_CFG_MAGIC 0xEE01
#define WIFI_CFG_ADDR  0
#define MAX_WIFI_PROFILES 5

typedef struct {
    char ssid[33];
    char password[64];
    uint8_t valid;
    uint8_t is_default;
} WifiProfile;

typedef struct {
    uint16_t magic;
    uint8_t  count;
    uint8_t  active_idx;
    WifiProfile profiles[MAX_WIFI_PROFILES];
} WifiConfig;

static WifiConfig wifiCfg;
static bool wifiCfgDirty = false;

static void loadWifiConfig() {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(WIFI_CFG_ADDR, wifiCfg);
    if (wifiCfg.magic != WIFI_CFG_MAGIC || wifiCfg.count > MAX_WIFI_PROFILES) {
        wifiCfg.magic = WIFI_CFG_MAGIC;
        wifiCfg.count = 1;
        wifiCfg.active_idx = 0;
        memset(wifiCfg.profiles, 0, sizeof(wifiCfg.profiles));
        strncpy(wifiCfg.profiles[0].ssid, "jianxin", 32);
        strncpy(wifiCfg.profiles[0].password, "123456789", 63);
        wifiCfg.profiles[0].valid = 1;
        wifiCfg.profiles[0].is_default = 1;
        wifiCfgDirty = true;
    }
}

static void saveWifiConfig() {
    if (!wifiCfgDirty) return;
    EEPROM.put(WIFI_CFG_ADDR, wifiCfg);
    EEPROM.commit();
    wifiCfgDirty = false;
}

// ========== 网络连接 ==========
const uint16_t SERVER_PORT = 41002;
IPAddress serverIP;
WiFiClient tcpClient;

// ========== PING检测（最小化，只拦截"PING\n"五个字节，其余全透传） ==========
static uint8_t pingState = 0;
static const uint8_t PING_PATTERN[] = {'P','I','N','G','\n'};

static void checkPingAndForward(uint8_t b) {
    if (b == PING_PATTERN[pingState]) {
        pingState++;
        if (pingState == 5) {
            tcpClient.print("PONG\n");
            pingState = 0;
            return;
        }
    } else {
        if (pingState > 0) {
            for (uint8_t i = 0; i < pingState; i++) {
                Serial.write(PING_PATTERN[i]);
            }
            pingState = 0;
            if (b == PING_PATTERN[0]) {
                pingState = 1;
                return;
            }
        }
        Serial.write(b);
    }
}

void connectToWiFi() {
    loadWifiConfig();
    saveWifiConfig();

    int idx = wifiCfg.active_idx;
    if (idx >= MAX_WIFI_PROFILES || !wifiCfg.profiles[idx].valid) idx = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiCfg.profiles[idx].ssid, wifiCfg.profiles[idx].password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED) {
        serverIP = WiFi.gatewayIP();
    }
}

bool connectToServer() {
    if (!tcpClient.connected()) {
        tcpClient.setNoDelay(true);
        if (tcpClient.connect(serverIP, SERVER_PORT)) return true;
        delay(2000);
        return false;
    }
    return true;
}

void setup() {
    Serial.begin(2000000);
    Serial.setRxBufferSize(4096);
    connectToWiFi();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        connectToWiFi();
        return;
    }

    if (!connectToServer()) return;

    // 1. TCP下行优先：透传到MCU串口（只拦截PING）
    while (tcpClient.available()) {
        uint8_t buf[128];
        size_t len = tcpClient.available();
        if (len > sizeof(buf)) len = sizeof(buf);
        len = tcpClient.readBytes(buf, len);
        for (size_t i = 0; i < len; i++) {
            checkPingAndForward(buf[i]);
        }
    }

    // 2. 串口→TCP上行透传
    if (Serial.available()) {
        size_t len = Serial.available();
        uint8_t buf[256];
        if (len > sizeof(buf)) len = sizeof(buf);
        len = Serial.readBytes(buf, len);
        tcpClient.write(buf, len);
    }

    yield();

    if (!tcpClient.connected()) delay(100);
}
