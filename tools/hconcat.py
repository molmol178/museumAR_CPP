import numpy as np
import cv2

img = cv2.imread('sample.pbm')
img1 = cv2.imread('sample_perspective.pbm')

img3 = cv2.hconcat([img, img1])

cv2.imwrite('test.png' ,img3)
