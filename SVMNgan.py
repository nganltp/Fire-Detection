from __future__ import print_function
import sys
PY3 = sys.version_info[0] == 3

if PY3:
    from functools import reduce

import numpy as np
import cv2

# built-in modules
import os
import itertools as it
from contextlib import contextmanager
import random

SZ = 20

def deskew(img):
    m = cv2.moments(img)
    if abs(m['mu02']) < 1e-2:
        return img.copy()
    skew = m['mu11']/m['mu02']
    M = np.float32([[1, skew, -0.5*SZ*skew], [0, 1, 0]])
    img = cv2.warpAffine(img, M, (SZ, SZ), flags=cv2.WARP_INVERSE_MAP | cv2.INTER_LINEAR)
    return img

def split2d(img, cell_size, flatten=True):
    h, w = img.shape[:2]
    sx, sy = cell_size
    cells = [np.hsplit(row, w//sx) for row in np.vsplit(img, h//sy)]
    cells = np.array(cells)
    if flatten:
        cells = cells.reshape(-1, sy, sx)
    return cells

def get_hog() : 
    winSize = (20,20)
    blockSize = (8,8)
    blockStride = (4,4)
    cellSize = (8,8)
    nbins = 9
    derivAperture = 1
    winSigma = -1.
    histogramNormType = 0
    L2HysThreshold = 0.2
    gammaCorrection = 1
    nlevels = 64
    signedGradient = True 

    hog = cv2.HOGDescriptor(winSize,blockSize,blockStride,cellSize,nbins,derivAperture,winSigma,histogramNormType,L2HysThreshold,gammaCorrection,nlevels, signedGradient)

    return hog
    affine_flags = cv2.WARP_INVERSE_MAP|cv2.INTER_LINEAR

def svmInit(C=12.5, gamma=0.50625):
  model = cv2.ml.SVM_create()
  model.setGamma(gamma)
  model.setC(C)
  model.setKernel(cv2.ml.SVM_RBF)
  model.setType(cv2.ml.SVM_C_SVC)
  
  return model

def svmTrain(model, samples, responses):
  model.train(samples, cv2.ml.ROW_SAMPLE, responses)
  model.save("fire_svm_model.yml")
  return model

def svmPredict(model, samples):
  return model.predict(samples)[1].ravel()

def svmEvaluate(model, digits, samples, labels):
    predictions = svmPredict(model, samples)
    # print(labels)
    accuracy = (labels == predictions).mean()
    print('Percentage Accuracy: %.2f %%' % (accuracy*100))

    # confusion = np.zeros((10, 10), np.int32)
    # for i, j in zip(labels, predictions):
    #     confusion[int(i), int(j)] += 1
    # print('confusion matrix:')
    # print(confusion)

    # vis = []
    # for img, flag in zip(digits, predictions == labels):
    #     img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
    #     if not flag:
    #         img[...,:2] = 0
        
    #     vis.append(img)
    # return mosaic(25, vis)

# winSize = (64,64)
# blockSize = (16,16)
# blockStride = (8,8)
# cellSize = (8,8)
# nbins = 9
# derivAperture = 1
# winSigma = 4.
# histogramNormType = 0
# L2HysThreshold = 2.0000000000000001e-01
# gammaCorrection = 0
# nlevels = 64

if __name__ == '__main__':
    file = open("D:\\work\\GIT\\Fire-Detection\\savePathFireImg.txt","r+") # r = read
    contents = file.readlines()
    labels = []
    data = []
    hog_descriptors = []
    count = 0
    hog = get_hog()
    # hog = cv2.HOGDescriptor(winSize,blockSize,blockStride,cellSize,nbins,derivAperture,winSigma,
                        # histogramNormType,L2HysThreshold,gammaCorrection,nlevels)
    for pathImg in contents:
        # gan nhan labels
        count = count + 1
        if(count < 201 ):
            labels.append(1)
        else:
            labels.append(0)

        pathImg=pathImg.replace("\n", "")
        # print(pathImg)
        img = cv2.imread(pathImg)

        data.append(img)
        # compute(img[, winStride[, padding[, locations]]]) -> descriptors
        winStride = (8,8)
        padding = (8,8)
        locations = ((10,20),)
        hist = hog.compute(img,winStride,padding,locations)
        #hist=hist.T #xoay chieu
        hog_descriptors.append(hist)
        # hog_descriptors = np.squeeze(hog_descriptors)
        # # np.savetxt("feature.csv", hist, delimiter=",")

        # print(hist)

        # exit()
        
        
    # hist.save("hog.xml")

    hog_descriptors = np.squeeze(hog_descriptors)

    # print('Shuffle data ... ')
    # # Shuffle data
    # shuffle_order = list(range(len(labels)))
    # random.shuffle(shuffle_order)
    # data = data[shuffle_order]
    # labels = labels[shuffle_orther]

    # rand=np.random.RandomState(10)
    # shuffle = rand.permutation(len(labels))
    # data, labels = data[shuffle], labels[shuffle]

    print('Spliting data into training (80%) and test set (20%)... ')
    train_n=int(0.8*len(hog_descriptors))
    data_train, data_test = np.split(data, [train_n])
    hog_descriptors_train, hog_descriptors_test = np.split(hog_descriptors, [train_n])
    labels_train, labels_test = np.split(labels, [train_n])


    print('Training SVM model ...')
    model = svmInit()
    svmTrain(model, hog_descriptors_train, labels_train)

    print('Evaluating model ... ')
    svmEvaluate(model, data_test, hog_descriptors_test, labels_test)

    # print(type(hog_descriptors))
    print(labels)
    # print(len(labels))

    file.close()