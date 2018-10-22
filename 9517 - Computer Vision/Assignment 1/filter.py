import cv2, sys, argparse
import numpy as np

def denoising(img):
	kernel = np.ones((5, 5), np.uint8)
	close = cv2.morphologyEx(img, cv2.MORPH_CLOSE, kernel)
	close[close > 250] = 255
	close[close <= 250] = 0
#	print(img)
#	print(close)
	return close

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input",  nargs=1, help="Import a greyscale image path")
	parser.add_argument("-o", "--output",  nargs=1, help="Outpott a binary image path")	
	args = parser.parse_args()
	img = cv2.imread(args.input[0],0)
	result_img = denoising(img)
#	avg_grey = np.sum(result_img) / result_img.size
#	final_img = setImage_threshold(result_img, 150)
	cv2.imwrite(args.output[0],result_img)