from __future__ import print_function  #
import cv2, sys
import argparse, math, re, random, sys, time , os
import numpy as np

#========================================= STEP 1 =========================================

def up_to_step_1(imgs):
	"""Complete pipeline up to step 3: Detecting features and descriptors"""
	surf = cv2.xfeatures2d.SURF_create(2000)
	new_imgs = list()
	for img in imgs:
		kp, des = surf.detectAndCompute(img, None)
		img_new = cv2.drawKeypoints(img,kp, None,flags=cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)
		new_imgs.append(img_new)
	return new_imgs


def save_step_1(imgs, output_path='./output/step1'):
	"""Save the intermediate result from Step 1"""
	if not os.path.exists(output_path):
		os.makedirs(output_path)
	for i,img in enumerate(imgs):
		filename = imgs_filename[i]
		cv2.imwrite(os.path.join(output_path, filename),img)

#========================================= STEP 2 =========================================

def create_lable(des):
	labels = np.ones(des.shape[0],).astype(np.float32)
	for i,k in enumerate(des):
		labels[i] = i
	return labels

def matcher(des1, des2, k):
	label1 = create_lable(des1)
	knn = cv2.ml.KNearest_create()
	knn.train(des1,cv2.ml.ROW_SAMPLE,label1)
	ret, results, neighbours, dist = knn.findNearest(des2, k)

	threshold = 0.75
	good = []
	for i,d in enumerate(dist):
		if d[0] / d[1] < threshold:
			good.append([cv2.DMatch( i, int(neighbours[i][0]), d[0])])
	return good

def up_to_step_2(imgs):
	"""Complete pipeline up to step 2: Calculate matching feature points"""
	surf = cv2.xfeatures2d.SURF_create(2000)
	matched_imgs = []
	matched_list = []
	for i,img in enumerate(imgs):
		if i == len(imgs) - 1 :
			break
		kp1, des1 = surf.detectAndCompute(imgs[i], None)
		kp2, des2 = surf.detectAndCompute(imgs[i+1], None)

		imgs[i] = cv2.drawKeypoints(imgs[i], kp1, imgs[i])
		imgs[i+1] = cv2.drawKeypoints(imgs[i + 1], kp2, imgs[i + 1])

		matches_1 = matcher(des1, des2, 2)
		img_match_1 = cv2.drawMatchesKnn(imgs[i+1],kp2,imgs[i],kp1,matches_1,None,flags=2)
		match_name_1 = imgs_filename[i+1] +"_" + str(len(kp2)) + "_" + imgs_filename[i]+ "_"+ str(len(kp1)) + "_" + str(len(matches_1))+".jpg"

		matched_imgs.append(img_match_1)
		matched_list.append(match_name_1)

		matches_2 = matcher(des2, des1, 2)
		img_match_2 = cv2.drawMatchesKnn(imgs[i],kp1,imgs[i + 1],kp2,matches_2,None,flags=2)
		match_name_2 = imgs_filename[i] + "_" + str(len(kp1)) + "_" + imgs_filename[i+1] +"_"+ str(len(kp2)) + "_" + str(len(matches_2))+".jpg"

		matched_imgs.append(img_match_2)
		matched_list.append(match_name_2)
	return matched_imgs, matched_list


def save_step_2(imgs, match_list, output_path="./output/step2"):
	"""Save the intermediate result from Step 2"""
	if not os.path.exists(output_path):
		os.makedirs(output_path)
	for i, img in enumerate(imgs):
		 cv2.imwrite(os.path.join(output_path, match_list[i]),imgs[i])


#========================================= STEP 3 =========================================

