# T-AIMS IoT Heartbeat - Ilvina

Mekanisme heartbeat ESP32 & Raspberry Pi untuk T-AIMS.

## Struktur
- `esp32/` - firmware ESP32 (PlatformIO)
- `simulator/` - simulator ESP32 (Python)
- `raspi/` - agent Raspberry Pi (Python)
- `docs/` - dokumentasi & test result

## Cara Jalankan Simulator
1. Install Mosquitto (broker MQTT lokal)
2. Install library: `pip install paho-mqtt`
3. Jalankan: `python simulator/simulate_esp32.py`

## Cara Upload ESP32
1. Buka folder `esp32/` di VS Code (PlatformIO)
2. Isi `include/config.h` dengan WiFi & MQTT
3. Klik Upload

## Dokumentasi
Lihat `docs/heartbeat.md` dan `docs/test-result.md`.