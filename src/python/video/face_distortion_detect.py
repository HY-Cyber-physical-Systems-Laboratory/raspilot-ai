import cv2
import numpy as np
import math
import os
import glob

modelFile = "res10_300x300_ssd_iter_140000.caffemodel"
configFile = "deploy.prototxt"
net = cv2.dnn.readNetFromCaffe(configFile, modelFile)

orb = cv2.ORB_create(nfeatures=2000)
bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=False)

K = np.array([[800, 0, 320],
              [0, 800, 240],
              [0,   0,   1]], dtype=np.float64)

cap = cv2.VideoCapture(0)
save_dir = "face_refs"
os.makedirs(save_dir, exist_ok=True)

ref_data = {}  # {key: [(img, kp, des), ...]}

def rotationMatrixToEulerAngles(R):
    sy = math.sqrt(R[0, 0] ** 2 + R[1, 0] ** 2)
    singular = sy < 1e-6
    if not singular:
        x = math.atan2(R[2, 1], R[2, 2])
        y = math.atan2(-R[2, 0], sy)
        z = math.atan2(R[1, 0], R[0, 0])
    else:
        x = math.atan2(-R[1, 2], R[1, 1])
        y = math.atan2(-R[2, 0], sy)
        z = 0
    return np.degrees(np.array([x, y, z]))

def load_references():
    global ref_data
    ref_data = {}
    keys = ['w', 'a', 's', 'd', 'r']
    for k in keys:
        ref_data[k] = []
        files = sorted(glob.glob(f"{save_dir}/ref_{k}_*.png"))
        for f in files:
            img = cv2.imread(f)
            if img is None:
                continue
            kp, des = orb.detectAndCompute(img, None)
            if des is not None:
                ref_data[k].append((img, kp, des))

load_references()

while True:
    ret, frame = cap.read()
    if not ret:
        break

    h, w = frame.shape[:2]
    blob = cv2.dnn.blobFromImage(frame, 1.0, (300, 300), (104.0, 177.0, 123.0))
    net.setInput(blob)
    detections = net.forward()

    best_score = 0
    best_angles = None
    best_box = None
    best_H = None
    best_face_roi = None
    best_ref_key = None
    best_ref_idx = None

    for i in range(detections.shape[2]):
        confidence = detections[0, 0, i, 2]
        if confidence > 0.7:
            box = detections[0, 0, i, 3:7] * [w, h, w, h]
            x1, y1, x2, y2 = box.astype("int")
            face_roi = frame[y1:y2, x1:x2]
            kp2, des2 = orb.detectAndCompute(face_roi, None)

            if des2 is None or len(kp2) < 10:
                continue

            for key in ref_data.keys():
                for idx, (ref_img, ref_kp, ref_des) in enumerate(ref_data[key]):
                    matches = bf.knnMatch(ref_des, des2, k=2)
                    good = [m for m, n in matches if m.distance < 0.8 * n.distance]
                    if len(good) < 8:
                        continue

                    src_pts = np.float32([ref_kp[m.queryIdx].pt for m in good]).reshape(-1, 1, 2)
                    dst_pts = np.float32([kp2[m.trainIdx].pt for m in good]).reshape(-1, 1, 2)
                    H, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 10.0)
                    if H is not None:
                        num_solutions, Rs, Ts, Ns = cv2.decomposeHomographyMat(H, K)
                        R = Rs[0]
                        angles = rotationMatrixToEulerAngles(R)
                        score = len(good)
                        if score > best_score:
                            best_score = score
                            best_angles = angles
                            best_box = (x1, y1, x2, y2)
                            best_H = H
                            best_face_roi = face_roi.copy()
                            best_ref_key = key
                            best_ref_idx = idx

    if best_box is not None:
        x1, y1, x2, y2 = best_box
        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)
        cv2.putText(frame, f"Ref: {best_ref_key.upper()}_{best_ref_idx+1}", (x1, y1 - 55),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)
        cv2.putText(frame, f"Pitch: {best_angles[0]:.1f}", (x1, y1 - 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
        cv2.putText(frame, f"Yaw:   {best_angles[1]:.1f}", (x1, y1 - 25),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
        cv2.putText(frame, f"Roll:  {best_angles[2]:.1f}", (x1, y1 - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)

        h_ref, w_ref = best_face_roi.shape[:2]
        corners = np.float32([[0, 0], [w_ref, 0], [w_ref, h_ref], [0, h_ref]]).reshape(-1, 1, 2)
        transformed = cv2.perspectiveTransform(corners, best_H)
        cv2.polylines(best_face_roi, [np.int32(transformed)], True, (255, 255, 0), 2)
        cv2.imshow("Best Face ROI", best_face_roi)

    cv2.imshow("Face Rotation Analysis", frame)

    key = cv2.waitKey(1) & 0xFF
    if key == 27:
        break
    elif key in [ord('w'), ord('a'), ord('s'), ord('d'), ord('r')]:
        # 저장할 파일명 인덱스 계산
        existing_files = glob.glob(f"{save_dir}/ref_{chr(key)}_*.png")
        next_idx = len(existing_files) + 1
        for i in range(detections.shape[2]):
            confidence = detections[0, 0, i, 2]
            if confidence > 0.7:
                box = detections[0, 0, i, 3:7] * [w, h, w, h]
                x1, y1, x2, y2 = box.astype("int")
                face_roi = frame[y1:y2, x1:x2]
                if face_roi.size == 0:
                    continue
                filename = f"{save_dir}/ref_{chr(key)}_{next_idx}.png"
                cv2.imwrite(filename, face_roi)
                print(f"Saved face reference as {filename}")
                load_references()
                break

cap.release()
cv2.destroyAllWindows()
