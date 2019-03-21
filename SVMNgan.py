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
    file = open("D:\\TrainSVM-OpenCV\\savePathFireImg.txt","r+") # r = read
    contents = file.readlines()
    labels = []
    hog_descriptors = []
    img = []
    count = 0
    hog = get_hog()
    # hog = cv2.HOGDescriptor(winSize,blockSize,blockStride,cellSize,nbins,derivAperture,winSigma,
                        # histogramNormType,L2HysThreshold,gammaCorrection,nlevels)
    for pathImg in contents:
        # gan nhan labels
        count = count + 1
        if(count < 296 ):
            labels.append(1)
        else:
            labels.append(0)

        pathImg=pathImg.replace("\n", "")
        # print(pathImg)
        img = cv2.imread(pathImg)


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
    print(type(hog_descriptors))
    # print(labels)
    # print(len(labels))

    file.close()