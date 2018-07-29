## Submission.py for COMP6714-Project2
###################################################################################################################
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import collections
import math
import os
import random
import zipfile
import spacy
import gensim

import numpy as np
from six.moves import urllib
from six.moves import xrange  # pylint: disable=redefined-builtin
import tensorflow as tf

nlp = spacy.load('en')

vocabulary_size = 50000 # This variable is used to define the maximum vocabulary size.

def build_dataset(words, n_words):
    """Process raw inputs into a dataset. 
       words: a list of words, i.e., the input data
       n_words: Vocab_size to limit the size of the vocabulary. Other words will be mapped to 'UNK'
    """
    count = [['UNK', -1]]
    count.extend(collections.Counter(words).most_common(n_words - 1))
    dictionary = dict()
    for word, _ in count:
        dictionary[word] = len(dictionary)
    data = list()
    unk_count = 0
    for word in words:
        index = dictionary.get(word, 0)
        if index == 0:  # i.e., one of the 'UNK' words
            unk_count += 1
        data.append(index)
    count[0][1] = unk_count
    reversed_dictionary = dict(zip(dictionary.values(), dictionary.keys()))
    return data, count, dictionary, reversed_dictionary

data, count, dictionary, reverse_dictionary = build_dataset(data, vocabulary_size)

def read_data(filename):
    with zipfile.ZipFile(filename) as f:
        for info in f.infolist():
            if not info.is_dir():
                with f.open(info.filename) as text:
                    print(text.read().split())

read_data("./BBC_Data.zip")

def process_data(input_data):
    pass

def adjective_embeddings(data_file, embeddings_file_name, num_steps, embedding_dim):
    pass # Remove this pass line, you need to implement your code for Adjective Embeddings here...



def Compute_topk(model_file, input_adjective, top_k):
    pass # Remove this pass line, you need to implement your code to compute top_k words similar to input_adjective