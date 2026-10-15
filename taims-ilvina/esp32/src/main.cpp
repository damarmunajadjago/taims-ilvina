/*
 * Firmware ESP32 Heartbeat - T-AIMS
 * Teknolab Asset & Infrastructure Monitoring System
 * 
 * Author : Ilvina Muvarika
 * Shift  : 1
 * Date   : 14 Oktober 2026
 */

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

// ============ KONFIGURASI ============
const char* WIFI_SSID       = "espnow";
const char* WIFI_PASSWORD   = "icayangku";

const char* MQTT_BROKER = "10.181.43.144";  // ← GANTI dari 192.168.1.43
const int   MQTT_PORT       = 1883;

const char* DEVICE_ID       = "IOT-ESP-02";
const char* MQTT_TOPIC      = "taims/heartbeat/IOT-ESP-02";

const unsigned long HEARTBEAT_INTERVAL = 30000;
const char* FIRMWARE_VERSION = "1.0.0";

// ============ OBJEK ============
WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastHeartbeat = 0;
unsigned long bootTime = 0;

// ============ FUNGSI WIFI ============
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("[WIFI] Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" OK!");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WIFI] RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println(" FAILED!");
  }
}

// ============ FUNGSI MQTT ============
void connectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("[MQTT] Connecting... ");

    // LWT di-set di parameter connect()
    if (mqttClient.connect(
        DEVICE_ID,                                              // clientId
        NULL,                                                   // username
        NULL,                                                   // password
        MQTT_TOPIC,                                             // willTopic
        1,                                                      // willQoS
        false,                                                  // willRetain
        "{\"status\":\"offline\",\"reason\":\"connection_lost\"}", // willMessage
        true                                                    // cleanSession
    )) {
      Serial.println("OK!");
    } else {
      Serial.print("FAILED, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" — retry in 3s");
      delay(3000);
    }
  }
}

// ============ FUNGSI HEARTBEAT ============
void sendHeartbeat() {
  StaticJsonDocument<512> doc;

  // Timestamp
  time_t now = time(nullptr);
  struct tm timeinfo;
  char timestamp[30];

  if (now > 1700000000) {
    localtime_r(&now, &timeinfo);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S+07:00", &timeinfo);
    doc["timestamp"] = timestamp;
  } else {
    doc["timestamp"] = String(millis());
  }

  // Payload lengkap
  doc["device_id"]        = DEVICE_ID;
  doc["status"]           = "online";
  doc["ip_address"]       = WiFi.localIP().toString();
  doc["rssi"]             = WiFi.RSSI();
  doc["uptime_seconds"]   = (millis() - bootTime) / 1000;
  doc["firmware_version"] = FIRMWARE_VERSION;

  char payload[512];
  serializeJson(doc, payload, sizeof(payload));

  // Publish dengan QoS 1
  bool sent = mqttClient.publish(
    MQTT_TOPIC,
    (const uint8_t*)payload,
    strlen(payload),
    false
  );

  if (sent) {
    Serial.print("[OK] Sent: ");
    Serial.println(payload);
  } else {
    Serial.println("[FAIL] Publish gagal!");
  }
}

// ============ SETUP ============
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println("  T-AIMS ESP32 Heartbeat");
  Serial.print("  Device: ");
  Serial.println(DEVICE_ID);
  Serial.println("================================");

  bootTime = millis();

  connectWiFi();

  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setKeepAlive(30);
  mqttClient.setBufferSize(512);

  connectMQTT();

  sendHeartbeat();
  lastHeartbeat = millis();
}

// ============ LOOP ============
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WARN] WiFi terputus — reconnect");
    connectWiFi();
  }

  if (!mqttClient.connected()) {
    Serial.println("[WARN] MQTT terputus — reconnect");
    connectMQTT();
    sendHeartbeat();
    lastHeartbeat = millis();
  }

  mqttClient.loop();

  if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }

  delay(10);
}