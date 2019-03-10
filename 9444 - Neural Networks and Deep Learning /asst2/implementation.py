import tensorflow as tf
import re

RNN_SIZE = 64  # RNN layer size
ATTENTION_SIZE = 10  # Attention layer size
LEARNING_RATE = 0.0001  # Training learning rate
RNN_LAYERS = 1  # LSTM RNN Cell layer 

BATCH_SIZE = 128
MAX_WORDS_IN_REVIEW = 200  # Maximum length of a review to consider
EMBEDDING_SIZE = 50  # Dimensions for each word vector

stop_words = set({'ourselves', 'hers', 'between', 'yourself', 'again', 'there', 'about', 'once', 'during', 'out', 'very', 'having', 'with', 'they', 'own', 'an', 'be', 'some', 'for', 'do', 'its', 'yours', 'such', 'into', 'of', 'most', 'itself', 'other', 'off', 'is', 's', 'am', 'or', 'who', 'as', 'from', 'him', 'each', 'the', 'themselves', 'below', 'are', 'we', 'these', 'your', 'his', 'through', 'don', 'me', 'were', 'her', 'more', 'himself', 'this', 'down', 'should', 'our', 'their', 'while', 'above', 'both', 'up', 'to', 'ours', 'had', 'she', 'all', 'no', 'when', 'at', 'any', 'before', 'them', 'same', 'and', 'been', 'have', 'in', 'will', 'on', 'does', 'yourselves', 'then', 'that', 'because', 'what', 'over', 'why', 'so', 'can', 'did', 'not', 'now', 'under', 'he', 'you', 'herself', 'has', 'just', 'where', 'too', 'only', 'myself', 'which', 'those', 'i', 'after', 'few', 'whom', 't', 'being', 'if', 'theirs', 'my', 'against', 'a', 'by', 'doing', 'it', 'how', 'further', 'was', 'here', 'than', 'but', 'br', 'little', 'him', 'her', 'many', 'however', 'one', 'still', 'two', 'much', 'call', 'beside', 'get', 'due', 'indeed', 'five', 'already', 'besides', 'either', 'least', 'keep', 'whence', 'quite', 'within', 'could', 'say', 'using', 'whereas', 're', 'hereupon', 'meanwhile', 'whereupon', 'wherein', 'third', 'hereafter', 'done', 'name', 'top', 'four', 'well', 'amongst', 'whenever', 'seems', 'forty', 'herein', 'must', 'mostly', 'fifteen', 'make', 'beforehand', 'take', 'anywhere', 'every', 'per', 'become', 'moreover', 'seem', 'full', 'empty', 'put', 'sixty', 'side', 'three', 'hereby', 'serious', 'please', 'first', 'anyway', 'else', 'twelve', 'afterwards', 'thereupon', 'also', 'became', 'several', 'almost', 'everyone', 'via', 'elsewhere', 'until', 'made', 'last', 'nowhere', 'along', 'whereby', 'sometimes', 'show', 'even', 'around', 'whither', 'formerly', 'various', 'move', 'without', 'cannot', 'give', 'ten', 'beyond', 'another', 'eight', 'always', 'upon', 'across', 'somewhere', 'onto', 'anyhow', 'fifty', 'thru', 'often', 'mine', 'seeming', 'toward', 'us', 'although', 'see', 'really', 'ever', 'bottom', 'latter', 'since', 'eleven', 'together', 'whose', 'none', 'front', 'seemed', 'someone', 'twenty', 'hundred', 'nobody', 'everywhere', 'everything', 'nevertheless', 'perhaps', 'would', 'next', 'thereafter', 'less', 'whatever', 'neither', 'nothing', 'go', 'becomes', 'latterly', 'towards', 'throughout', 'may', 'alone', 'others', 'nor', 'part', 'thence', 'though', 'unless', 'regarding', 'somehow', 'behind', 'back', 'whoever', 'therein', 'sometime', 'whereafter', 'amount', 'rather', 'thereby', 'therefore', 'used', 'might', 'whether', 'wherever', 'hence', 'anything', 'never', 'yet', 'six', 'anyone', 'whole', 'becoming', 'enough', 'except', 'former', 'noone', 'thus', 'nine', 'something', 'among', 'otherwise', 'namely', 'ca', 'saw', 'etc', 'let', 'wasn', 'oh', 'to', 'jo', 'movie', 'movies', 'film', 'films', 'seen', 'time', 'story', 'think'})

