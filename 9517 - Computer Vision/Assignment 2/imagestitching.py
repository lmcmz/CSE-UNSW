from __future__ import print_function  #
import cv2
import argparse
import os


def up_to_step_1(imgs):
    """Complete pipeline up to step 3: Detecting features and descriptors"""
    # ... your code here ...
    return imgs


def save_step_1(imgs, output_path='./output/step1'):
    """Save the intermediate result from Step 1"""
    # ... your code here ...
    pass


def up_to_step_2(imgs):
    """Complete pipeline up to step 2: Calculate matching feature points"""
    # ... your code here ...
    return imgs, []


def save_step_2(imgs, match_list, output_path="./output/step2"):
    """Save the intermediate result from Step 2"""
    # ... your code here ...
    pass


def up_to_step_3(imgs):
    """Complete pipeline up to step 3: estimating homographies and warpings"""
    # ... your code here ...
    return imgs


def save_step_3(img_pairs, output_path="./output/step3"):
    """Save the intermediate result from Step 3"""
    # ... your code here ...
    pass


def up_to_step_4(imgs):
    """Complete the pipeline and generate a panoramic image"""
    # ... your code here ...
    return imgs[0]


def save_step_4(imgs, output_path="./output/step4"):
    """Save the intermediate result from Step 4"""
    # ... your code here ...
    pass


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
    for filename in os.listdir(args.input):
        print(filename)
        img = cv2.imread(os.path.join(args.input, filename))
        imgs.append(img)

    if args.step == 1:
        print("Running step 1")
        modified_imgs = up_to_step_1(imgs)
        save_step_1(imgs, args.output)
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
        save_step_4(img_pairs, args.output)
