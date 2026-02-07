/**
 * Ví dụ 02: Định vị GPS và gửi lên MQTT
 * Thư viện: EG800K_MQTT
 * * Mục đích: 
 * - Lấy tọa độ GPS thời gian thực từ vệ tinh.
 * - Gửi tọa độ định kỳ lên Server qua 4G LTE.
 */

#include <EG800K_MQTT.h>

// 1. Khởi tạo đối tượng gsm
EG800K_MQTT gsm(Serial1);

// --- CẤU HÌNH THÔNG TIN MQTT ---
const char* mqtt_broker = "XXX.XXX.XXX";
const int   mqtt_port   = 1234;
const char* client_id   = "ESP32C3_GPS_Tracker";
const char* pub_topic   = "KhoaVatKy_DaiHocDaLat/gps"; 

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("HỆ THỐNG ĐỊNH VỊ GPS EG800K KHỞI ĐỘNG...");

  // Cấu hình Server MQTT
  gsm.setServer(mqtt_broker, mqtt_port, client_id);

  // Khởi động module (Mặc định chân 20, 21 cho ESP32-C3)
  gsm.begin();
}

void loop() {
  // 2. Tự động kết nối/kết nối lại MQTT
  if (!gsm.connected()) {
    Serial.println("Đang kết nối mạng di động...");
    gsm.connect();
    
    if (gsm.connected()) {
      Serial.println("Đã kết nối Broker - Bắt đầu chế độ Tracker");
      gsm.publish(pub_topic, "Tracker Started");
    }
  }

  // 3. Duy trì xử lý phản hồi từ module
  gsm.loop();

  /**
   * 4. ĐỌC VÀ GỬI TỌA ĐỘ GPS (Cứ mỗi 30 giây)
   * Lưu ý: Lần đầu tiên bật GPS có thể mất 1-2 phút để "Fix" được vị trí 
   * nếu bạn đang ở trong nhà. Hãy đưa anten ra gần cửa sổ hoặc ngoài trời.
   */
  static unsigned long lastGPSUpdate = 0;
  if (millis() - lastGPSUpdate > 30000) {
    lastGPSUpdate = millis();

    if (gsm.connected()) {
      Serial.println("[GPS] Đang lấy tọa độ...");
      
      // Gọi hàm lấy GPS từ thư viện (trả về chuỗi "lat,lon")
      String location = gsm.getGPS();

      if (location != "") {
        Serial.print("Vị trí hiện tại: ");
        Serial.println(location);

        // Gửi tọa độ lên MQTT
        gsm.publish(pub_topic, location);
        Serial.println("[Hệ thống] Đã cập nhật vị trí lên Server.");
      } else {
        Serial.println("[GPS] Đang đợi tín hiệu vệ tinh (Searching...)...");
      }
    }
  }
}