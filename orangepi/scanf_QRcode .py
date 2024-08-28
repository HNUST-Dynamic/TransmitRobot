#scanf QR code
import cv2
import numpy as np
from pyzbar.pyzbar import decode

def decode_qr_code(image):
    decoded_objects = decode(image)
    for obj in decoded_objects:
        # Draw a rectangle around the detected QR code
        points = obj.polygon
        if len(points) == 4:
            pts = [(point.x, point.y) for point in points]
            cv2.polylines(image, [np.array(pts, dtype=np.int32)], True, (0, 255, 0), 2)

        # Draw the QR code data on the image
        qr_data = obj.data.decode('utf-8')
        cv2.putText(image, qr_data, (obj.rect.left, obj.rect.top - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

        # Print the QR code data and return True to indicate detection
        print(f"QR Code Data: {qr_data}")
        return True

    return False

# Initialize the camera
camera = cv2.VideoCapture(0)  # Use 0 for the default camera

if not camera.isOpened():
    raise IOError("Cannot open the webcam")

try:
    while True:
        # Capture frame-by-frame
        ret, frame = camera.read()
        if not ret:
            raise IOError("Cannot capture frame")

        # Decode QR codes
        if decode_qr_code(frame):
            print("QR Code detected. Exiting...")
            break

        # Display the resulting frame
        cv2.imshow('QR Code Detection', frame)

        # Break the loop on 'q' key press
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
finally:
    # Release the capture and close windows
    camera.release()
    cv2.destroyAllWindows()

