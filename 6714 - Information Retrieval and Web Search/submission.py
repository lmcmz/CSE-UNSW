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
from numpy import array
import gensim

import numpy as np
from six.moves import urllib
import tensorflow as tf
from six.moves import range
from six.moves.urllib.request import urlretrieve
from sklearn.manifold import TSNE

nlp = spacy.load('en')

data_index = 0 
vocabulary_size = 10000
batch_size = 128      # Size of mini-batch for skip-gram model.
skip_window = 2       # How many words to consider left and right of the target word.
num_samples = 4       # How many times to reuse an input to generate a label.
num_sampled = 64     # Sample size for negative examples.
logs_path = './log/'

def process_data(input_data):
    with zipfile.ZipFile(input_data) as f:
        word_list = list()
        word_string = ""
        for info in f.infolist():
            if not info.is_dir():
                with f.open(info.filename) as text:
                    string = tf.compat.as_str(text.read()).lower()
                    string = re.sub('\W', ' ', string)
                    string = re.sub('\s+', ' ', string)
                    string = re.sub('[.,$-;:]', '', string)
                    #word_list.extend(string.split())
                    word_string += string
    doc = nlp(word_string)
    for token in doc:
        if re.match('[ \t]+', token.text):
            continue
        if len(token.text) < 3:        #filter 
            continue
        if len(token.lemma_) < 3:        #filter 
            continue
        if token.is_stop:        #filter stop words
            continue
        if not token.text.isalpha():    #filter no words (ie. number)
            continue
        if not token.pos_ == "ADJ":
            if token.pos_ == "PRON":
                word_list.append(token.text)
            else:
                word_list.append(token.lemma_)
        else:
            word_list.append(token.text)
    f.close()
    return word_list

def generate_batch(batch_size, num_samples, skip_window,data,count,dictionary,reverse_dictionary):
    global data_index
    
    assert batch_size % num_samples == 0
    assert num_samples <= 2 * skip_window
    
    batch = np.ndarray(shape=(batch_size), dtype=np.int32)
    labels = np.ndarray(shape=(batch_size, 1), dtype=np.int32)
    span = 2 * skip_window + 1  # span is the width of the sliding window
    buffer = collections.deque(maxlen=span)
    if data_index + span > len(data):
        data_index = 0
    buffer.extend(data[data_index:data_index + span]) # initial buffer content = first sliding window
    
