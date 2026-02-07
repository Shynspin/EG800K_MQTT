#ifndef EG800K_MQTT_H
#define EG800K_MQTT_H

#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * Định nghĩa kiểu hàm Callback để xử lý tin nhắn MQTT đến.
 * @param topic: Tên chủ đề mà tin nhắn gửi tới.
 * @param payload: Nội dung tin nhắn nhận được.
 */
typedef void (*MQTTCallback)(String topic, String payload);

class EG800K_MQTT {
private:
    // --- Các biến nội bộ (Private Members) ---
    HardwareSerial* _serial;       // Con trỏ quản lý giao tiếp Serial với module
    MQTTCallback _callback = nullptr; // Lưu trữ hàm xử lý khi có tin nhắn đến
    
    bool _isConnected = false;     // Trạng thái kết nối MQTT hiện tại
    unsigned long _lastReconnectAttempt = 0; // Thời điểm thử kết nối lại gần nhất
    
    String _broker;                // Địa chỉ Server MQTT (Broker)
    int _port;                     // Cổng kết nối (thường là 1883)
    String _clientID;              // ID duy nhất của thiết bị trên Broker
    String _lastTopic = "";        // Lưu topic cuối cùng để tự động đăng ký lại khi mất mạng
    
    int _rx = 20;                  // Chân nhận dữ liệu (Default: 20 cho ESP32-C3)
    int _tx = 21;                  // Chân truyền dữ liệu (Default: 21 cho ESP32-C3)

    // Hàm nội bộ để gửi lệnh AT và kiểm tra phản hồi từ module
    bool sendAT(String cmd, const char* expected_resp, int timeout = 2000);

public:
    /**
     * Khởi tạo đối tượng thư viện.
     * @param serial: Truyền vào HardwareSerial (ví dụ: Serial1 hoặc Serial2).
     */
    EG800K_MQTT(HardwareSerial& serial);

    /**
     * Thiết lập chân kết nối Serial giữa ESP32 và EG800K.
     */
    void SetSerial(int rx, int tx);

    /**
     * Cấu hình thông tin Server MQTT.
     * Hàm này sẽ gán thông tin vào biến thay vì để mặc định trong code.
     */
    void setServer(String broker, int port, String clientID);

    /**
     * Gán hàm xử lý sự kiện khi có tin nhắn từ Broker gửi về.
     */
    void setCallback(MQTTCallback cb);

    /**
     * Bắt đầu khởi tạo giao tiếp Serial và cấu hình module.
     */
    void begin();

    /**
     * Kiểm tra trạng thái mạng 4G và SIM.
     * @return true nếu mạng đã sẵn sàng.
     */
    bool isNetworkReady();

    /**
     * Thực hiện quy trình kết nối tới Broker MQTT thông qua lệnh AT.
     */
    void connect();

    /**
     * Gửi dữ liệu lên một Topic nhất định.
     */
    void publish(String topic, String payload);

    /**
     * Đăng ký nhận dữ liệu từ một Topic nhất định.
     */
    void subscribe(String topic);

    /**
     * Lấy tọa độ GPS hiện tại từ module.
     * @return Chuỗi định dạng "vĩ độ, kinh độ".
     */
    String getGPS();

    /**
     * Hàm duy trì kết nối và kiểm tra dữ liệu đến. 
     * Cần được gọi liên tục trong hàm loop() của Arduino.
     */
    void loop();

    /**
     * Kiểm tra nhanh trạng thái kết nối.
     */
    bool connected() {
        return _isConnected;
    }
};

#endif