def get_bestH_4_pionts(kp1, kp2, matches):

	max_count = 0
	best_H = np.zeros((3,3)).astype(np.float32)
	ransac_threshold = 3

	for j in range(2000):
		pts1 = np.zeros((4,2)).astype(np.float32)
		pts2 = np.zeros((4,2)).astype(np.float32)
		m_list = []
		for i in range(4):
			rand_num = random.randrange(len(matches))
			m = matches[rand_num]
			m_list.append(m)

		for i,p in enumerate(pts1):
			pts1[i] = [kp2[m_list[i][0].queryIdx].pt[0], kp2[m_list[i][0].queryIdx].pt[1]]
			pts2[i] = [kp1[m_list[i][0].trainIdx].pt[0], kp1[m_list[i][0].trainIdx].pt[1]]

		H = cv2.getPerspectiveTransform(pts1, pts2)
		count = 0
		for match in matches:
			x1 = kp2[match[0].queryIdx].pt[0]
			y1 = kp2[match[0].queryIdx].pt[1]
			x2 = kp1[match[0].trainIdx].pt[0]
			y2 = kp1[match[0].trainIdx].pt[1]

			w1 = (H[0][0]*x1 + H[0][1]*y1 + H[0][2])
			z1 = (H[2][0]*x1 + H[2][1]*y1 + H[2][2])

			w2 = (H[1][0]*x1 + H[1][1]*y1 + H[1][2])
			z2 = (H[2][0]*x1 + H[2][1]*y1 + H[2][2])

			distance = abs(x2 - ( w1 / z1 )) + abs(y2 - ( w2 / z1 ))

			if distance < ransac_threshold:
				count += 1

		if max_count < count:
			max_count = count
			best_H = H

	return best_H


def matcher_obj(img1, img2):
	surf = cv2.xfeatures2d.SURF_create(2000)
	kp1, des1 = surf.detectAndCompute(img1, None)
	kp2, des2 = surf.detectAndCompute(img2, None)
	matches_1 = matcher(des1, des2, 2)
	H = get_bestH_4_pionts(kp1, kp2, matches_1)
	return H

def get_left_H(H, img):
	xh = np.linalg.inv(H)
	ds = np.dot(xh, np.array([img.shape[1], img.shape[0], 1]));
	ds = ds/ds[-1]
	f1 = np.dot(xh, np.array([0,0,1]))
	f1 = f1/f1[-1]
	xh[0][-1] += abs(f1[0])
	xh[1][-1] += abs(f1[1])
	ds = np.dot(xh, np.array([img.shape[1], img.shape[0], 1]))
	offsety = abs(int(f1[1]))
	offsetx = abs(int(f1[0]))
	dsize = (int(ds[0])+offsetx, int(ds[1]) + offsety)
	return xh, dsize , offsetx, offsety


def up_to_step_3(imgs):
	"""Complete pipeline up to step 3: estimating homographies and warpings"""

	imgs_pairs = []
	for i,img in enumerate(imgs):
		if i == len(imgs) - 1 :
			break

		img1 = imgs[i]
		img2 = imgs[i+1]
		H1 = matcher_obj(img1, img2)
		H2 = matcher_obj(img2, img1)

		warp_1 = cv2.warpPerspective(imgs[i + 1], H1, (imgs[i+1].shape[1] + imgs[i].shape[1], imgs[i].shape[0]))

		xh, dsize, offsetx, offsety = get_left_H(H1, img1)
		warp_2 = cv2.warpPerspective(img1, H2, (imgs[i+1].shape[1], imgs[i].shape[0]))

		
		name_1 = "warped_"+ imgs_filename[i+1] +" (" + imgs_filename[i] + " as reference).jpg"
		name_2 = "warped_" + imgs_filename[i] +" (" + imgs_filename[i+1]  + " as reference).jpg"

		img_pair = [warp_1, warp_2]
		name_pair = [name_1, name_2]
		imgs_pairs.append([name_pair, img_pair])
	return imgs_pairs


def save_step_3(img_pairs, output_path="./output/step3"):
	"""Save the intermediate result from Step 3"""
	if not os.path.exists(output_path):
		os.makedirs(output_path)
	for name_pair, img_pair in img_pairs:
		name_1, name_2 = name_pair
		warp_1, warp_2 = img_pair
		cv2.imwrite(os.path.join(output_path, name_1),warp_1)
		cv2.imwrite(os.path.join(output_path, name_2),warp_2)


#========================================= STEP 4 =========================================


