import sys
import gym
import tensorflow as tf
import numpy as np
import random
import datetime

"""
Hyper Parameters
"""
GAMMA = 0.9  # discount factor for target Q
INITIAL_EPSILON = 0.6  # starting value of epsilon
FINAL_EPSILON = 0.1  # final value of epsilon
EPSILON_DECAY_STEPS = 100
REPLAY_SIZE = 10000  # experience replay buffer size
BATCH_SIZE = 256  # size of minibatch
TEST_FREQUENCY = 10  # How many episodes to run before visualizing test accuracy
SAVE_FREQUENCY = 1000  # How many episodes to run before saving model (unused)
NUM_EPISODES = 100  # Episode limitation
EP_MAX_STEPS = 1000  # Step limitation in an episode
# The number of test iters (with epsilon set to 0) to run every TEST_FREQUENCY episodes
NUM_TEST_EPS = 4
HIDDEN_NODES = 100

# Global variables


def init(env, env_name):
		"""
		Initialise any globals, e.g. the replay_buffer, epsilon, etc.
		return:
				state_dim: The length of the state vector for the env
				action_dim: The length of the action space, i.e. the number of actions

		NB: for discrete action envs such as the cartpole and mountain car, this
		function can be left unchanged.

		Hints for envs with continuous action spaces, e.g. "Pendulum-v0"
		1) you'll need to modify this function to discretise the action space and
		create a global dictionary mapping from action index to action (which you
		can use in `get_env_action()`)
		2) for Pendulum-v0 `env.action_space.low[0]` and `env.action_space.high[0]`
		are the limits of the action space.
		3) setting a global flag iscontinuous which you can use in `get_env_action()`
		might help in using the same code for discrete and (discretised) continuous
		action spaces
		"""
		global replay_buffer, epsilon
		replay_buffer = []
		epsilon = INITIAL_EPSILON

		state_dim = env.observation_space.shape[0]
		action_dim = env.action_space.n
		return state_dim, action_dim


def deep_nn(state_in, state_dim, hidden_nodes, action_dim):
		# Define W and b of the first layer
		W1 = tf.get_variable("W1", shape=[state_dim, hidden_nodes],)
		b1 = tf.get_variable("b1", shape=[1, hidden_nodes], initializer = tf.constant_initializer(0.0))

		# Define W and b of the second layer
		W2 = tf.get_variable("W2", shape=[hidden_nodes, hidden_nodes],)
		b2 = tf.get_variable("b2", shape=[1, hidden_nodes], initializer = tf.constant_initializer(0.0))

		# Define W and b of the third layer
		W3 = tf.get_variable("W3", shape=[hidden_nodes, action_dim])
		b3 = tf.get_variable("b3", shape=[1, action_dim], initializer = tf.constant_initializer(0.0))

		# Layer1
		logits_layer1 = tf.matmul(state_in, W1) + b1
		output_layer1 = tf.tanh(logits_layer1)  # tf.sigmoid(logits_layer1) or tf.tanh(logits_layer1) or ... ?

		# Layer2
		logits_layer2 = tf.matmul(output_layer1, W2) + b2
		output_layer2 = tf.tanh(logits_layer2)  # tf.sigmoid(logits_layer2) or tf.tanh(logits_layer2) or ... ?

		# Layer3
		logits_layer3 = tf.matmul(output_layer2, W3) + b3
		output_layer3 = logits_layer3  # tf.sigmoid(logits_layer3) or tf.tanh(logits_layer3) or ... ?
		
		return output_layer3


