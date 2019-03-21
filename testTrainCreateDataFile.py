import os

file = open("D:\\TrainSVM-OpenCV\\savePathFireImg.txt","a+") #mo ghi vao cuoi file: a
path = "D:\\work\\Fireimages_data\\TRAIN\\1\\"
nameImg = os.listdir(path)
for i in nameImg:
	pathImg = os.path.join(path,i)
	file.write(pathImg)
	# file.write(" "+"1")
	file.write("\n")



file.close()