#    print('data_index = {}, buffer = {}'.format(data_index, [reverse_dictionary[w] for w in buffer]))

    data_index += span
    for i in range(batch_size // num_samples):
        context_words = [w for w in range(span) if w != skip_window]
        random.shuffle(context_words)
        words_to_use = collections.deque(context_words) # now we obtain a random list of context words
        for j in range(num_samples): # generate the training pairs
            batch[i * num_samples + j] = buffer[skip_window]
            context_word = words_to_use.pop()
            labels[i * num_samples + j, 0] = buffer[context_word] # buffer[context_word] is a random context word
        
        # slide the window to the next position    
        if data_index == len(data):
            buffer = data[:span]
            data_index = span
        else: 
            buffer.append(data[data_index]) # note that due to the size limit, the left most word is automatically removed from the buffer.
            data_index += 1
        
        print('data_index = {}, buffer = {}'.format(data_index, [reverse_dictionary[w] for w in buffer]))
        
    # end-of-for
    data_index = (data_index + len(data) - span) % len(data) # move data_index back by `span`
    return batch, labels

def adjective_embeddings(data_file, embedding_file_name, num_steps, embedding_dim):
    count = [['UNK', -1]]
    count.extend(collections.Counter(data_file).most_common(vocabulary_size - 1))
    dictionary = dict()
    for word, _ in count:
        dictionary[word] = len(dictionary)
    data = list()
    unk_count = 0
    for word in data_file:
        index = dictionary.get(word, 0)
        if index == 0:  # i.e., one of the 'UNK' words
            unk_count += 1
        data.append(index)
    count[0][1] = unk_count
    reverse_dictionary = dict(zip(dictionary.values(), dictionary.keys()))
    
    graph = tf.Graph()
    with graph.as_default():
        with tf.device('/cpu:0'):
            # Placeholders to read input data.
            with tf.name_scope('Inputs'):
                train_inputs = tf.placeholder(tf.int32, shape=[batch_size])
                train_labels = tf.placeholder(tf.int32, shape=[batch_size, 1])
                
            # Look up embeddings for inputs.
            with tf.name_scope('Embeddings'):            
#                sample_dataset = tf.constant(sample_examples, dtype=tf.int32)
                embeddings = tf.Variable(tf.random_uniform([vocabulary_size, embedding_dim], -1.0, 1.0))
                embed = tf.nn.embedding_lookup(embeddings, train_inputs)

                # Construct the variables for the NCE loss
                nce_weights = tf.Variable(tf.truncated_normal([vocabulary_size, embedding_dim],
                                                          stddev=1.0 / math.sqrt(embedding_dim)))
                nce_biases = tf.Variable(tf.zeros([vocabulary_size]))
            print(embed)
            # Compute the average NCE loss for the batch.
            # tf.nce_loss automatically draws a new sample of the negative labels each
            # time we evaluate the loss.
            with tf.name_scope('Loss'):
                loss = tf.reduce_mean(tf.nn.sampled_softmax_loss(weights=nce_weights, biases=nce_biases, inputs=embed,labels=train_labels, num_sampled=num_sampled, num_classes=vocabulary_size))
            
            # Construct the Adam optimizer using a learning rate of 0.002.
            with tf.name_scope('Adam'):
                optimizer = tf.train.AdamOptimizer(0.002).minimize(loss)
                #optimizer = tf.train.GradientDescentOptimizer(learning_rate = 1).minimize(loss)

            # Normalize the embeddings to avoid overfitting.
            with tf.name_scope('Normalization'):
                norm = tf.sqrt(tf.reduce_sum(tf.square(embeddings), 1, keep_dims=True))
                normalized_embeddings = embeddings / norm
            
            # Add variable initializer.
            init = tf.global_variables_initializer()
            
            # Create a summary to monitor cost tensor
            tf.summary.scalar("cost", loss)
            # Merge all summary variables.
            merged_summary_op = tf.summary.merge_all()

    with tf.Session(graph=graph) as session:
        # We must initialize all variables before we use them.
        session.run(init)
        summary_writer = tf.summary.FileWriter(logs_path, graph=tf.get_default_graph())
        
        print('Initializing the model')
        
        average_loss = 0
        for step in range(num_steps):
            batch_inputs, batch_labels = generate_batch(batch_size, num_samples, skip_window,data,count,dictionary,reverse_dictionary)
            feed_dict = {train_inputs: batch_inputs, train_labels: batch_labels}
            
            # We perform one update step by evaluating the optimizer op using session.run()
            _, loss_val, summary = session.run([optimizer, loss, merged_summary_op], feed_dict=feed_dict)
            
            summary_writer.add_summary(summary, step )
            average_loss += loss_val

            if step % 5000 == 0:
                if step > 0:
                    average_loss /= 5000
                
                    # The average loss is an estimate of the loss over the last 5000 batches.
                    print('Average loss at step ', step, ': ', average_loss)
                    average_loss = 0

        final_embeddings = normalized_embeddings.eval()
        
    with gensim.utils.smart_open(embedding_file_name,'wb',encoding='utf-8') as f:
        f.write(gensim.utils.to_utf8("{} {}\n".format(vocabulary_size, embedding_dim)))
        for i in range(vocabulary_size):
            row = final_embeddings[i]
            word = reverse_dictionary[i].strip()
            f.write(gensim.utils.to_utf8("{} {}\n".format(word, ' '.join(str(val) for val in row))))

def Compute_topk(model_file, input_adjective, top_k):
    model = gensim.models.KeyedVectors.load_word2vec_format(model_file, binary=False)
    result = []
    for words,number in model.most_similar(input_adjective, topn=top_k):
        result.append(words)
    return result


