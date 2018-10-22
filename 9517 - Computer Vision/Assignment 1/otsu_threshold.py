import numpy as np
import cv2, sys, argparse

def OTSU_threshold(img_gray):
	max_g = 0
	suitable_th = 0
	img_bin = img_gray
	for threshold in range(0, 255):
		big_th = img_gray > threshold
		small_th = img_gray <= threshold
		fore_pix = np.sum(big_th)
		back_pix = img_gray.size - fore_pix
		if 0 == fore_pix:
			break
		if 0 == back_pix:
			continue
		w0 = float(fore_pix) / img_gray.size
		u0 = float(np.sum(img_gray * big_th)) / fore_pix
		w1 = float(back_pix) / img_gray.size
		u1 = float(np.sum(img_gray * small_th)) / back_pix
		g = w0 * w1 * (u0 - u1) * (u0 - u1)
		if g > max_g:
			max_g = g
			suitable_th = threshold
	img_bin[img_bin > suitable_th] = 255
	img_bin[img_bin <= suitable_th] = 0
	return img_bin,suitable_th

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input",  nargs=1, help="Import a greyscale image path")
	parser.add_argument("-o", "--output",  nargs=1, help="Outpott a binary image path")
	parser.add_argument("-t", "--threshold", help="The suitable thredhold", action='store_true')
	args = parser.parse_args()
	img = cv2.imread(args.input[0],0)
	result_img, threshold = OTSU_threshold(img)
	cv2.imwrite(args.output[0],result_img)
	if args.threshold:
		print("The best threshold is : " + str(threshold))