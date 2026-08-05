// ========== 编译开关: SD卡测试 / WiFi透传 二选一 ==========
// 1 = SD卡检测测试模式(不连WiFi), 0 = WiFi透传模式
#define SD_CARD_TEST 1

#if SD_CARD_TEST
// ============================================================
// SD 卡检测测试模式
// ============================================================
#include <ESP8266WiFi.h>
#include <SPI.h>

// ========== SD卡引脚(原理图CARD1) ==========
#define SD_CS_PIN 15   // GPIO15 = CDDAT3(CS)  注意:上电时必须为低,否则无法从Flash启动
// GPIO14 = SCLK (CLK)  — HSPI CLK
// GPIO13 = MOSI (CMD)  — HSPI MOSI
// GPIO12 = MISO (DAT0) — HSPI MISO

uint8_t g_r1 = 0xFF;
uint8_t g_resp[4];

// 发送命令(带CRC), 可选读响应数据字节
uint8_t sdSendCmd(uint8_t cmd, uint32_t arg, uint8_t crc, int readBytes) {
    digitalWrite(SD_CS_PIN, LOW);
    SPI.transfer(0x40 | cmd);
    SPI.transfer((uint8_t)(arg >> 24));
    SPI.transfer((uint8_t)(arg >> 16));
    SPI.transfer((uint8_t)(arg >> 8));
    SPI.transfer((uint8_t)(arg));
    SPI.transfer(crc);
    uint8_t r1 = 0xFF;
    for (int i = 0; i < 20; i++) {
        r1 = SPI.transfer(0xFF);
        if (r1 != 0xFF) break;
    }
    for (int i = 0; i < readBytes && i < 4; i++) g_resp[i] = SPI.transfer(0xFF);
    digitalWrite(SD_CS_PIN, HIGH);
    SPI.transfer(0xFF); // 补时钟
    return r1;
}

// 单块读: CMD17, 返回0=成功
int sdReadBlock(uint32_t addr, uint8_t *buf) {
    digitalWrite(SD_CS_PIN, LOW);
    SPI.transfer(0x51); // CMD17
    SPI.transfer((uint8_t)(addr >> 24));
    SPI.transfer((uint8_t)(addr >> 16));
    SPI.transfer((uint8_t)(addr >> 8));
    SPI.transfer((uint8_t)(addr));
    SPI.transfer(0x01);
    uint8_t r1 = 0xFF;
    for (int i = 0; i < 20; i++) {
        r1 = SPI.transfer(0xFF);
        if (r1 != 0xFF) break;
    }
    if (r1 != 0x00) { digitalWrite(SD_CS_PIN, HIGH); SPI.transfer(0xFF); return -1; }

    uint8_t token = 0xFF;
    for (int i = 0; i < 64; i++) {
        token = SPI.transfer(0xFF);
        if (token != 0xFF) break;
    }
    if (token != 0xFE) { digitalWrite(SD_CS_PIN, HIGH); SPI.transfer(0xFF); return -2; }

    for (int i = 0; i < 512; i++) buf[i] = SPI.transfer(0xFF);
    SPI.transfer(0xFF); SPI.transfer(0xFF); // CRC
    digitalWrite(SD_CS_PIN, HIGH);
    SPI.transfer(0xFF);
    return 0;
}