def get_network(state_dim, action_dim, hidden_nodes=HIDDEN_NODES):
		"""Define the neural network used to approximate the q-function

		The suggested structure is to have each output node represent a Q value for
		one action. e.g. for cartpole there will be two output nodes.

		Hints:
		1) Given how q-values are used within RL, is it necessary to have output
		activation functions?
		2) You will set `target_in` in `get_train_batch` further down. Probably best
		to implement that before implementing the loss (there are further hints there)
		"""
		state_in = tf.placeholder("float", [None, state_dim])
		action_in = tf.placeholder("float", [None, action_dim])  # one hot
		target_in = tf.placeholder("float", [None])  # q value for the target network

		# TO IMPLEMENT: Q network, whose input is state_in, and has action_dim outputs
		# which are the network's esitmation of the Q values for those actions and the
		# input state. The final layer should be assigned to the variable q_values


		q_values = deep_nn(state_in, state_dim, hidden_nodes, action_dim)

		q_selected_action = \
				tf.reduce_sum(tf.multiply(q_values, action_in), reduction_indices=1)

		# TO IMPLEMENT: loss function
		# should only be one line, if target_in is implemented correctly
		
		loss = tf.reduce_sum(tf.square(target_in - q_selected_action))

		optimise_step = tf.train.AdamOptimizer().minimize(loss)

		train_loss_summary_op = tf.summary.scalar("TrainingLoss", loss)
		return state_in, action_in, target_in, q_values, q_selected_action, \
					 loss, optimise_step, train_loss_summary_op


def init_session():
		global session, writer
		session = tf.InteractiveSession()
		session.run(tf.global_variables_initializer())

		# Setup Logging
		logdir = "tensorboard/" + datetime.datetime.now().strftime(
				"%Y%m%d-%H%M%S") + "/"
		writer = tf.summary.FileWriter(logdir, session.graph)


def get_action(state, state_in, q_values, epsilon, test_mode, action_dim):

		Q_estimates = q_values.eval(feed_dict={state_in: [state]})[0]
		epsilon_to_use = 0.0 if test_mode else epsilon
		if random.random() < epsilon_to_use:
				action = random.randint(0, action_dim - 1)
		else:
				action = np.argmax(Q_estimates)
		return action


def get_env_action(action):
		"""
		Modify for continous action spaces that you have discretised, see hints in
		`init()`
		"""
		return action


def update_replay_buffer(replay_buffer, state, action, reward, next_state, done,
												 action_dim):
		"""
		Update the replay buffer with provided input in the form:
		(state, one_hot_action, reward, next_state, done)

		Hint: the minibatch passed to do_train_step is one entry (randomly sampled)
		from the replay_buffer
		"""
		# TO IMPLEMENT: append to the replay_buffer
		# ensure the action is encoded one hot
		one_hot_action = np.zeros(action_dim)
		one_hot_action[action] = 1

		# print('Action is: {0}'.format(action))

		# append to buffer
		replay_buffer.append((state, one_hot_action, reward, next_state, done))
		# Ensure replay_buffer doesn't grow larger than REPLAY_SIZE
		if len(replay_buffer) > REPLAY_SIZE:
				replay_buffer.pop(0)
		return None


def do_train_step(replay_buffer, state_in, action_in, target_in,
									q_values, q_selected_action, loss, optimise_step,
									train_loss_summary_op, batch_presentations_count):
		minibatch = random.sample(replay_buffer, BATCH_SIZE)
		target_batch, state_batch, action_batch = \
				get_train_batch(q_values, state_in, minibatch)

		summary, _ = session.run([train_loss_summary_op, optimise_step], feed_dict={
				target_in: target_batch,
				state_in: state_batch,
				action_in: action_batch
		})
		writer.add_summary(summary, batch_presentations_count)


def get_train_batch(q_values, state_in, minibatch):
		"""
		Generate Batch samples for training by sampling the replay buffer"
		Batches values are suggested to be the following;
				state_batch: Batch of state values
				action_batch: Batch of action values
				target_batch: Target batch for (s,a) pair i.e. one application
						of the bellman update rule.

		return:
				target_batch, state_batch, action_batch

		Hints:
		1) To calculate the target batch values, you will need to use the
		q_values for the next_state for each entry in the batch.
		2) The target value, combined with your loss defined in `get_network()` should
		reflect the equation in the middle of slide 12 of Deep RL 1 Lecture
		notes here: https://webcms3.cse.unsw.edu.au/COMP9444/17s2/resources/12494
		"""
		state_batch = [data[0] for data in minibatch]
		action_batch = [data[1] for data in minibatch]
		reward_batch = [data[2] for data in minibatch]
		next_state_batch = [data[3] for data in minibatch]

		target_batch = []
		Q_value_batch = q_values.eval(feed_dict={
				state_in: next_state_batch
		})

		global GAMMA

		for i in range(0, BATCH_SIZE):
				sample_is_done = minibatch[i][4]
				if sample_is_done:
						target_batch.append(reward_batch[i])
				else:
						# TO IMPLEMENT: set the target_val to the correct Q value update
						target_val = reward_batch[i] + GAMMA * np.max(Q_value_batch[i])
						target_batch.append(target_val)
		return target_batch, state_batch, action_batch


