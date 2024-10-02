import cv2
import torch

# 使用 torch.hub 加载 YOLOv5 模型
model = torch.hub.load('yolov5', 'custom', path='/home/orangepi/Desktop/yolov5/best.pt', source='local')

# 初始化摄像头
camera = cv2.VideoCapture(0)  # 0 表示默认摄像头

if not camera.isOpened():
    raise IOError("无法打开摄像头")

while True:
    # 捕获视频帧
    ret, frame = camera.read()
    if not ret:
        break

    # 使用 YOLOv5 进行推理
    # 进行推理时使用 torch.amp.autocast
    with torch.amp.autocast('cuda'):
        results = model(frame)

    # 提取检测结果并绘制
    annotated_frame = results.render()[0]

    # 显示结果
    cv2.imshow('YOLOv5 检测', annotated_frame)

    # 按 'q' 键退出
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放摄像头和关闭窗口
camera.release()
cv2.destroyAllWindows()
