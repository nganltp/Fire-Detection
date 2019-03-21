import cv2
import numpy as np
import glob
import sys
import os
sys.path.append('C:\Program Files\Python36\Lib\site-packages\libsvm-3.22\python')
from svmutil import *
from datetime import datetime
import matplotlib.pyplot as plt
from sklearn import datasets	
from sklearn import svm
from matplotlib.colors import ListedColormap
import math

C = 12.5
gamma = 0.50625

train_dir = 'D:\\workspace\\TrafficSignRecognitionAndDetection\\Contest\\datasets\\Images'
templates_dir = 'D:\\workspace\\TrafficSignRecognitionAndDetection\\Contest\\templates'

svm_model_file = 'svm_model_' + str(C) + '_' + str(gamma) + '.xml'
confusion_matrix = 'confusion_matrix_' + str(C) + '_' + str(gamma) + '.png'
visualize = 'visualize_' + str(C) + '_' + str(gamma) + '.png'

train_data_file = 'train_data'
video_input = 'video_input.avi'
video_output = 'video_output.avi'
output = 'output.txt'

normal_width = 640
normal_height = 480

blue_templates_id = [2, 3, 8]
red_templates_id = [1, 4, 5, 6, 7]
blue_red_templates_id = [9]
white_black_templates_id = [10]

# threshold
lower_red1 = np.array([0, 40, 40])
upper_red1 = np.array([15, 255, 255])
lower_red2 = np.array([145, 60, 60])
upper_red2 = np.array([180, 255, 255])

lower_blue = np.array([95, 40, 40])
upper_blue = np.array([110, 255, 255])

lower_black = np.array([0, 0, 0])
upper_black = np.array([180, 255, 30])

lower_white = np.array([0, 0, 190])
upper_white = np.array([180, 50, 255])
'''
lower_gray = np.array([0, 0, 0])
upper_gray = np.array([180, 50, 255])
lower_brown = np.array([10, 10, 0])
upper_brown = np.array([20, 255, 200])
'''
# HOG feature
width = height = 48
hog = cv2.HOGDescriptor(_winSize = (width, height), 
						_blockSize = (width // 2, height // 2),
						_blockStride = (width // 4, height // 4),
						_cellSize = (width // 2, height // 2),
						_nbins = 9,
						_derivAperture = 1,
						_winSigma = -1,
						_histogramNormType = 0,
						_L2HysThreshold = 0.2,
						_gammaCorrection = 1,
						_nlevels = 64, 
						_signedGradient = True)

def deskew(img):
    m = cv2.moments(img)
    if abs(m['mu02']) < 1e-2:
        # no deskewing needed. 
        return img.copy()
    # Calculate skew based on central momemts. 
    skew = m['mu11']/m['mu02']
    # Calculate affine transform to correct skewness. 
    M = np.float32([[1, skew, -0.5*width*skew], [0, 1, 0]])
    # Apply affine transform
    img = cv2.warpAffine(img, M, (width, height), flags=cv2.WARP_INVERSE_MAP | cv2.INTER_LINEAR)
    return img

def load_templates():
	images = []
	for img_dir in glob.glob(templates_dir + '\*.jpg'):
		img = cv2.imread(img_dir)
		img = cv2.resize(img, (width, height))
		images.append(img)

	titles = []
	with open(templates_dir + '\\titles.txt') as f:
		for line in f:
			titles.append(line.replace('\n', ''))

	print(titles)
	return images, titles

def extract_video_datasets(_dir):
	print(_dir)
	for vid_dir in glob.glob(_dir + '\*.mp4'):
		if not os.path.exists(vid_dir.split('.')[0]):
			os.makedirs(vid_dir.split('.')[0])
		
		print('\t' + vid_dir.split('.')[0])
		print('\tProcessing...')
		video = cv2.VideoCapture(vid_dir)
		frame_id = 0

		while video.isOpened():
			ret, frame = video.read()
			if not ret:
				break
			cv2.imwrite(vid_dir.split('.')[0] + '\\' + str(frame_id).zfill(5) + '_light_03.jpg', frame) # save frame as JPEG file
			frame_id += 1
		
def calculate_hog(_images, _data_file):
	print('\tProcessing...')
	n_datasets = len(_images)
	
	# open _data_file and append hog descriptors to it
	with open(_data_file, 'a') as f:
		# calculate hog_descriptors for each image in _images
		for img in _images:
			descriptor = hog.compute(img)
			f.write(' '.join(str(x[0]) for x in descriptor) + '\n')
	
def load_data_file(_data_file):
	print('\tProcessing...')
	hog_descriptors = []
	labels = []
	
	with open(_data_file, 'r') as f:
		# load labels
		for x in f.readline().split(' '):
			labels.append(int(x))
		# load hog descriptors
		for line in f:
			descriptor = []
			for x in line.split(' '):
				descriptor.append([x])
			hog_descriptors.append(np.array(descriptor, dtype=np.float32))

	return hog_descriptors, labels

'''	
Execute another function and calculate the execution time of that
Parameters:
	- notification: the string that printed when the function executed 
	- func: the function name
	- *arg: all parameters of the function
'''
def execute(_notification, _func, *_args):	
	print(_notification)
	
	t_start = datetime.now()
	result = _func(*_args)
	delta = datetime.now() - t_start

	print('Time: %fs' % (delta.seconds + delta.microseconds/1E6))
	print('-----')
	return result
'''
class_dir = 'D:\\workspace\\TrafficSignRecognitionAndDetection\\Contest\\datasets\\Images\\00035'
print(class_dir + '\\GT-' + class_dir.split('\\')[-1] + '.csv')
with open(class_dir + '\\GT-' + class_dir.split('\\')[-1] + '.csv', 'r') as f:
	f.readline()
	for line in f:
		[file_name, width, height, x1, y1, x2, y2, class_id] = line.split(';')
		img = cv2.imread(class_dir + '\\' + file_name)
		img = img[int(x1):int(x2), int(y1):int(y2)]
		cv2.imwrite(class_dir + '\\' + 'crop_' + class_dir.split('\\')[-1] + '_' + file_name, img)
'''