// 单块写: CMD24, 返回0=成功
int sdWriteBlock(uint32_t addr, const uint8_t *buf) {
    digitalWrite(SD_CS_PIN, LOW);
    SPI.transfer(0x58); // CMD24
    SPI.transfer((uint8_t)(addr >> 24));
    SPI.transfer((uint8_t)(addr >> 16));
    SPI.transfer((uint8_t)(addr >> 8));
    SPI.transfer((uint8_t)(addr));
    SPI.transfer(0x01);
    uint8_t r1 = 0xFF;
    for (int i = 0; i < 20; i++) {
        r1 = SPI.transfer(0xFF);
        if (r1 != 0xFF) break;
    }
    if (r1 != 0x00) { digitalWrite(SD_CS_PIN, HIGH); SPI.transfer(0xFF); return -1; }

    SPI.transfer(0xFE); // 起始token
    for (int i = 0; i < 512; i++) SPI.transfer(buf[i]);
    SPI.transfer(0xFF); SPI.transfer(0xFF); // CRC(dummy)

    uint8_t dataResp = SPI.transfer(0xFF);
    if ((dataResp & 0x1F) != 0x05) { digitalWrite(SD_CS_PIN, HIGH); SPI.transfer(0xFF); return -2; }

    for (int i = 0; i < 100000; i++) {
        uint8_t b = SPI.transfer(0xFF);
        if (b == 0xFF) break;
    }
    digitalWrite(SD_CS_PIN, HIGH);
    SPI.transfer(0xFF);
    return 0;
}

void sdCardTest() {
    Serial.println("\r\n===== SD Card Test (ESP8266) =====");

    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    digitalWrite(SD_CS_PIN, HIGH);
    for (int i = 0; i < 80; i++) {
        SPI.transfer(0xFF);
    }

    uint8_t r0 = sdSendCmd(0, 0, 0x95, 0);
    Serial.printf("[SD] CMD0   R1=0x%02X\r\n", r0);
    if (r0 != 0x01) {
        Serial.println("[SD] CMD0 failed - abort");
        Serial.println("===================================");
        return;
    }

    uint8_t r8 = sdSendCmd(8, 0x000001AA, 0x87, 4);
    Serial.printf("[SD] CMD8   R1=0x%02X resp=%02X %02X %02X %02X\r\n",
                  r8, g_resp[0], g_resp[1], g_resp[2], g_resp[3]);

    uint8_t r41 = 0xFF;
    for (int i = 0; i < 100; i++) {
        uint8_t r55 = sdSendCmd(55, 0, 0x01, 0);
        if (r55 != 0x01) break;
        r41 = sdSendCmd(41, 0x40000000, 0x01, 0);
        if (r41 == 0x00) break;
    }
    Serial.printf("[SD] ACMD41 R1=0x%02X\r\n", r41);
    if (r41 != 0x00) {
        Serial.println("[SD] ACMD41 failed - abort");
        Serial.println("===================================");
        return;
    }

    SPI.setClockDivider(SPI_CLOCK_DIV8);

    uint32_t testSector = 1;
    uint32_t byteAddr = testSector * 512;

    uint8_t wbuf[512];
    uint8_t rbuf[512];
    for (int i = 0; i < 512; i++) wbuf[i] = (uint8_t)(i & 0xFF);

    Serial.printf("[SD] Write block @sector %lu...\r\n", (unsigned long)testSector);
    int wr = sdWriteBlock(byteAddr, wbuf);
    Serial.printf("[SD] Write ret=%d (0=OK)\r\n", wr);

    Serial.printf("[SD] Read block @sector %lu...\r\n", (unsigned long)testSector);
    int rd = sdReadBlock(byteAddr, rbuf);
    Serial.printf("[SD] Read  ret=%d (0=OK)\r\n", rd);

    if (wr == 0 && rd == 0) {
        int match = 1;
        for (int i = 0; i < 512; i++) {
            if (wbuf[i] != rbuf[i]) { match = 0; break; }
        }
        Serial.printf("[SD] Verify: %s\r\n", match ? "PASS (512B match)" : "FAIL (data mismatch)");
    } else {
        Serial.println("[SD] Read/Write FAIL");
    }

    Serial.println("===================================");
}

void setup() {
    Serial.begin(2000000);
    Serial.setRxBufferSize(4096);
}

void loop() {
    sdCardTest();
    delay(2000);
}

#else
// ============================================================
// WiFi 透传模式 (参照 WIFI.ino)
// ============================================================
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
#endif
