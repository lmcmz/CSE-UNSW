import gym
import tensorflow as tf
import numpy as np
import random

# General Parameters
# -- DO NOT MODIFY --
ENV_NAME = 'CartPole-v0'
EPISODE = 501  # Episode limitation
STEP = 200  # Step limitation in an episode
TEST = 10  # The number of tests to run every TEST_FREQUENCY episodes
TEST_FREQUENCY = 100  # Num episodes to run before visualizing test accuracy

# TODO: HyperParameters
GAMMA = 0.95  # discount factor
INITIAL_EPSILON = 0.9  # starting value of epsilon  greedy
FINAL_EPSILON = 0.1  # final value of epsilon
EPSILON_DECAY_STEPS = 100  # decay period

HIDDEN_LAYER = 100

BATCH_SIZE = 200
REPLAY_SIZE = 5000
batch_list = []

# Create environment
# -- DO NOT MODIFY --
env = gym.make(ENV_NAME)
epsilon = INITIAL_EPSILON
STATE_DIM = env.observation_space.shape[0]
ACTION_DIM = env.action_space.n

# TODO: Define Network Graph
def cnn(state_in, state_dim, hidden_layer_1, action_dim):
    W1 = tf.get_variable("W1", shape=[state_dim, hidden_layer_1],)
    b1 = tf.get_variable(
        "b1", shape=[1, hidden_layer_1], initializer=tf.constant_initializer(0.0))

    W2 = tf.get_variable("W2", shape=[hidden_layer_1, action_dim],)
    b2 = tf.get_variable(
        "b2", shape=[1, action_dim], initializer=tf.constant_initializer(0.0))

    layer_1 = tf.matmul(state_in, W1) + b1
    output_layer_1 = tf.tanh(layer_1)

    layer_2 = tf.matmul(output_layer_1, W2) + b2

    return layer_2

# -- DO NOT MODIFY ---
def explore(state, epsilon):
    """
    Exploration function: given a state and an epsilon value,
    and assuming the network has already been defined, decide which action to
    take using e-greedy exploration based on the current q-value estimates.
    """
    Q_estimates = q_values.eval(feed_dict={
        state_in: [state]
    })
    if random.random() <= epsilon:
        action = random.randint(0, ACTION_DIM - 1)
    else:
        action = np.argmax(Q_estimates)
    one_hot_action = np.zeros(ACTION_DIM)
    one_hot_action[action] = 1
    return one_hot_action


def get_train_batch(q_values, state_in, sample_batch, size):
    state_batch = [data[0] for data in sample_batch]
    action_batch = [data[1] for data in sample_batch]
    reward_batch = [data[2] for data in sample_batch]
    next_state_batch = [data[3] for data in sample_batch]

    target_batch = []
    Q_value_batch = q_values.eval(feed_dict={
        state_in: next_state_batch
    })

    for i in range(0, size):
        sample_is_done = sample_batch[i][4]
        if sample_is_done:
            target_batch.append(reward_batch[i])
        else:
            target_val = reward_batch[i] + GAMMA * np.max(Q_value_batch[i])
            target_batch.append(target_val)
    return target_batch, state_batch, action_batch


# Placeholders
# -- DO NOT MODIFY --
state_in = tf.placeholder("float", [None, STATE_DIM])
action_in = tf.placeholder("float", [None, ACTION_DIM])
target_in = tf.placeholder("float", [None])

# Network outputs
q_values = cnn(state_in, STATE_DIM, HIDDEN_LAYER, ACTION_DIM)
q_action = tf.reduce_sum(tf.multiply(q_values, action_in), reduction_indices=1)

# Loss/Optimizer Definition
loss = tf.reduce_sum(tf.square(target_in - q_action))
optimizer = tf.train.AdamOptimizer().minimize(loss)

# Start session - Tensorflow housekeeping
session = tf.InteractiveSession()
session.run(tf.global_variables_initializer())

# Main learning loop
for episode in range(EPISODE):
    # initialize task
    state = env.reset()
    env.render()
    # Update epsilon once per episode
    epsilon -= (epsilon - FINAL_EPSILON) / EPSILON_DECAY_STEPS

    if epsilon < 0:
        epsilon = 0

    e_reward = 0
    # Move through env according to e-greedy policy
    for step in range(STEP):
        action = explore(state, epsilon)
        next_state, reward, done, _ = env.step(np.argmax(action))

        nextstate_q_values = q_values.eval(feed_dict={
            state_in: [next_state]
        })
        
        e_reward += reward

        # env.render() # show animation
        batch_list.append((state, action, reward, next_state, done)) ## add to batch
        
        if len(batch_list) > REPLAY_SIZE:
            batch_list.pop(0)

        # Update
        state = next_state

        if (len(batch_list) > BATCH_SIZE): ## Less than BATCH SIZE, do not train
            size = BATCH_SIZE-50*int(episode/TEST_FREQUENCY)
            if size < 1:
                size = 1
#            print("episode:" +str(episode))
#            print("size:" +str(size))
            minibatch = random.sample(batch_list, size)
            target_batch, state_batch, action_batch = get_train_batch(
                q_values, state_in, minibatch, size)

            session.run([optimizer], feed_dict={
                target_in: target_batch,
                action_in: action_batch,
                state_in: state_batch
            })

        if done:
            break

#    if (episode % 1 == 0 and episode != 0):
#        print('episode:', episode, 'Reward', e_reward)

    # Test and view sample runs - can disable render to save time
    # -- DO NOT MODIFY --
    if (episode % TEST_FREQUENCY == 0 and episode != 0):
        total_reward = 0
        for i in range(TEST):
            state = env.reset()
            for j in range(STEP):
                env.render()
                action = np.argmax(q_values.eval(feed_dict={
                    state_in: [state]
                }))
                state, reward, done, _ = env.step(action)
                total_reward += reward
                if done:
                    break
        ave_reward = total_reward / TEST
        print('episode:', episode, 'epsilon:', epsilon, 'Evaluation '
                                                        'Average Reward:', ave_reward)

env.close()
