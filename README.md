# EG800K_MQTT
Giải pháp IoT (Internet of Things) xây dựng với module EG800K và ESP32-C3 là một hệ thống Giám sát và Định vị từ xa qua mạng di động (4G LTE).
🛰️ Giải pháp Hệ thống Giám sát & Định vị GPS qua 4G LTE
1. Tổng quan (Overview)
Hệ thống này tận dụng sức mạnh của dòng vi điều khiển ESP32-C3 (nhỏ gọn, hỗ trợ RISC-V) kết hợp với module truyền thông Quectel EG800K (4G LTE Cat 1) để tạo ra một thiết bị IoT có khả năng di động cao. Giải pháp này giải quyết bài toán truyền tin và định vị ở những nơi không có sóng Wi-Fi.

2. Các thành phần chính (Key Components)
Thiết bị cuối (Edge Device): ESP32-C3 đóng vai trò bộ não, xử lý dữ liệu từ cảm biến và điều khiển module viễn thông.

Kết nối (Connectivity): Mạng 4G LTE thông qua module EG800K, đảm bảo tốc độ cao và độ trễ thấp.

Giao thức truyền tin: MQTT (Message Queuing Telemetry Transport) - giao thức tiêu chuẩn cho IoT giúp tiết kiệm băng thông và năng lượng.

Định vị: Công nghệ GNSS/GPS tích hợp trong EG800K để xác định tọa độ thời gian thực.

3. Sơ đồ luồng dữ liệu (Data Flow)
Thu thập: ESP32-C3 lấy dữ liệu GPS (Kinh độ, Vĩ độ) từ module EG800K thông qua tập lệnh AT.

Đóng gói: Dữ liệu được đóng gói dưới định dạng JSON hoặc chuỗi ký tự tùy chỉnh.

Truyền tải: Dữ liệu được gửi lên MQTT Broker (ví dụ: xxx.xxx.xxx.xxx) qua kết nối 4G.

Xử lý & Hiển thị: Các ứng dụng phía người dùng (Web dashboard, Mobile App hoặc Node-RED) đăng ký (Subscribe) vào cùng một Topic để nhận và hiển thị vị trí lên bản đồ.

4. Tính năng nổi bật của Thư viện (Library Features)
Giải pháp này đi kèm với thư viện EG800K_MQTT giúp tối giản hóa việc lập trình:

Tự động kết nối lại (Auto-reconnect): Tự động khôi phục kết nối MQTT khi đi vào vùng sóng yếu.

Quản lý bộ nhớ tốt: Thiết kế tối ưu cho các dòng chip có tài nguyên giới hạn như ESP32-C3.

Xử lý Callback: Hỗ trợ nhận lệnh điều khiển từ xa thông qua cơ chế Callback linh hoạt.

Trích xuất GPS trực tiếp: Hàm lấy tọa độ được xử lý sẵn, trả về chuỗi lat,lon cực kỳ đơn giản.

5. Khả năng ứng dụng (Applications)
Theo dõi hành trình (Fleet Management): Giám sát xe tải, xe máy, container.

Nông nghiệp thông minh: Thu thập dữ liệu từ các trạm quan trắc ở cánh đồng xa nơi không có Wi-Fi.

Hệ thống giám sát: Thu thập dữ liệu từ các trạm quan trắc nơi không có Wi-Fi.

Thiết bị an ninh di động: Hệ thống báo động, chống trộm có khả năng gửi cảnh báo tức thời qua mạng di động.
