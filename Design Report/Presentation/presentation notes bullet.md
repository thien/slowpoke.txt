 # Introduce

- introduce yourself

Hi, I'm _ and i'm going to be talking about using NN's and GA's to play Draughts.

# Outlines

These is how my presentation will flow, i'll talk about the problem I'd like to tackle; and hopefully this will progress towards a solution to the problem.

# Description

- The issue with checkers is that:
- good ai exists already
- serious research doesn't exist in the game anymore
- been solved

# Motivation

based on three things:
- I liked AI search last year
- Also keen on machine learning 
- I also like board games

# Related Work

Of course, this is not a wildly new topic; people have been exercising this problem since the advent of machine learning (starting with Samuel in the late fifties.)

# Approach

If it all goes right, this would be the plan that will be used to create an AI playing Agent.
- Find a way to evaluate a current position of a board
- make it choose a move
- improve the evaluator in part 1

# Evaluating Checkerboards

So how will we go about evaluating checkerboards?

- use neural networks
- metaphor of the human brain
- vertices are neurons
- edges are synapses
- values of a vertex propagate to other vertices until they reach an output layer
- used to model fairly sophisticated relationships between input and output without actually knowing why

- output will give us a value from -1 to 1 where -1 represents the probability of one side winning, and vice versa.

# Activation function

Our activation function of choice is a tanh function; 
- symmetrical around the origin
- perfect at mapping the zero sumness of the game
- if one side is winning to some extent, the other side is losing by the same extent.

# Choosing Moves

So at a given state, a play might have a selection of moves to make. How do we choose each one? how do we evaluate each one?

# Monte Carlo Tree Search

Due to the constraints of presentation time I'll have to grossly simplify the monte carlo tree search algorithm.

MCTS runs in four stages, which is explained far batter in this diagram that i've shamelessly poached from a paper talking about the concept.

Traditionally, MCTS would simulate random moves to the end of a game, but I have chosen to cap it to some depth and we then simulate the state of the game using our neural network.

# Now what

Since we can now evaluate a move, and choose the best move, we essentially have ourselves some 'AI'. However, the chances of it playing very well at this stage is slim. Here we introduce our genetic algorithm.

I'll assume that everyone remember or knows what genetic algorithms are;

Our population would consist of different weights and biases which would be used to evaluate the neural network.

# Tournament

Our tournament rounds come as such:
- generate agents as just described
- make them play each other. Each agent plays 5 times as black against random agents in the pool.
- choose the best ones and keep them (which would be the top 5)
- the top 5 will produce some offspring to play for the next round
- Our offsprings are produced by crossover and mutation methods.
- the rest are killed
- rinse and repeat for 200 rounds.


# Crossover

Crossover is a process in genetic algorithms where the traits that build two parent agents are combined to create children. In our scenario we use the weights and biases of the parent’s neural networks.

A random hidden node in the network is chosen, and we look at the dominant inputs to that node. We swap the weights and biases that contribute to those values and swap them.

Two offsprings would be created from a pair of parents, with each offspring being the reciprocal crossover of each other. 

# Mutations

Mutations are another way to simulate 'learning' in genetic algorithms. For our situation, we manipulate each weight and bias in new agents in a fairly subtle way. We simply generate a random value to differ the current weight and bias by.

# Evaluation

So that is essentially how the AI will operate. We create a bot that can evaluate a move using neural networks, we choose good moves using our monte carlo tree search, and we train the neural network using genetic algorithms.

How would I evaluate it?


- See that it can beat a bot who plays randomly
- We'll need to make the end bot play against bots from prior stages to determine whether it is learning
- We'll need to evaluate the end bot against generic game playing algorithms like a vanilla MCTS and minimax
- We can also pit the bot against other human players on some onlline checkers site but there'll need to be more research on what website to use.

# Current Progress

It works, but no hard measurements would be found. It's been found to play against random bots but that's not very hard to beat.

Most of the time allocated to the project would be spent on the list of remaining work:

# Remaining Work

- Priority 1 revolves around squashing bugs that could provide the bot with a false positive
- Experiment with the parameters for crossover/mutation algorithms
- Train the agent again (and find a computer willing to run it)
- Measure the bot's performance.

# FIN!