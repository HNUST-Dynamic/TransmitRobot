#use without npu
import cv2
import numpy as np
from IPython.display import display, clear_output,Image
from ultralytics import YOLO
from time import time
# Load a model
model = YOLO('best.pt')  # pretrained YOLOv8n model
# Initialize the camera
camera = cv2.VideoCapture(1)  # Use 0 for the default camera
# Set the codec to MJPG if it is supported
if camera.isOpened():
    # camera.set(cv2.CAP_PROP_FRAME_WIDTH, 1280.0)
    # camera.set(cv2.CAP_PROP_FRAME_HEIGHT, 720.0)
    camera.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
else:
    raise IOError("Cannot open the webcam")
try:
    while True:
        # Capture frame-by-frame
        ret, frame = camera.read()
        if not ret:
            raise IOError("Cannot capture frame")
        s = time()
        results = model(frame,conf=0.25,iou=0.5,verbose=False)
        print(time()-s)
        for r in results:
            im = r.plot()
        # Display the image
        clear_output(wait=True)
        # Afficher l'image capturée
        display(Image(data=cv2.imencode('.jpg', im)[1]))
finally:
    # When everything done, release the capture
    camera.release()