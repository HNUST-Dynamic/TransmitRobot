#!/bin/bash
source /home/orangepi/miniconda3/etc/profile.d/conda.sh

conda activate yolov5

xvfb-run -a /home/orangepi/miniconda3/envs/yolov5/bin/python /home/orangepi/Desktop/yolov5/qrcode.py

sleep 5

xvfb-run -a /home/orangepi/miniconda3/envs/yolov5/bin/python /home/orangepi/Desktop/yolov5/rangging.py

sleep 5

xvfb-run -a /home/orangepi/miniconda3/envs/yolov5/bin/python /home/orangepi/Desktop/yolov5/detect.py --weights /home/orangepi/Desktop/yolov5/best.pt --source 0

sleep 5