def prepare_lists(imgs):
	left_list, right_list = [], []
	centerIdx = int(len(imgs)/2)
	for i,img in enumerate(imgs):
		if(i<=centerIdx):
			left_list.append(img)
		else:
			right_list.append(img)
	return left_list, right_list

def leftshift(left_list):
	img1 = left_list[0]
	tmp = img1
	for img2 in left_list[1:]:

		H = matcher_obj(img1, img2)
		xh, dsize, offsetx, offsety = get_left_H(H, img1)
		tmp = cv2.warpPerspective(img1, xh, dsize)
		tmp[offsety:img2.shape[0]+offsety, offsetx:img2.shape[1]+offsetx] = img2
		img1 = tmp
	return tmp

def rightshift(leftImage, right_list):
	tmp = right_list[0]
	for each in right_list:
		H = matcher_obj(leftImage, each)
		txyz = np.dot(H, np.array([each.shape[1], each.shape[0], 1]))
		txyz = txyz/txyz[-1]
		dsize = (int(txyz[0])+100, int(txyz[1])+100)
		tmp = cv2.warpPerspective(each, H, dsize)
		tmp = mix_and_match(leftImage, tmp)
		leftImage = tmp
	return leftImage

def mix_and_match(leftImage, warpedImage):
	i1y, i1x = leftImage.shape[:2]
	i2y, i2x = warpedImage.shape[:2]

	black_l = np.where(leftImage == np.array([0,0,0]))
	black_wi = np.where(warpedImage == np.array([0,0,0]))

	for i in range(0, i1x):
		for j in range(0, i1y):
			try:
				if(np.array_equal(leftImage[j,i],np.array([0,0,0])) and  np.array_equal(warpedImage[j,i],np.array([0,0,0]))):
					warpedImage[j,i] = [0, 0, 0]
				else:
					if(np.array_equal(warpedImage[j,i],[0,0,0])):
						warpedImage[j,i] = leftImage[j,i]
					else:
						if not np.array_equal(leftImage[j,i], [0,0,0]):
							bw, gw, rw = warpedImage[j,i]
							bl,gl,rl = leftImage[j,i]
							warpedImage[j, i] = [bl,gl,rl]
			except:
				pass
	return warpedImage


def up_to_step_4(imgs):
	"""Complete the pipeline and generate a panoramic image"""
	left_list, right_list = prepare_lists(imgs)
	leftImage = leftshift(left_list)
	panoImage = rightshift(leftImage, right_list)
	return panoImage


def save_step_4(img, output_path="./output/step4"):
	"""Save the intermediate result from Step 4"""
	if not os.path.exists(output_path):
		os.makedirs(output_path)
	cv2.imwrite(os.path.join(output_path, "panoramic_img.jpg"),img)


if __name__ == "__main__":

	parser = argparse.ArgumentParser()
	parser.add_argument(
		"step",
		help="compute image stitching pipeline up to this step",
		type=int
	)

	parser.add_argument(
		"input",
		help="a folder to read in the input images",
		type=str
	)

	parser.add_argument(
		"output",
		help="a folder to save the outputs",
		type=str
	)

	args = parser.parse_args()

	imgs = []
	imgs_filename = []
	for filename in sorted(os.listdir(args.input)):
		print(filename)
		if not re.search(r'g$', filename):
			continue
		img = cv2.imread(os.path.join(args.input, filename))
		height = 600
		h,w,c = img.shape
		ratio = float(h)/float(w)
		width = int(ratio*height)
		img = cv2.resize(img, (height, width))
		imgs.append(img)
		imgs_filename.append(filename)

	if args.step == 1:
		print("Running step 1")
		modified_imgs = up_to_step_1(imgs)
		save_step_1(modified_imgs, args.output)
	elif args.step == 2:
		print("Running step 2")
		modified_imgs, match_list = up_to_step_2(imgs)
		save_step_2(modified_imgs, match_list, args.output)
	elif args.step == 3:
		print("Running step 3")
		img_pairs = up_to_step_3(imgs)
		save_step_3(img_pairs, args.output)
	elif args.step == 4:
		print("Running step 4")
		panoramic_img = up_to_step_4(imgs)
		save_step_4(panoramic_img, args.output)
