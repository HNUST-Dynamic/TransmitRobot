import cv2
import numpy as np
from pyzbar.pyzbar import decode
import serial  # 导入串口通信库
import time

# 定义帧头和帧尾
FRAME_HEADER = b'\x02'  # 十六进制表示帧头 (STX)
FRAME_TAIL = b'\x03'    # 十六进制表示帧尾 (ETX)

# 二维码实际宽度 (厘米)
w_real = 8.0  # 根据实际二维码宽度调整

# 相机的焦距 (像素单位)
focal_length = 340  # 通过相机标定或实验确定

# 初始化串口
ser = serial.Serial('/dev/ttyS1', 9600, timeout=1)  # 根据需要调整串口号和波特率
time.sleep(2)  # 等待串口稳定

def measure_qr_code_distance(image):
    decoded_objects = decode(image)
    for obj in decoded_objects:
        # 取出二维码的四个角点
        points = obj.polygon
        if len(points) == 4:
            pts = [(point.x, point.y) for point in points]

            # 计算二维码在图像中的宽度（像素）
            w_image = np.linalg.norm(np.array(pts[0]) - np.array(pts[1]))

            # 计算距离 (单位: 厘米)
            distance = (w_real * focal_length) / w_image
            print(f"Distance: {distance:.2f} cm")
            return distance

    return None

def send_data_with_frame(data):
    # 通过串口发送带帧头和帧尾的数据
    frame = FRAME_HEADER + data.encode('utf-8') + FRAME_TAIL
    ser.write(frame)
    print(f"发送数据：{frame}")

# 初始化摄像头
camera = cv2.VideoCapture(0)  # 使用默认摄像头

if not camera.isOpened():
    raise IOError("无法打开摄像头")

try:
    print("等待串口指令...")
    while True:
        # 检查串口是否有数据传入
        if ser.in_waiting > 0:
            request = ser.readline().decode('utf-8').strip()
            print(f"Received request: {request}")

            # 当32端发送 "2" 时开始测距
            if request == "2":
                print("收到 '2' 指令，开始测距...")
                
                while True:
                    ret, frame = camera.read()
                    if not ret:
                        raise IOError("无法捕获帧")

                    # 测量二维码距离
                    distance = measure_qr_code_distance(frame)
                    if distance:
                        # 将距离数据通过串口发送，并加上帧头帧尾
                        send_data_with_frame(f"{distance:.2f}")
                        print(f"二维码距离: {distance:.2f} cm")

                    # 检查是否收到停止测距的指令 "0"
                    if ser.in_waiting > 0:
                        stop_request = ser.readline().decode('utf-8').strip()
                        if stop_request == "0":
                            print("收到 '0' 指令，停止测距...")
                            break

        # 按 'q' 键退出程序
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    camera.release()
    cv2.destroyAllWindows()
    ser.close()  # 关闭串口
