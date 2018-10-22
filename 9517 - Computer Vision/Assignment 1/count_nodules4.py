import cv2, argparse
import numpy as np

max_int = 10000
np.set_printoptions(threshold=np.inf)

def setImage_threshold(img_gray):  
	img_bin = np.zeros(( img_gray.shape[0], img_gray.shape[1]))
	for i,col in enumerate(img_gray):
		for j,value in enumerate(col):
			if value > 150:
				img_bin[i,j] = 0
			else:
				img_bin[i,j] = max_int
	return img_bin

def min_neighbor_4(img, x, y):
	size_w, size_h = img.shape
	temp_list = []
	x1 = 0 if x==0 else img[x - 1, y]
	x2 = 0 if x==size_w-1 else img[x + 1, y]
	y1 = 0 if y==0 else img[x, y - 1]
	y2 = 0 if y==size_h-1 else img[x, y + 1] 

	if not x1==0:
		temp_list.append(x1)
	if not x2==0:
		temp_list.append(x2)
	if not y1==0:
		temp_list.append(y1)
	if not y2==0:
		temp_list.append(y2)

	if len(temp_list) == 0:
		return 0
	else:
		return min(temp_list)



def count_dot(img, limit_s):
	label = 1
	for i,col in enumerate(img): 	#Frist pass
		for j,value in enumerate(col):
			if value == max_int:
				img[i,j] = label
				min_n = min_neighbor_4(img, i, j)
				if not min_n == 0 and min_n < label:
					img[i,j] = min_n
				else:
					label += 1

	for i,col in enumerate(img):    #Second pass
		for j,value in enumerate(col):
			if value > 0:
				min_n = min_neighbor_4(img, i, j)
				img[img == value] = min_n

	unique, counts = np.unique(img, return_counts=True)
	uniqueDict = dict(zip(unique, counts))
	
	copyDict = uniqueDict.copy()
	for key, value in copyDict.items():
		if value < limit_s:
			uniqueDict.pop(key)

	keyDict = dict()
	i = 0
	for key, value in uniqueDict.items():
		keyDict[key] = i
		i += 1

	result_img = np.zeros((img.shape[0], img.shape[1]), np.uint8)
	for i,col in enumerate(img):
		for j,value in enumerate(col):
			if not value == 0:
				if value in keyDict:
					result_img[i,j] = keyDict[value]
				else:
					result_img[i,j] = 0

	label_hue = np.uint8((result_img/np.max(result_img)) * 179)
	blank_ch = 255 * np.ones_like(label_hue)
	labeled_img = cv2.merge([label_hue, blank_ch, blank_ch])
	labeled_img = cv2.cvtColor(labeled_img, cv2.COLOR_HSV2BGR)
	labeled_img[label_hue==0] = 255
	
	return len(uniqueDict.keys()) - 1, labeled_img


if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("-i", "--input",  nargs=1, help="Import a greyscale image path")
	parser.add_argument("-s", "--size",  nargs=1, help="Import a greyscale image path")
	parser.add_argument("-o", "--optional_output",  nargs=1, help="Outpott a binary image path", required=False)
	args = parser.parse_args()
	img_ori = cv2.imread(args.input[0],0)
	limit_size = args.size[0]
	input_img = setImage_threshold(img_ori)
	count, final_img = count_dot(input_img, int(limit_size))
	print("The number of nodules is : " +str(count))
	if args.optional_output:
		output = args.optional_output[0]
		cv2.imwrite(output,final_img)

	
	
	
