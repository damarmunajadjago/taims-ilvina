#ifndef CONFIG_H
#define CONFIG_H

// ===== WIFI =====
#define WIFI_SSID   "NamaWiFiKamu"
#define WIFI_PASS   "PasswordWiFiKamu"

// ===== MQTT =====
#define MQTT_HOST   "192.168.1.10"
#define MQTT_PORT   1883
#define MQTT_USER   ""
#define MQTT_PASS   ""

// ===== DEVICE =====
#define DEVICE_ID   "IOT-ESP-01"
#define FIRMWARE    "1.0.0"
#define TOPIC_HEARTBEAT "taims/heartbeat/IOT-ESP-01"

// ===== TIMING =====
#define HEARTBEAT_INTERVAL_MS 30000

#endif