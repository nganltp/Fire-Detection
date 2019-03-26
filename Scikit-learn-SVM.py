import numpy as np
import cv2
from sklearn import svm, metrics
from sklearn.svm import SVC  
from sklearn.model_selection import train_test_split  
import os
# print('import ok')
if __name__ == '__main__':
	file = open("D:\\work\\GIT\\Fire-Detection\\savePathFireImg.txt","r+") # r = read
	contents = file.readlines()
	labels = []
	data = []
	count = 0
	# hog = cv2.HOGDescriptor(winSize,blockSize,blockStride,cellSize,nbins,derivAperture,winSigma,
	                    # histogramNormType,L2HysThreshold,gammaCorrection,nlevels)
	for pathImg in contents:
	    # gan nhan labels
	    count = count + 1
	    if(count < 274 ):
	        labels.append(1)
	    else:
	        labels.append(0)

	    pathImg=pathImg.replace("\n", "")
	    img = cv2.imread(pathImg)
	    data.append(img)

	print('Shuffle data ... ')
	# Shuffle data
	rng_state = np.random.get_state()
	np.random.shuffle(data)
	np.random.set_state(rng_state)
	np.random.shuffle(labels)

	print(len(data))
	print(len(labels))
	print(data[0])
	print('Spliting data into training (80%) and test set (20%)... ')
	# train_n=int(0.9*len(labels))
	# data_train, data_test = np.split(data, [train_n])
	# labels_train, labels_test = np.split(labels, [train_n])


	# data_train = np.array(data_train)/255
	# data_test = np.array(data_test)/255
	X_train, X_test, y_train, y_test = train_test_split(data, labels, test_size = 0.20) 

	# # TRAIN
	clf = svm.SVC(kernel='linear')
	clf.fit(X_train, y_train)

	# # TEST
	# print("PREDICT")
	# predict = clf.predict(test_images)

	# print("RESULT")
	# ac_score = metrics.accuracy_score(test_labels, predict)
	# cl_report = metrics.classification_report(test_labels, predict)
	# print("Score = ", ac_score)
	# print(cl_report)	    