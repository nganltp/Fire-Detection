from skimage import feature
from skimage.feature import local_binary_pattern

import numpy as np

from sklearn.svm import LinearSVC
from imutils import paths
import argparse
import cv2
import os

pathImgTest = "D:\\work\\GIT\\Fire-Detection\\TestImage\\shibainu.jpg"

# print('import ok')
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

if __name__ == '__main__':
	
	# open the file where save path of all image
	file = open("D:\\work\\GIT\\Fire-Detection\\savePathFireImg.txt","r+")
	contents = file.readlines()
	
	count = 0
	labels = []
	data = []

	# parameter for compute LBP
	radius = 3
	numPoints = 8*radius 

	numImg = 500

	desc = LocalBinaryPatterns(numPoints, radius)

	for pathImg in contents:
		# assign labels
		# label 0: Fire
		# label 1: Non Fire
		# label 2: Smoke
		count = count + 1
		if(count < numImg + 1):
			labels.append(1)
		elif (numImg < count) and (count < 2*numImg+1) :
			labels.append(2)
		else:
			labels.append(0)

		pathImg=pathImg.replace("\n", "")
		# print(pathImg)		
		img = cv2.imread(pathImg)
		gray = cv2.cvtColor(img , cv2.COLOR_BGR2GRAY)

		hist = desc.describe(gray)
		data.append(hist)
	print('Shuffle data ... ')
	# Shuffle data
	rng_state = np.random.get_state()
	np.random.shuffle(data)
	np.random.set_state(rng_state)
	np.random.shuffle(labels)

	print(len(labels))
	print('Spliting data into training (80%) and test set (20%)... ')
	train_n=int(0.8*len(labels))
	data_train, data_test = np.split(data, [train_n])
	labels_train, labels_test = np.split(labels, [train_n])

	# train a Linear SVM on the data
	model = LinearSVC(C=100.0, random_state=42)
	model.fit(data_train, labels_train)

	print('Evaluating model ... ')
    # svmEvaluate(model, data_test , labels_test)

	# loop over the testing images
	# for imagePath in paths.list_images(args["testing"]):
	# load the image, convert it to grayscale, describe it,
	# and classify it

	dem = 0
	for i in range(0,len(data_test)):
		# print(labels_test[i])
		prediction = model.predict(data_test[i].reshape(1, -1))
		if (prediction[0]==labels_test[i]):

			dem = dem + 1
			# cv2.putText(image, str(prediction_[0]), (10, 30), cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,0),2,cv2.LINE_AA)
			# cv2.imshow("Image", image)
	accuracy = float(dem/len(data_test))
	print('so anh nhan dien: ' + str(len(data_test)))
	print('so anh nhan dien dung: ' + str(dem))
	print('Percentage Accuracy: %.2f %%' % (accuracy*100))	

	image = cv2.imread('D:\\work\\GIT\\Fire-Detection\\TestImage\\111.jpg')
	dst = cv2.resize(image,(64,64))
	image = cv2.resize(image,(300,300))
	gray = cv2.cvtColor(dst, cv2.COLOR_BGR2GRAY)
	hist = desc.describe(gray)
	prediction_ = model.predict(hist.reshape(1, -1))

	# print(prediction_)
	# display the image and the prediction
	cv2.putText(image, str(prediction_[0]), (10, 30), cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,0),2,cv2.LINE_AA)
	cv2.imshow("Image", image)


	cv2.imshow('gray', gray)
	cv2.waitKey(0)
	cv2.destroyAllWindows()
	file.close()