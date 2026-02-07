# EG800K_MQTT - Khoa Vat Ly, Dai hoc Da Lat

Thu vien Arduino chuyen dung cho module **Quectel EG800K (4G LTE Cat 1)** tich hop **GPS**, duoc toi uu hoa cho vi dieu khien **ESP32-C3**.

## ? Tinh nang noi bat
- **Ket noi 4G LTE:** Giao tiep MQTT nhanh va on dinh qua ha tang mang di dong.
- **Dinh vi GPS:** Trich xuat toa do truc tiep (Latitude, Longitude) voi do chinh xac cao.
- **Tu dong khoi phuc:** Co che tu dong ket noi lai (Auto-reconnect) khi mat song hoac Broker bi ngat.
- **De su dung:** Ho tro co che Callback de xu ly du lieu den va cac vi du mau truc quan.

## ?? So do dau noi (Hardware Connection)
Cau hinh chan mac dinh cho dong **ESP32-C3**:

| ESP32-C3 Pin | EG800K Pin | Chuc nang |
| :--- | :--- | :--- |
| **GPIO 21** | **RXD** | Truyen du lieu (TX) |
| **GPIO 20** | **TXD** | Nhan du lieu (RX) |
| **GND** | **GND** | Mass chung |
| **3.7V - 4.2V** | **VBAT** | Cap nguon cho module SIM |

> **Luu y:** Module EG800K can dong dien dinh kha cao khi bat song 4G, hay dam bao nguon dien cap cho module du manh (nen dung Pin Li-ion hoac nguon DC 2A).

## ?? Huong dan cai dat
1. Tai thu muc nay ve may tinh (hoac tai file .zip tu muc Releases).
2. Mo Arduino IDE.
3. Chon **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Chon file ban vua tai ve.

## ?? Cach su dung co ban

```cpp
#include <EG800K_MQTT.h>

EG800K_MQTT gsm(Serial1);

void setup() {
  // Thay doi thong tin Broker cua ban tai day
  gsm.setServer("your.broker.com", 1883, "MyDeviceID");
  gsm.begin();
}

void loop() {
  if (!gsm.connected()) gsm.connect();
  gsm.loop();
  
  // Lay toa do GPS
  String pos = gsm.getGPS();
  if (pos != "") {
    gsm.publish("my/topic", pos);
  }
}