from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import collections
import math
import os
import random
from tempfile import gettempdir
import zipfile
import re
import spacy

import numpy as np
from six.moves import urllib
import tensorflow as tf
from six.moves import range
from six.moves.urllib.request import urlretrieve
from sklearn.manifold import TSNE

def read_data(filename):
	with zipfile.ZipFile(filename) as f:
		word_list = ""
		for info in f.infolist():
			if not info.is_dir():
				with f.open(info.filename) as text:
					string = tf.compat.as_str(text.read()).lower()
					#string = re.sub('\W', ' ', string)
					#string = re.sub('\s+', ' ', string)
					word_list += string
					break
	return word_list
	
data = read_data('./BBC_Data.zip')
nlp = spacy.load("en")
doc = nlp(data)

for word in doc:
	if word.pos_ == "ADJ":
		print(word)