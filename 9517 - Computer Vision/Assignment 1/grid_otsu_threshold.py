import numpy as np
import cv2, sys, argparse, math

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
	

def setImage_threshold(img_gray, suitable_th):  
	img_bin = img_gray
	img_bin[img_bin > suitable_th] = 0
	img_bin[img_bin <= suitable_th] = 255
	return img_bin

def blockshaped(arr, r_nbrs, c_nbrs, interp=cv2.INTER_LINEAR):
	arr_h, arr_w = arr.shape
	size_w = int( math.floor(arr_w // c_nbrs) * c_nbrs )
	size_h = int( math.floor(arr_h // r_nbrs) * r_nbrs )
	
	if size_w != arr_w or size_h != arr_h:
		arr = cv2.resize(arr, (size_w, size_h), interpolation=interp)
	
	nrows = int(size_w // r_nbrs)
	ncols = int(size_h // c_nbrs)
	
	return (arr.reshape(r_nbrs, ncols, -1, nrows) 
			   .swapaxes(1,2)
			   .reshape(-1, ncols, nrows))

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input",  nargs=2, help="Import a greyscale image path")
	parser.add_argument("-o", "--output",  nargs=1, help="Outpott a binary image path")	
	args = parser.parse_args()
	img = cv2.imread(args.input[0],0)
	size = args.input[1]
	img_h, img_w = img.shape
	number = img_h // int(size)
	subImgs = blockshaped(img, number, number)
	num, sub_h, sub_w = subImgs.shape
	finalImg = np.zeros((sub_h*number , sub_w*number))
	re_rows = 0
	re_cols = -1
	avg_grey = np.sum(img) / img.size
	pre_threshold = avg_grey
	for i in range(0, num):
		result_img, threshold = OTSU_threshold(subImgs[i])
		# Check the threshold
		if threshold >= avg_grey - 2:
			result_img = setImage_threshold(subImgs[i], pre_threshold)
		else:
			pre_threshold = result_img
		# Convert 3D subImages to 2D image
		re_cols += 1
		if re_cols == number:
			re_cols = 0
			re_rows = re_rows + 1
		finalImg[re_rows*sub_h : re_rows*sub_h + sub_h,re_cols*sub_w : re_cols*sub_w + sub_w] = result_img
	cv2.imwrite(args.output[0],finalImg)