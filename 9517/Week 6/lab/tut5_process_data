""" Code adapted from Steven Hurwitt:
https://www.kaggle.com/stevenhurwitt/cats-vs-dogs-using-a-keras-convnet

who adapted it from Jeff Delaney:
https://www.kaggle.com/jeffd23/catdognet-keras-convnet-starter
"""

import os, cv2, random
import numpy as np

TRAIN_DIR = './train/'

ROWS2 = 64
COLS2 = 64
CHANNELS = 3

# train_images = [TRAIN_DIR+i for i in os.listdir(TRAIN_DIR)]  # use this for full dataset
train_dogs = [TRAIN_DIR+i for i in os.listdir(TRAIN_DIR) if 'dog' in i]
train_cats = [TRAIN_DIR+i for i in os.listdir(TRAIN_DIR) if 'cat' in i]


# slice datasets for memory efficiency, delete if using full dataset
train_images = train_dogs[:2000] + train_cats[:2000]
random.shuffle(train_images)


def read_image(file_path):
    img = cv2.imread(file_path, cv2.IMREAD_COLOR)
    b, g, r = cv2.split(img)
    img2 = cv2.merge([r, g, b])
    return cv2.resize(img2, (ROWS2, COLS2), interpolation=cv2.INTER_CUBIC)


def prep_data(images):
    count = len(images)
    data = np.ndarray((count, CHANNELS, ROWS2, COLS2), dtype=np.uint8)

    for i, image_file in enumerate(images):
        image = read_image(image_file)
        data[i] = image.T
        if i % 5000 == 0: print('Processed {} of {}'.format(i, count))
    return data


train = prep_data(train_images)

print("Train shape: {}".format(train.shape))


labels = []
for i in train_images:
    if 'dog' in i:
        labels.append(1)
    else:
        labels.append(0)

np.save("train.out",  train)
np.save("labels.out",  labels)
