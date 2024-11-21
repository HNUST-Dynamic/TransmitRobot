import cv2
import numpy as np
from pyzbar.pyzbar import decode
import serial
import time
import sys

# 定义帧头和帧尾
FRAME_HEADER = b'\x02'
FRAME_TAIL = b'\x03'

# 二维码实际宽度 (厘米)
w_real = 8.0

# 相机的焦距 (像素单位)
focal_length = 340

# 初始化串口
ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
time.sleep(0.02)

def measure_qr_code_distance(image):
    decoded_objects = decode(image)
    for obj in decoded_objects:
        points = obj.polygon
        if len(points) == 4:
            pts = [(point.x, point.y) for point in points]
            w_image = np.linalg.norm(np.array(pts[0]) - np.array(pts[1]))

            distance = (w_real * focal_length) / w_image
            print(f"测量宽度: {w_image:.2f} 像素, 计算距离: {distance:.2f} cm")
            return distance
    print("未检测到二维码")
    return None

def send_data_with_frame(data):
    frame = FRAME_HEADER + data.encode('utf-8') + FRAME_TAIL
    ser.write(frame)
    print(f"发送数据：{frame}")

# 初始化摄像头
camera = cv2.VideoCapture(2)

if not camera.isOpened():
    raise IOError("无法打开摄像头")

try:
    print("等待串口指令...")
    while True:
        ret, frame = camera.read()
        if not ret:
            print("无法捕获帧")
            continue

        # 实时显示摄像头画面
        cv2.imshow('rangging', frame)

        # 检查串口是否有数据传入
        if ser.in_waiting > 0:
            request = ser.readline().decode('utf-8').strip()
            print(f"Received request: {request}")

            # 当收到 "2" 时进行一次测距
            if request == "2":
                print("收到 '2' 指令，开始测距...")
                #ser.write("1".encode('utf-8'))
                
                # 测量二维码距离
                distance = measure_qr_code_distance(frame)
                if distance:
                    send_data_with_frame(f"{distance:.2f}")
                    print(f"二维码距离: {distance:.2f} cm")
                else:
                    print("未检测到二维码")

            # 当收到 "0" 指令时退出程序
            elif request == "0":
                print("收到 '0' 指令，退出程序...")
                break

        # 按 'q' 键退出程序
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    camera.release()
    cv2.destroyAllWindows()
    ser.close()
