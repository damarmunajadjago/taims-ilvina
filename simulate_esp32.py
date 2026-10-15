"""
Simulator ESP32 untuk T-AIMS Heartbeat
Meniru perilaku ESP32 yang kirim heartbeat via MQTT.
Jalankan: python simulate_esp32.py
"""
import paho.mqtt.client as mqtt
import json, time, socket
from datetime import datetime, timezone, timedelta

# ===== KONFIGURASI =====
MQTT_HOST = "localhost"
MQTT_PORT = 1883
MQTT_USER = None
MQTT_PASS = None
DEVICE_ID = "IOT-ESP-01"
TOPIC     = f"taims/heartbeat/{DEVICE_ID}"
INTERVAL  = 30
# ========================

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

client = mqtt.Client(client_id=f"SIM-{DEVICE_ID}")
if MQTT_USER:
    client.username_pw_set(MQTT_USER, MQTT_PASS)
client.on_connect = on_connect
client.on_disconnect = on_disconnect

# LWT: kalau simulator mati mendadak
lwt = json.dumps({"status": "offline", "reason": "connection_lost"})
client.will_set(TOPIC, lwt, qos=1, retain=False)

boot_time = time.monotonic()
wib = timezone(timedelta(hours=7))

print(f">> Simulator ESP32 untuk {DEVICE_ID}")
print(f"   Broker: {MQTT_HOST}:{MQTT_PORT}")
print(f"   Topic : {TOPIC}")
print(f"   Interval: {INTERVAL}s")
print("-" * 50)

client.connect(MQTT_HOST, MQTT_PORT, 60)
client.loop_start()

try:
    while True:
        payload = {
            "status":           "online",
            "timestamp":        datetime.now(wib).isoformat(),
            "ip_address":       get_ip(),
            "rssi":             -67,
            "uptime_seconds":   int(time.monotonic() - boot_time),
            "firmware_version": "SIM-1.0.0"
        }
        result = client.publish(TOPIC, json.dumps(payload), qos=1)
        if result.rc == 0:
            print(f"[OK] [{datetime.now().strftime('%H:%M:%S')}] Sent")
        else:
            print(f"[GAGAL] rc={result.rc}")
        time.sleep(INTERVAL)

except KeyboardInterrupt:
    print("\n>> Simulator dihentikan")
    client.loop_stop()
    client.disconnect()
    