def preprocess(review):
    """
    Apply preprocessing to a single review. You can do anything here that is manipulation
    at a string level, e.g.
        - removing stop words
        - stripping/adding punctuation
        - changing case
        - word find/replace
    RETURN: the preprocessed review in string form.
    """
    
    processed_review = []
    string = review.lower();
    tokens = [i for i in re.split(r'([\d.]+|\W+)', string) if i]
    for token in tokens:
      if re.search("\W+", token):
        continue  
      if re.search("\s+", token):
        continue  
      if re.search("[.,$-;:<>]", token):
        continue
      if token in stop_words:
        continue
      if len(token) < 3:
        continue
      processed_review.append(token)
    return processed_review


def attention_mechanism(rnn_inputs, attention_size):
    len = rnn_inputs.shape[1].value
    size = rnn_inputs.shape[2].value
    b = tf.Variable(tf.truncated_normal([attention_size], stddev=0.1))
    w = tf.Variable(tf.truncated_normal([size, attention_size], stddev=0.1))
    p = tf.Variable(tf.truncated_normal([attention_size], stddev=0.1))
    q = tf.tanh(tf.matmul(tf.reshape(rnn_inputs, [-1, size]), w) + tf.reshape(b, [1, -1]))
    qp = tf.matmul(q, tf.reshape(p, [-1, 1]))
    exps = tf.reshape(tf.exp(qp), [-1, len])
    alphas = exps / tf.reshape(tf.reduce_sum(exps, 1), [-1, 1])
    output = tf.reduce_sum(rnn_inputs * tf.reshape(alphas, [-1, len, 1]), 1)
    return output

def define_graph():
    """
    Implement your model here. You will need to define placeholders, for the input and labels,
    Note that the input is not strings of words, but the strings after the embedding lookup
    has been applied (i.e. arrays of floats).

    In all cases this code will be called by an unaltered runner.py. You should read this
    file and ensure your code here is compatible.

    Consult the assignment specification for details of which parts of the TF API are
    permitted for use in this function.

    You must return, in the following order, the placeholders/tensors for;
    RETURNS: input, labels, optimizer, accuracy and loss
    """
    
    # Input Data
    input_data = tf.placeholder(tf.float32, [BATCH_SIZE, MAX_WORDS_IN_REVIEW ,EMBEDDING_SIZE],name='input_data')  # 128 * 200 * 50
    labels = tf.placeholder(tf.float32, [BATCH_SIZE, 2], name='labels') # 128 * 2
    dropout_keep_prob = tf.placeholder_with_default(0.6, shape=None, name='dropout_keep_prob')
    
    # RNN
    def cell():
      gru = tf.contrib.rnn.GRUCell(RNN_SIZE)
      cell = tf.contrib.rnn.DropoutWrapper(gru, dropout_keep_prob) 
      return cell

    with tf.variable_scope('init_name', initializer=tf.orthogonal_initializer()):
      cell = tf.contrib.rnn.MultiRNNCell([cell()] * RNN_LAYERS)
      rnn_outputs, final_state = tf.nn.dynamic_rnn(cell, input_data, dtype = "float32")

    # Attention
    atten_output = attention_mechanism(rnn_outputs, ATTENTION_SIZE)
  
    # Full-connect
    w = tf.Variable(tf.truncated_normal([atten_output.get_shape()[1].value, 2], stddev=0.1)) # 128 * 2
    b = tf.Variable(tf.constant(0., shape=[2])) # 2,
    logits = tf.nn.xw_plus_b(atten_output, w, b)
    logits = tf.squeeze(logits)

    # Loss and accuracy
    loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits_v2(logits=logits, labels=labels), name = "loss")
    optimizer = tf.train.AdamOptimizer(LEARNING_RATE).minimize(loss)
    accuracy = tf.reduce_mean(tf.cast(tf.equal(tf.round(tf.sigmoid(logits)), labels), tf.float32), name = "accuracy")

    return input_data, labels, dropout_keep_prob, optimizer, accuracy, loss
