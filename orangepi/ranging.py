<<<<<<< HEAD
#ranging
import cv2
import numpy as np
from pyzbar.pyzbar import decode

# 二维码实际宽度 (厘米)
w_real = 8.0  # 你需要测量实际的二维码宽度并替换这个值

# 相机的焦距 (单位：像素)
focal_length = 340  # 这个值是根据相机标定结果得到的，或者通过实验确定

def decode_qr_code(image):
    decoded_objects = decode(image)
    for obj in decoded_objects:
        # 绘制检测到的二维码的矩形边框
        points = obj.polygon
        if len(points) == 4:
            pts = [(point.x, point.y) for point in points]
            cv2.polylines(image, [np.array(pts, dtype=np.int32)], True, (0, 255, 0), 2)

        # 显示二维码的数据
        qr_data = obj.data.decode('utf-8')
        cv2.putText(image, qr_data, (obj.rect.left, obj.rect.top - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

        # 计算图像中二维码的宽度（像素单位）
        w_image = np.linalg.norm(np.array(pts[0]) - np.array(pts[1]))

        # 计算距离 (单位: 厘米)
        distance = (w_real * focal_length) / w_image

        # 显示距离
        distance_text = f"Distance: {distance:.2f} cm"
        cv2.putText(image, distance_text, (obj.rect.left, obj.rect.top + obj.rect.height + 20), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 0, 0), 2)

        # 打印二维码数据和距离，并返回 True 表示检测成功
        print(f"QR Code Data: {qr_data}")
        print(f"Distance: {distance:.2f} cm")
        return True

    return False

# 初始化摄像头
camera = cv2.VideoCapture(0)  # 使用索引0的默认摄像头
=======
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
ser = serial.Serial('/dev/ttyS1', 9600, timeout=1)
time.sleep(2)
def measure_qr_code_distance(image, num_samples=5):
    distances = []
    for _ in range(num_samples):
        decoded_objects = decode(image)
        for obj in decoded_objects:
            points = obj.polygon
            if len(points) == 4:
                pts = [(point.x, point.y) for point in points]
                w_image = np.linalg.norm(np.array(pts[0]) - np.array(pts[1]))

                distance = (w_real * focal_length) / w_image
                distances.append(distance)
                print(f"测量宽度: {w_image:.2f} 像素, 计算距离: {distance:.2f} cm")
                break
        else:
            print("未检测到二维码")
            return None

    if distances:
        avg_distance = sum(distances) / len(distances)
        print(f"平均距离: {avg_distance:.2f} cm")
        return avg_distance
    return None

def send_data_with_frame(data):
    frame = FRAME_HEADER + data.encode('utf-8') + FRAME_TAIL
    ser.write(frame)
    print(f"发送数据：{frame}")

# 初始化摄像头
camera = cv2.VideoCapture('/dev/came1')
>>>>>>> 6fdb71e4f4f8c0801235cfac60f06b8a59b4909a

if not camera.isOpened():
    raise IOError("无法打开摄像头")

try:
<<<<<<< HEAD
    while True:
        # 捕获逐帧图像
        ret, frame = camera.read()
        if not ret:
            raise IOError("无法捕获帧")

        # 解码二维码
        if decode_qr_code(frame):
            print("检测到二维码。退出...")
            break

        # 显示结果帧
        cv2.imshow('QR Code Detection', frame)

        # 按 'q' 键退出
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
finally:
    # 释放摄像头并关闭窗口
    camera.release()
    cv2.destroyAllWindows()
=======
    print("等待串口指令...")
    while True:
        # 检查串口是否有数据传入
        if ser.in_waiting > 0:
            request = ser.readline().decode('utf-8').strip()
            print(f"Received request: {request}")

            # 当收到 "2" 时进行一次测距
            if request == "2":
                print("收到 '2' 指令，开始测距...")
                ser.write("1".encode('utf-8'))
                ret, frame = camera.read()
                cv2.imshow('rangging', frame)
                if not ret:
                    print("无法捕获帧")
                    continue

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
                sys.exit(0)



        # 按 'q' 键退出程序
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    camera.release()
    cv2.destroyAllWindows()
    ser.close()
>>>>>>> 6fdb71e4f4f8c0801235cfac60f06b8a59b4909a
