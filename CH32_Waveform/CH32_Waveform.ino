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
        strncpy(wifiCfg.profiles[0].ssid, "testttt", 32);
        strncpy(wifiCfg.profiles[0].password, "12345678", 63);
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

// ========== 二进制协议帧识别（只拦截 PING 命令帧并回 PONG，其余全透传MCU） ==========
#define PROTO_AA 0xAA
#define PROTO_55 0x55
#define PROTO_TAIL 0x7E
#define PROTO_ESC 0x7D
#define PROTO_XOR 0x20
#define PROTO_ADDR_MCU 0x01
#define PROTO_CMD_PING 0x30
#define PROTO_CMD_PONG 0x31
#define PROTO_BODY_BASE 8 // addr + cmd + len(2) + ts(4)

static uint8_t fsmState = 0;
static uint8_t fsmBody[128];
static uint16_t fsmBodyLen = 0;

static uint16_t fsmChecksum16(const uint8_t *data, uint16_t len) {
    uint16_t sum = 0;
    for (uint16_t i = 0; i < len; i++) sum = (uint16_t)(sum + data[i]);
    return sum;
}

static void sendBinaryPong() {
    uint8_t body[16];
    uint16_t bodyLen = 0;
    uint32_t ts = millis();
    body[bodyLen++] = PROTO_ADDR_MCU;
    body[bodyLen++] = PROTO_CMD_PONG;
    body[bodyLen++] = 0x01; // len lo
    body[bodyLen++] = 0x00; // len hi
    body[bodyLen++] = (uint8_t)(ts & 0xFF);
    body[bodyLen++] = (uint8_t)((ts >> 8) & 0xFF);
    body[bodyLen++] = (uint8_t)((ts >> 16) & 0xFF);
    body[bodyLen++] = (uint8_t)((ts >> 24) & 0xFF);
    body[bodyLen++] = PROTO_CMD_PONG; // payload[0]
    uint16_t crc = fsmChecksum16(body, bodyLen);
    body[bodyLen++] = (uint8_t)(crc & 0xFF);
    body[bodyLen++] = (uint8_t)((crc >> 8) & 0xFF);

    uint8_t frame[32];
    uint16_t idx = 0;
    frame[idx++] = PROTO_AA;
    frame[idx++] = PROTO_55;
    for (uint16_t i = 0; i < bodyLen; i++) {
        uint8_t b = body[i];
        if (b == PROTO_AA || b == PROTO_55 || b == PROTO_TAIL || b == PROTO_ESC) {
            frame[idx++] = PROTO_ESC;
            frame[idx++] = (uint8_t)(b ^ PROTO_XOR);
        } else {
            frame[idx++] = b;
        }
    }
    frame[idx++] = PROTO_TAIL;
    tcpClient.write(frame, idx);
}

// 返回 1 表示识别为 PING 并已回复（不转发），返回 0 表示需要透传。
static uint8_t tryHandleCompleteFrame() {
    if (fsmBodyLen < PROTO_BODY_BASE + 2) return 0;
    uint16_t payloadLen = fsmBody[2] | ((uint16_t)fsmBody[3] << 8);
    if (fsmBodyLen < PROTO_BODY_BASE + payloadLen + 2) return 0;
    uint16_t calc = fsmChecksum16(fsmBody, PROTO_BODY_BASE + payloadLen);
    uint16_t got = fsmBody[PROTO_BODY_BASE + payloadLen]
            | ((uint16_t)fsmBody[PROTO_BODY_BASE + payloadLen + 1] << 8);
    if (calc != got) return 0;
    if (fsmBody[PROTO_BODY_BASE] == PROTO_CMD_PING) { // payload[0] == CMD_PING
        sendBinaryPong();
        return 1;
    }
    return 0;
}

static void checkBinaryFrameAndForward(uint8_t b) {
    switch (fsmState) {
        case 0:
            if (b == PROTO_AA) {
                fsmState = 1;
            } else {
                Serial.write(b);
            }
            break;
        case 1:
            if (b == PROTO_55) {
                fsmState = 2;
                fsmBodyLen = 0;
            } else if (b == PROTO_AA) {
                fsmState = 1;
            } else {
                fsmState = 0;
                Serial.write(PROTO_AA);
                Serial.write(b);
            }
            break;
        case 2:
            if (b == PROTO_ESC) {
                fsmState = 3;
            } else if (b == PROTO_TAIL) {
                uint8_t handled = tryHandleCompleteFrame();
                if (!handled) {
                    Serial.write(PROTO_AA);
                    Serial.write(PROTO_55);
                    for (uint16_t i = 0; i < fsmBodyLen; i++) {
                        Serial.write(fsmBody[i]);
                    }
                    Serial.write(PROTO_TAIL);
                }
                fsmState = 0;
                fsmBodyLen = 0;
            } else {
                if (fsmBodyLen < sizeof(fsmBody)) {
                    fsmBody[fsmBodyLen++] = b;
                } else {
                    fsmState = 0;
                }
            }
            break;
        case 3:
            if (fsmBodyLen < sizeof(fsmBody)) {
                fsmBody[fsmBodyLen++] = (uint8_t)(b ^ PROTO_XOR);
                fsmState = 2;
            } else {
                fsmState = 0;
            }
            break;
    }
}

// ★★★ 修改后的 WiFi 连接函数 ★★★
void connectToWiFi() {
    WiFi.mode(WIFI_STA);

    // ---- 第一步：优先尝试固定 SSID/密码 ----
    WiFi.begin("jianxin", "123456789");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) { // 10秒超时
        delay(500);
    }
    if (WiFi.status() == WL_CONNECTED) {
        serverIP = WiFi.gatewayIP();
        return;  // 连接成功，直接返回
    }

    // ---- 第二步：若失败，回退到 EEPROM 中存储的配置 ----
    loadWifiConfig();
    saveWifiConfig();   // 确保默认配置被写入（仅首次）

    int idx = wifiCfg.active_idx;
    if (idx >= MAX_WIFI_PROFILES || !wifiCfg.profiles[idx].valid) idx = 0;

    WiFi.begin(wifiCfg.profiles[idx].ssid, wifiCfg.profiles[idx].password);
    start = millis();
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

    // 1. TCP下行优先：识别二进制 PING 帧，其余透传到MCU串口
    while (tcpClient.available()) {
        uint8_t buf[128];
        size_t len = tcpClient.available();
        if (len > sizeof(buf)) len = sizeof(buf);
        len = tcpClient.readBytes(buf, len);
        for (size_t i = 0; i < len; i++) {
            checkBinaryFrameAndForward(buf[i]);
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
