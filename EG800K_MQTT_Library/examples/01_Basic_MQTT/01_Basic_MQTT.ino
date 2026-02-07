/**
 * Ví dụ 01: Kết nối MQTT Cơ bản
 * Thư viện: EG800K_MQTT
 * * Mục đích: 
 * - Khởi tạo module EG800K với ESP32-C3
 * - Kết nối vào mạng 4G và Broker MQTT
 * - Đăng ký (Subscribe) và gửi (Publish) dữ liệu đơn giản
 */

#include <EG800K_MQTT.h>

// 1. Khởi tạo đối tượng gsm sử dụng Serial1 (hoặc Serial2 tùy chọn)
EG800K_MQTT gsm(Serial1);

// --- CẤU HÌNH THÔNG TIN TẠI ĐÂY ---
const char* mqtt_broker = "XXX.XXX.XXX";
const int   mqtt_port   = 1234;
const char* client_id   = "ESP32C3_Client"; // ID này nên là duy nhất
const char* sub_topic   = "KhoaVatKy_DaiHocDaLat/commands";  // Topic để nhận lệnh
const char* pub_topic   = "KhoaVatKy_DaiHocDaLat/sensor";    // Topic để gửi trạng thái

/**
 * Hàm Callback: Tự động gọi khi có tin nhắn từ Server gửi xuống
 */
void mqtt_callback(String topic, String payload) {
  Serial.println("-----------------------");
  Serial.print("Tin nhắn mới từ topic: ");
  Serial.println(topic);
  Serial.print("Nội dung: ");
  Serial.println(payload);
  Serial.println("-----------------------");

  // Ví dụ: Nếu nhận "Xin chào" thì in ra thông báo
  if (payload == "Xin chào") {
    Serial.println(">> ĐÃ NHẬN LỜI CHÀO");
  }
}

void setup() {
  // Mở cổng Serial của máy tính để theo dõi Log
  Serial.begin(115200);
  delay(1000);
  Serial.println("BẮT ĐẦU KIỂM TRA EG800K...");

  // 2. Thiết lập chân kết nối (RX=20, TX=21 cho ESP32-C3)
//   gsm.SetSerial(20, 21);// KHÔNG CẦN THIẾT VÌ MẶC ĐỊNH CỦA ESP32-C3 ĐÃ LÀ CHÂN ĐÓ

  // 3. Cấu hình Server MQTT
  gsm.setServer(mqtt_broker, mqtt_port, client_id);

  // 4. Thiết lập hàm xử lý tin nhắn đến
  gsm.setCallback(mqtt_callback);

  // 5. Khởi động module
  gsm.begin();
}

void loop() {
  // 6. Kiểm tra và tự động kết nối lại nếu bị mất mạng
  if (!gsm.connected()) {
    Serial.println("Đang thử kết nối MQTT...");
    gsm.connect();
    
    if (gsm.connected()) {
      // Sau khi kết nối thành công, đăng ký nhận tin từ topic
      gsm.subscribe(sub_topic);
      
      // Thông báo lên server là tôi đã online
      gsm.publish(pub_topic, "ESP32-C3 Online!");
    }
  }

  // 7. Duy trì kết nối và xử lý dữ liệu (BẮT BUỘC có trong loop)
  gsm.loop();

  // Ví dụ: Cứ mỗi 60 giây gửi một bản tin "Heartbeat" để báo cáo thiết bị còn sống
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    lastUpdate = millis();
    if (gsm.connected()) {
        // ĐỌC SENSOR
      gsm.publish(pub_topic, "Keep-alive message");// GỬI THÔNG SỐ SENSOR TƯƠNG TỰ
      Serial.println("[Hệ thống] Đã gửi bản tin duy trì kết nối.");
    }
  }
}