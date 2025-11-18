# Đề tài: Hệ thống điều khiển quạt thông minh sử dụng vi điều khiển STM32

## 1. Giới thiệu

> Trong thời đại công nghiệp 4.0, các hệ thống điều khiển tự động và IoT (Internet of Things) ngày càng được ứng dụng rộng rãi nhằm nâng cao tính tiện nghi, tiết kiệm năng lượng và khả năng giám sát từ xa. Một trong những ứng dụng phổ biến là hệ thống điều khiển quạt thông minh, có khả năng tự động điều chỉnh tốc độ theo nhiệt độ môi trường và cho phép người dùng điều khiển từ xa thông qua mạng Internet.

Đề tài này tập trung vào việc thiết kế và xây dựng hệ thống điều khiển quạt thông minh sử dụng vi điều khiển **STM32**.

---

## 2. Các chế độ hoạt động

Hệ thống có các chế độ hoạt động linh hoạt như sau:

* **Chế độ tắt (Default mode):**
    Sau khi khởi động (power-on), quạt mặc định ở trạng thái tắt.

* **Chế độ mở (Manual ON):**
    Quạt chỉ hoạt động sau khi nhận lệnh mở từ người dùng.

* **Chế độ tự động:**
    Vi điều khiển đọc giá trị nhiệt độ từ cảm biến (qua bộ ADC) trong khoảng 5–50°C và tự động điều chỉnh tốc độ quạt tương ứng:
    * **Dưới 15°C:** Mức gió 1 (tắt quạt)
    * **15–30°C:** Mức gió 2 (tốc độ thấp)
    * **Trên 30°C:** Mức gió 3 (tốc độ cao)

* **Chế độ điều khiển bằng tay:**
    Người dùng gửi lệnh chọn mức gió 1, 2 hoặc 3. Khi hệ thống đang ở chế độ tự động mà nhận lệnh thủ công, hệ thống tự chuyển về chế độ điều khiển tay.

* **Chế độ điều khiển qua web:**
    Người dùng có thể giám sát trạng thái quạt và gửi lệnh điều khiển thông qua giao diện web trực quan.

---

## 3. Công nghệ tích hợp

Hệ thống này tích hợp các công nghệ nhúng và IoT, sử dụng:

* **Giao tiếp UART:** Để truyền nhận lệnh điều khiển.
* **ADC:** Để đọc dữ liệu cảm biến nhiệt độ.
* **Bluetooth:** Để chuyền dữ liệu từ IoTNode đến Gateway.
* **Giao diện Web:** Để giám sát và điều khiển từ xa.
