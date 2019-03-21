from header import *
from image_processing import *

def load_train_datasets():
	train_images = []
	train_labels = []
	
	# load all images and labels in the train directory
	for class_dir in glob.glob(train_dir + '\*'):
		if '.mp4' not in class_dir:
			print('\t' + class_dir)
			load_class_datasets(class_dir, train_images, train_labels)

	# open train data file and write labels to it
	with open(train_data_file, 'w') as f:
		f.write(' '.join(str(x) for x in train_labels) + '\n')	
	return train_images

def svm_training():
	# load hog descriptors and labels from train d0ata file
	train_hog_descriptors, train_labels = execute('\tLoading data file', load_data_file, train_data_file)
	print('Training')
	# init SVM model
	svm = cv2.ml.SVM_create()
	svm.setC(C)
	svm.setGamma(gamma)	
	svm.setType(cv2.ml.SVM_C_SVC)
	svm.setKernel(cv2.ml.SVM_RBF)
	
	# Train SVM on train data  
	svm.train(np.array(train_hog_descriptors), cv2.ml.ROW_SAMPLE, np.array(train_labels))
	# Save trained model
	svm.save(svm_model_file)

	return svm, train_hog_descriptors, train_labels

def load_class_datasets(_dir, _images, _labels):
	print('\tProcessing...')
	for img_dir in glob.glob(_dir + '\*'):
		img = cv2.imread(img_dir, 0)
		img = cv2.resize(img, (width, height))
		_images.append(img)
		_labels.append(int(_dir.split('\\')[-1]))

def create_train_datasets(_dir):
	print('Processing...')
	print(_dir)
	for img_dir in glob.glob(_dir + '\*'):
		print(img_dir)
		if ('_img' not in img_dir) and ('_mask' not in img_dir):
			img = cv2.imread(img_dir)
			equ_img = ycrcb_equalize(img)
			#equ_img = cv2.medianBlur(img, 21)
			mask = get_mask(equ_img)
			contour = find_contour(equ_img, mask)
			
			if contour:
				# print('ok')
				[x, y, w, h] = contour
				# img = cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)
				img = img[int(y):int(y + h), int(x):int(x + w)]
				
			cv2.imwrite(img_dir.split('.')[0] + '_mask.jpg', mask)
			cv2.imwrite(img_dir.split('.')[0] + '_img.jpg', img)

def train():
	# After the first running, you can skip these steps at the next time
	# by loading train_data file in svm_training() function
	train_images = execute('Loading train datasets', load_train_datasets)
	execute('HOG calculating', calculate_hog, train_images, train_data_file)
	
	svm, train_hog_descriptors, train_labels = execute('Train', svm_training)