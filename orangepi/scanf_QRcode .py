import cv2
import numpy as np
from pyzbar.pyzbar import decode
import serial  # 导入串口通信库
import time
import sys  # 导入系统模块，用于退出程序

# 定义帧头和帧尾
FRAME_HEADER = b'\x01'  # 十六进制表示帧头 (STX)
FRAME_TAIL = b'\x03'    # 十六进制表示帧尾 (ETX)

def decode_qr_code(image):
    decoded_objects = decode(image)
    for obj in decoded_objects:
        points = obj.polygon
        if len(points) == 4:
            pts = [(point.x, point.y) for point in points]
            cv2.polylines(image, [np.array(pts, dtype=np.int32)], True, (0, 255, 0), 2)

        qr_data = obj.data.decode('utf-8')
        cv2.putText(image, qr_data, (obj.rect.left, obj.rect.top - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

        print(f"QR Code Data: {qr_data}")

        # 加上帧头帧尾并发送二维码数据
        data_with_frame1 = FRAME_HEADER + f"{qr_data}".encode('utf-8') + FRAME_TAIL
        data_with_frame2 = f't0.txt="{qr_data}"' 
        data_with_frame3=b'\xff\xff\xff'
        # 发送第一个数据

        ser1.write(data_with_frame1)
          
        print("发送第一个数据:", data_with_frame1)
        time.sleep(0.01)  # 添加延时，确保数据完整发送

        # 发送第二个数据
        ser2.write(data_with_frame3)
        time.sleep(0.01)
        ser2.write(data_with_frame2.encode('utf-8'))
        time.sleep(0.01)
        ser2.write(data_with_frame3)
        print("发送第二个数据:", data_with_frame2)
        return True 

    return False

# 初始化串口
ser1 = serial.Serial('/dev/ttyS1', 115200, timeout=1)  # 第一个串口
ser2 = serial.Serial('/dev/ttyS3', 115200, timeout=1)  # 第2个串口

time.sleep(0.01)

# 初始化摄像头
camera = cv2.VideoCapture(2)

if not camera.isOpened():
    raise IOError("无法打开摄像头")
#ser.write(FRAME_HEADER + b'0\n' + FRAME_TAIL)
#print("摄像头已打开，发送 '0' 到串口")
try:
    
    while True:
        # 检查串口是否有数据传入
                while True:
                    ret, frame = camera.read()
                    if not ret:
                        raise IOError("无法捕获帧")

                    # 解码二维码
                    if decode_qr_code(frame):
                        print("检测到二维码。退出程序...")
                        # 直接退出整个程序
                        sys.exit(0)

                    # 显示结果帧
                    cv2.imshow('QR Code Detection', frame)

                    # 按 'q' 键退出二维码扫描
                    if cv2.waitKey(1) & 0xFF == ord('q'):
                        break



finally:
    camera.release()
    ser1.close()
    ser2.close()
    cv2.destroyAllWindows()
