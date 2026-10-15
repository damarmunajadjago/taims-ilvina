"""
Heartbeat Agent untuk Raspberry Pi
T-AIMS - Teknolab Asset & Infrastructure Monitoring System

Author : Ilvina Muvarika
Shift  : 1
Date   : 14 Oktober 2026

Jalankan: python3 heartbeat.py
"""

import paho.mqtt.client as mqtt
import json
import time
import socket
from datetime import datetime, timezone, timedelta

# ============ KONFIGURASI ============
MQTT_HOST = "localhost"
MQTT_PORT = 1883
MQTT_USER = None
MQTT_PASS = None

DEVICE_ID = "IOT-RPI-01"
TOPIC     = f"taims/heartbeat/{DEVICE_ID}"
INTERVAL  = 30

# ============ UTILITAS ============
def get_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except:
        return "0.0.0.0"

def on_connect(client, userdata, flags, rc):
    print(f"[MQTT] Connected, rc={rc}")

def on_disconnect(client, userdata, rc):
    print(f"[MQTT] Disconnected, rc={rc}")

# ============ SETUP MQTT CLIENT ============
client = mqtt.Client(client_id=f"RPI-{DEVICE_ID}")

# LWT — HANYA 1 KALI
lwt_payload = json.dumps({
    "status": "offline",
    "reason": "connection_lost"
})
client.will_set(TOPIC, lwt_payload, qos=1, retain=False)

if MQTT_USER:
    client.username_pw_set(MQTT_USER, MQTT_PASS)

client.on_connect = on_connect
client.on_disconnect = on_disconnect

boot_time = time.monotonic()
wib = timezone(timedelta(hours=7))

# ============ KONEKSI ============
print(f">> Heartbeat RPi untuk {DEVICE_ID}")
print(f"   Broker  : {MQTT_HOST}:{MQTT_PORT}")
print(f"   Topic   : {TOPIC}")
print(f"   Interval: {INTERVAL}s")
print("-" * 45)

client.connect(MQTT_HOST, MQTT_PORT, 60)
client.loop_start()

# ============ MAIN LOOP ============
try:
    while True:
        payload = {
            "device_id":        DEVICE_ID,           # ← DITAMBAH
            "status":           "online",
            "timestamp":        datetime.now(wib).isoformat(),
            "ip_address":       get_ip(),
            "uptime_seconds":   int(time.monotonic() - boot_time),
            "firmware_version": "1.0.0"
        }
        
        result = client.publish(TOPIC, json.dumps(payload), qos=1)
        
        if result.rc == 0:
            print(f"[OK] [{datetime.now(wib).strftime('%H:%M:%S')}] Sent")
        else:
            print(f"[FAIL] Publish error, rc={result.rc}")
        
        time.sleep(INTERVAL)
        
except KeyboardInterrupt:
    print("\n>> Dihentikan user")
    client.loop_stop()
    client.disconnect()