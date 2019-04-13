# import the necessary packages
from imutils.video import VideoStream
import argparse
import datetime
import imutils
import time
import cv2
# import pickle
from sklearn.externals import joblib

# --------IMPORT FOR LOAD MODEL-------->
from skimage import feature
from skimage.feature import local_binary_pattern

import numpy as np

from sklearn.svm import LinearSVC
import matplotlib.pyplot as plt
from sklearn import svm
from sklearn.model_selection import GridSearchCV
from imutils import paths
import argparse
# ------------------------------------->
 
# construct the argument parser and parse the arguments
# ap = argparse.ArgumentParser()
# ap.add_argument("-v", "--video", help="path to the video file")
# ap.add_argument("-a", "--min-area", type=int, default=500, help="minimum area size")
# args = vars(ap.parse_args())
 
# # if the video argument is None, then we are reading from webcam
# if args.get("video", None) is None:
# 	vs = VideoStream(src=0).start()
# 	time.sleep(2.0)
 
# # otherwise, we are reading from a video file
# else:
# 	vs = cv2.VideoCapture(args["video"])
 
# initialize the first frame in the video stream
firstFrame = None
min_area = 100 #defaul 100
bgThresh = 70 #defaul 25
# Load model from file
joblib_file = "joblib_model.pkl" 
model = joblib.load(joblib_file)

class LocalBinaryPatterns:
	def __init__(self, numPoints, radius):
		# store the number of points and radius
		self.numPoints = numPoints
		self.radius = radius
 
	def describe(self, image, eps=1e-7):
		# compute the Local Binary Pattern representation
		# of the image, and then use the LBP representation
		# to build the histogram of patterns
		lbp = feature.local_binary_pattern(image, self.numPoints,
			self.radius, method="uniform")
		(hist, _) = np.histogram(lbp.ravel(),
			bins=np.arange(0, self.numPoints + 3),
			range=(0, self.numPoints + 2))
 
		# normalize the histogram
		hist = hist.astype("float")
		hist /= (hist.sum() + eps)
 
		# return the histogram of Local Binary Patterns
		return hist

cap = cv2.VideoCapture('D:\\work\\EarlyFireDetection-master\\EarlyFireDetection\\EarlyFireDetection\\test_6.mp4')

frame_width = int(cap.get(3))
frame_height = int(cap.get(4))

# Check if camera opened successfully
if (cap.isOpened()== False): 
	print("Error opening video stream or file")
# loop over the frames of the video
fourcc = cv2.VideoWriter_fourcc(*'DIVX') 
out = cv2.VideoWriter('output.avi', -1 , 20.0, (500,int(frame_height*500/frame_width)))

radius = 3
numPoints = 8*radius 
desc = LocalBinaryPatterns(numPoints, radius)

while (cap.isOpened()):
	# grab the current frame and initialize the occupied/unoccupied
	# text
	start_time = time.time()
	ret, frame = cap.read()
	# frame = frame if args.get("video", None) is None else frame[1]
	text = "None"
 
	# if the frame could not be grabbed, then we have reached the end
	# of the video
	if frame is None:
		break
	# if ret == True:
 
    # Display the resulting frame
    # num = num + 1
		# cv2.imshow('Frame',frame)
	
 
  # Break the loop
 
	# resize the frame, convert it to grayscale, and blur it
	frame = imutils.resize(frame, width=500)
	gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
	# gray = cv2.GaussianBlur(gray, (21, 21), 0)
 
	# if the first frame is None, initialize it
	if firstFrame is None:
		firstFrame = gray
		continue
	
# compute the absolute difference between the current frame and
	# first frame
	frameDelta = cv2.absdiff(firstFrame, gray)
	thresh = cv2.threshold(frameDelta, bgThresh, 255, cv2.THRESH_BINARY)[1]
 
	# dilate the thresholded image to fill in holes, then find contours
	# on thresholded image

# --------------------------CHECK BY COLOR SPACE--------------------------->
	# b, g, r = firstFrame[:, :, 0], firstFrame[:, :, 1], firstFrame[:, :, 2]
	threshCheckColor = thresh.copy()
	h, w, c = frame.shape
	b, g, r = cv2.split(frame)
	for x in range(h):
		for y in range(w):
			continue
			# print(frame[x][y])
			# if(r[x][y] < g[x][y] or g[x][y] < b[x][y] or r[x][y] < 230):
			# 	threshCheckColor[x][y] = 0
# ------------------------------------------------------------------------->
	
	# thresh = cv2.dilate(thresh, None, iterations=2)
	cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
	cnts = imutils.grab_contours(cnts)
 
	# loop over the contours
	for c in cnts:
		# if the contour is too small, ignore it
		if cv2.contourArea(c) < min_area:
			continue
		(x, y, w, h) = cv2.boundingRect(c)
# ------------------------------PREDICT------------------------------->
		crop_img = frame[y:y+h, x:x+w]	
		dst = cv2.resize(crop_img,(64,64))
		# image = cv2.resize(image,(300,300))
		dst = cv2. cvtColor(dst, cv2.COLOR_BGR2HSV)
		# gray = cv2.cvtColor(dst, cv2.COLOR_BGR2GRAY)
		# hist = desc.describe(gray)
		b, g, r = cv2.split(dst)

		histB = desc.describe(b)
		histG = desc.describe(g)
		histR = desc.describe(r)
		# gray = cv2.cvtColor(img , cv2.COLOR_BGR2GRAY)
		# hist = desc.describe(gray)
		hist = np.concatenate((histB,histG,histR))
		# print(hist)
		prediction_ = model.predict(hist.reshape(1, -1))
		# print(prediction_[0])
		if(prediction_[0] == 0):
			cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
		else:
			continue
			# cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)
# ----------------------------------------------------------------------->
		# compute the bounding box for the contour, draw it on the frame,
		# and update the text
		cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 0, 255), 2)
		text = "Motion"
		out.write(frame)
# draw the text and timestamp on the frame
	cv2.putText(frame, "Room Status: {}".format(text), (10, 20),
		cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 2)
	# cv2.putText(frame, datetime.datetime.now().strftime("%A %d %B %Y %I:%M:%S%p"),
	# 	(10, frame.shape[0] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.35, (0, 0, 255), 1)
	print("FPS: ", 1.0 / (time.time() - start_time))
	# show the frame and record if the user presses a key
	cv2.imshow("Security Feed", frame)
	cv2.imshow("Thresh", thresh)
	cv2.imshow("Frame Delta", frameDelta)
	cv2.imshow("color_space",threshCheckColor)

	key = cv2.waitKey(1) & 0xFF
	if key == ord('q'):
		break
 
# cleanup the camera and close any open windows
# vs.stop() if args.get("video", None) is None else vs.release()
cap.release()
out.release()
cv2.destroyAllWindows()