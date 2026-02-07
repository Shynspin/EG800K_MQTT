#include "EG800K_MQTT.h"

// Constructor: Lưu tham số Serial vào con trỏ nội bộ
EG800K_MQTT::EG800K_MQTT(HardwareSerial& serial) {
  _serial = &serial;
}

// Thiết lập chân RX, TX tùy chỉnh
void EG800K_MQTT::SetSerial(int rx, int tx) {
  _rx = rx;
  _tx = tx;
}

// Thiết lập thông tin Server MQTT từ người dùng (không còn fix cứng)
void EG800K_MQTT::setServer(String broker, int port, String clientID) {
  _broker = broker;
  _port = port;
  _clientID = clientID;
}

// Bắt đầu khởi tạo HardwareSerial với module EG800K
void EG800K_MQTT::begin() {
  _serial->begin(115200, SERIAL_8N1, _rx, _tx);
  Serial.println("[EG800K_MQTT] Serial Started. RX:" + String(_rx) + " TX:" + String(_tx));
}

// Gán hàm Callback để xử lý dữ liệu nhận về
void EG800K_MQTT::setCallback(MQTTCallback cb) {
  _callback = cb;
}

// Hàm gửi lệnh AT và đợi phản hồi mong muốn (Ok, Ready, v.v.)
bool EG800K_MQTT::sendAT(String cmd, const char* expected_resp, int timeout) {
  _serial->println(cmd);
  unsigned long start = millis();
  String res = "";
  while (millis() - start < timeout) {
    while (_serial->available()) {
      res += (char)_serial->read();
    }
    if (res.indexOf(expected_resp) != -1) return true;
  }
  return false;
}

// Kiểm tra trạng thái SIM và sóng 4G LTE
bool EG800K_MQTT::isNetworkReady() {
  if (!sendAT("AT+CPIN?", "READY", 1000)) {
    Serial.println("[EG800K_MQTT] ERROR: SIM not ready!");
    return false;
  }
  if (!sendAT("AT+CGATT?", "+CGATT: 1", 1000)) {
    Serial.println("[EG800K_MQTT] Waiting for 4G LTE...");
    return false;
  }
  return true;
}

// Quy trình kết nối MQTT theo chuẩn tập lệnh AT của Quectel
void EG800K_MQTT::connect() {
  if (!isNetworkReady()) {
    _isConnected = false;
    return;
  }

  Serial.println("[EG800K_MQTT] Connecting to Broker: " + _broker);
  
  // Đóng các kết nối cũ trước khi mở mới để tránh xung đột
  sendAT("AT+QMTCLOSE=0", "OK", 1000);
  sendAT("AT+QIDEACT=1", "OK", 1000);
  delay(1000);

  // 1. Mở kết nối mạng tới Broker
  String openCmd = "AT+QMTOPEN=0,\"" + _broker + "\"," + String(_port);
  if (sendAT(openCmd, "+QMTOPEN: 0,0", 10000)) {
    
    // 2. Đăng nhập với ClientID (thêm mã ngẫu nhiên để tránh trùng lặp)
    String fullClientID = _clientID + "_" + String(random(0xfff), HEX);
    String connCmd = "AT+QMTCONN=0,\"" + fullClientID + "\"";
    
    if (sendAT(connCmd, "+QMTCONN: 0,0,0", 5000)) {
      _isConnected = true;
      Serial.println("[EG800K_MQTT] MQTT Connected Successfully!");
    } else {
      Serial.println("[EG800K_MQTT] ERROR: QMTCONN Failed.");
    }
  } else {
    Serial.println("[EG800K_MQTT] ERROR: QMTOPEN Failed.");
  }
}

// Gửi tin nhắn MQTT (Publish)
void EG800K_MQTT::publish(String topic, String payload) {
  if (!_isConnected) return;
  // Bao bọc topic và payload trong dấu ngoặc kép để tránh lỗi ký tự đặc biệt
  String pubCmd = "AT+QMTPUB=0,0,0,0,\"" + topic + "\",\"" + payload + "\"";
  _serial->println(pubCmd);
}

// Đăng ký nhận tin nhắn (Subscribe)
void EG800K_MQTT::subscribe(String topic) {
  if (!_isConnected) return;
  if (sendAT("AT+QMTSUB=0,1,\"" + topic + "\",1", "+QMTSUB: 0,1,0", 3000)) {
    _lastTopic = topic; // Lưu lại để tự động sub lại nếu bị mất kết nối
    Serial.println("[EG800K_MQTT] Subscribed to: " + topic);
  }
}

// Lấy tọa độ GPS (Định dạng Decimal Degrees)
String EG800K_MQTT::getGPS() {
    sendAT("AT+QGPS=1", "OK", 500); // Bật GPS nếu chưa bật
    _serial->println("AT+QGPSLOC=2");
    
    unsigned long start = millis();
    while (millis() - start < 2000) {
        if (_serial->available()) {
            String res = _serial->readString();
            int pos = res.indexOf("+QGPSLOC:");
            if (pos != -1) {
                int firstComma = res.indexOf(",");
                int secondComma = res.indexOf(",", firstComma + 1);
                if (firstComma != -1 && secondComma != -1) {
                    String lat = res.substring(firstComma + 1, secondComma);
                    int endLon = res.indexOf(",", secondComma + 1);
                    if (endLon == -1) endLon = res.indexOf("\r", secondComma + 1);
                    String lon = res.substring(secondComma + 1, endLon);
                    return lat + "," + lon; 
                }
            }
        }
    }
    return ""; // Trả về chuỗi rỗng nếu không lấy được tọa độ
}

// Hàm duy trì kết nối và đọc dữ liệu phản hồi từ module
void EG800K_MQTT::loop() {
  if (_serial->available()) {
    String res = _serial->readString();

    // 1. Kiểm tra nếu có tin nhắn MQTT gửi đến (+QMTRECV)
    if (res.indexOf("+QMTRECV:") != -1 && _callback != nullptr) {
      int firstQuote = res.indexOf("\"", res.indexOf("+QMTRECV:"));
      int secondQuote = res.indexOf("\"", firstQuote + 1);
      String topic = res.substring(firstQuote + 1, secondQuote);

      int thirdQuote = res.indexOf("\"", secondQuote + 1);
      int fourthQuote = res.indexOf("\"", thirdQuote + 1);
      String payload = res.substring(thirdQuote + 1, fourthQuote);
      
      _callback(topic, payload); // Gọi hàm xử lý ở file chính (.ino)
    }

    // 2. Kiểm tra trạng thái mất kết nối từ hệ thống
    if (res.indexOf("+QMTSTAT: 0,1") != -1 || res.indexOf("CLOSED") != -1) {
      Serial.println("[EG800K_MQTT] Connection lost!");
      _isConnected = false;
    }
  }

  // 3. Cơ chế tự động kết nối lại mỗi 10 giây nếu bị ngắt
  if (!_isConnected) {
    unsigned long now = millis();
    if (now - _lastReconnectAttempt > 10000) {
      _lastReconnectAttempt = now;
      connect();
      // Nếu kết nối lại thành công, tự động đăng ký lại Topic cũ
      if (_isConnected && _lastTopic != "") {
        sendAT("AT+QMTSUB=0,1,\"" + _lastTopic + "\",1", "+QMTSUB: 0,1,0", 3000);
      }
    }
  }
}