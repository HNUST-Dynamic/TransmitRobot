import cv2
import numpy as np
from pyzbar.pyzbar import decode
import serial  # 导入串口通信库
import time

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

        # 打印二维码数据
        print(f"QR Code Data: {qr_data}")

        # 将二维码数据通过串口发送
        ser.write(f"QR Code Data: {qr_data}\n".encode('utf-8'))

        return True 
        
    return False

# 初始化串口
ser = serial.Serial('/dev/ttyS1', 9600, timeout=1)  # 根据需要调整串口号和波特率
time.sleep(2)  # 等待串口稳定

# 初始化摄像头
camera = cv2.VideoCapture(0)  # 使用索引0的默认摄像头

if not camera.isOpened():
    raise IOError("无法打开摄像头")

try:
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
    # 释放摄像头、关闭串口并关闭窗口
    camera.release()
    ser.close()
    cv2.destroyAllWindows()