def qtrain(env, state_dim, action_dim,
					 state_in, action_in, target_in, q_values, q_selected_action,
					 loss, optimise_step, train_loss_summary_op,
					 num_episodes=NUM_EPISODES, ep_max_steps=EP_MAX_STEPS,
					 test_frequency=TEST_FREQUENCY, num_test_eps=NUM_TEST_EPS,
					 final_epsilon=FINAL_EPSILON, epsilon_decay_steps=EPSILON_DECAY_STEPS,
					 force_test_mode=False, render=True):
		global epsilon
		# Record the number of times we do a training batch, take a step, and
		# the total_reward across all eps
		batch_presentations_count = total_steps = total_reward = avg_reward = 0

		for episode in range(num_episodes):
				# initialize task
				state = env.reset()
				if render: env.render()

				# Update epsilon once per episode - exp decaying
				epsilon -= (epsilon - final_epsilon) / epsilon_decay_steps

				# in test mode we set epsilon to 0
				test_mode = force_test_mode or \
										((episode % test_frequency) < num_test_eps and
												episode > num_test_eps
										)
				if test_mode: print("Test mode (epsilon set to 0.0)")

				ep_reward = 0
				for step in range(ep_max_steps):
						total_steps += 1

						# get an action and take a step in the environment
						action = get_action(state, state_in, q_values, epsilon, test_mode,
																action_dim)
						env_action = action
						# get_env_action(action)
						next_state, reward, done, _ = env.step(env_action)
						ep_reward += reward

						# display the updated environment
						if render: env.render()  # comment this line to possibly reduce training time

						# add the s,a,r,s' samples to the replay_buffer
						update_replay_buffer(replay_buffer, state, action, reward,
																 next_state, done, action_dim)
						state = next_state

						# perform a training step if the replay_buffer has a batch worth of samples
						if (len(replay_buffer) > BATCH_SIZE):
								do_train_step(replay_buffer, state_in, action_in, target_in,
															q_values, q_selected_action, loss, optimise_step,
															train_loss_summary_op, batch_presentations_count)
								batch_presentations_count += 1

						if done:
								if (not test_mode) and 200 == ep_reward:
										force_test_mode = True
								break

				total_reward += ep_reward
				if episode == 0:
						avg_reward = total_reward
				else:
						avg_reward = avg_reward * 0.9 + ep_reward * 0.1

				test_or_train = "test" if test_mode else "train"
				print("end {0} episode {1}, ep reward: {2}, ave reward: {3}, \
						Batch presentations: {4}, epsilon: {5}".format(
						test_or_train, episode, ep_reward, avg_reward,
						batch_presentations_count, epsilon
				))


def setup():
		default_env_name = 'CartPole-v0'
		# default_env_name = 'MountainCar-v0'
		# default_env_name = 'Pendulum-v0'
		# if env_name provided as cmd line arg, then use that
		env_name = sys.argv[1] if len(sys.argv) > 1 else default_env_name
		env = gym.make(env_name)

		# env = gym.wrappers.Monitor(env, './outputs/experiment-' + 
		#                           datetime.datetime.now().strftime("%Y%m%d-%H%M%S"), write_upon_reset = True, force=True)

		state_dim, action_dim = init(env, env_name)
		network_vars = get_network(state_dim, action_dim)
		init_session()
		return env, state_dim, action_dim, network_vars


def main():
		env, state_dim, action_dim, network_vars = setup()
		qtrain(env, state_dim, action_dim, *network_vars, render=True)


if __name__ == "__main__":
		main()

