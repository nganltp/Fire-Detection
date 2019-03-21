import numpy as np
import cv2

import os
# path = "D:\\work\\Fire images_data\\smoke\\original_img"
path ="D:\\work\\Fire images_data\\TRAIN\\resize-non300\\"
#os.mkdir("D:\\work\\Fire images_data\\smoke\\blur")
num=0
nameImg = os.listdir(path)
for i in nameImg:
	pathImg = os.path.join(path,i)
	# print(pathImg)
	img = cv2.imread(pathImg,1)
	
	# image processing
	dst = cv2.resize(img,(100,100))
	# dst = cv2.blur(dst,(5,5))
	# dst = img[0:300,150:400] #y:y+h, x:x+w
	# height, width = img.shape[:2]
	#save img
	cv2.imwrite("D:\\work\\Fire images_data\\TRAIN\\resize-non100\\" + str(num) + ".jpg",dst)
	num = num + 1
	# display
	cv2.imshow('image',img)
	cv2.imshow('processed',dst)


cv2.waitKey(0)
cv2.destroyAllWindows()
