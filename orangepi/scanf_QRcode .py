import cv2
import numpy as np
from pyzbar.pyzbar import decode
import serial  # 导入串口通信库
import time

# 定义帧头和帧尾
FRAME_HEADER = b'\x02'  # 十六进制表示帧头 (STX)
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
        data_with_frame = FRAME_HEADER + f"{qr_data}\n".encode('utf-8') + FRAME_TAIL
        ser.write(data_with_frame)

        return True 

    return False

# 初始化串口
ser = serial.Serial('/dev/ttyS1', 9600, timeout=1)
time.sleep(2)

# 初始化摄像头
camera = cv2.VideoCapture(0)

if not camera.isOpened():
    raise IOError("无法打开摄像头")

try:
    print("等待串口接收到 '1' 指令...")
    
    while True:
        # 检查串口是否有数据传入
        if ser.in_waiting > 0:
            request = ser.readline().decode('utf-8').strip()
            print(f"Received request: {request}")

            # 当收到的指令是 '1' 时开始扫描二维码
            if request == "1":
                print("收到 '1' 指令，开始扫描二维码...")
                
                while True:
                    ret, frame = camera.read()
                    if not ret:
                        raise IOError("无法捕获帧")

                    # 解码二维码
                    if decode_qr_code(frame):
                        print("检测到二维码。退出扫描...")
                        break

                    # 显示结果帧
                    cv2.imshow('QR Code Detection', frame)

                    # 按 'q' 键退出二维码扫描
                    if cv2.waitKey(1) & 0xFF == ord('q'):
                        break

        # 按 'q' 键退出程序
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    camera.release()
    ser.close()
    cv2.destroyAllWindows()
