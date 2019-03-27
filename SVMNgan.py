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
import matplotlib.pyplot as plt

# SZ = 100

# init svm
C=12.5 #defaul 
gamma=0.50625

# get hog descriptors
winSize = (20,20) #size w,h 20x20
blockSize = (10,10) #set 2xcellsize 10x10
blockStride = (5,5) #1/2 block size 4x4
cellSize = (10,10) #10x10 the scale of the features important to do the classification
nbins = 9
derivAperture = 1
winSigma = -1.
histogramNormType = 0
L2HysThreshold = 0.2
gammaCorrection = 0
nlevels = 64
signedGradient = True

# compute HoG
winStride = (8,8)
padding = (8,8)
locations = ((10,20),)

def deskew(img):
    img= cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
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
    hog = cv2.HOGDescriptor(winSize,blockSize,blockStride,cellSize,nbins,derivAperture,winSigma,histogramNormType,L2HysThreshold,gammaCorrection,nlevels, signedGradient)

    return hog
    affine_flags = cv2.WARP_INVERSE_MAP|cv2.INTER_LINEAR

def svmInit():
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

def plot_svc_decision_function(clf, ax=None, plot_support=True):
    """Plot the decision function for a 2D SVC"""
    if ax is None:
        ax = plt.gca()
    xlim = ax.get_xlim()
    ylim = ax.get_ylim()

    # create grid to evaluate model
    x = np.linspace(xlim[0], xlim[1], 30)
    y = np.linspace(ylim[0], ylim[1], 30)
    Y, X = np.meshgrid(y, x)
    xy = np.vstack([X.ravel(), Y.ravel()]).T
    P = clf.decision_function(xy).reshape(X.shape)

    # plot decision boundary and margins
    ax.contour(X, Y, P, colors='k',
               levels=[-1, 0, 1], alpha=0.5,
               linestyles=['--', '-', '--'])

    # plot support vectors
    if plot_support:
        ax.scatter(clf.support_vectors_[:, 0],
                   clf.support_vectors_[:, 1],
                   s=300, linewidth=1, facecolors='none');
    ax.set_xlim(xlim)
    ax.set_ylim(ylim)

if __name__ == '__main__':
    file = open("D:\\work\\GIT\\Fire-Detection\\savePathFireImg.txt","r+") # r = read
    contents = file.readlines()
    labels = []
    data = []
    hog_descriptors = []
    count = 0
    numimg = 422
    hog = get_hog()
    # hog = cv2.HOGDescriptor(winSize,blockSize,blockStride,cellSize,nbins,derivAperture,winSigma,
                        # histogramNormType,L2HysThreshold,gammaCorrection,nlevels)
    for pathImg in contents:
        # gan nhan labels
        count = count + 1
        if(count < numimg ):
            labels.append(1)
        else:
            labels.append(0)

        pathImg=pathImg.replace("\n", "")
        # print(pathImg)
        img = cv2.imread(pathImg)
        #img = deskew(img)
        data.append(img)
        # compute(img[, winStride[, padding[, locations]]]) -> descriptors
        
        hist = hog.compute(img,winStride,padding,locations)
        #hist=hist.T #xoay chieu
        hog_descriptors.append(hist)
        # hog_descriptors = np.squeeze(hog_descriptors)
        # # np.savetxt("feature.csv", hist, delimiter=",")

        # print(hist)

        # exit()
        
        
    # hist.save("hog.xml")

    hog_descriptors = np.squeeze(hog_descriptors)

    print('Shuffle data ... ')
    # Shuffle data
    rng_state = np.random.get_state()
    np.random.shuffle(data)
    np.random.set_state(rng_state)
    np.random.shuffle(labels)

    print('Spliting data into training (80%) and test set (20%)... ')
    train_n=int(0.9*len(hog_descriptors))
    data_train, data_test = np.split(data, [train_n])
    hog_descriptors_train, hog_descriptors_test = np.split(hog_descriptors, [train_n])
    labels_train, labels_test = np.split(labels, [train_n])

    print('Number of train img: '+ str(len(labels_train)/2))
    print('Number of test img: '+ str(len(labels_test)/2))
    
    print('Training SVM model ...')
    model = svmInit()
    svmTrain(model, hog_descriptors_train, labels_train)

    print('Evaluating model ... ')
    svmEvaluate(model, data_test, hog_descriptors_test, labels_test)

    # plt.scatter(X[:, 0], X[:, 1], c=y, s=50, cmap='brg');
    # plot_svc_decision_function(clf)
    # plt.show()

    # print(type(hog_descriptors))
    # print(labels)
    # print(len(labels))

    file.close()