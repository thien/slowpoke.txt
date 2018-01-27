 # Introduce

- introduce yourself

# Outlines

These is how my presentation will flow, i'll talk about the problem I'd like to tackle; and hopefully this will progress towards a solution to the problem.

# Description

- When I was looking around, lots of competitive Draughts playing AI depended on a move database, which I'd argue is not very artificially intelligent at all.
- could be a variety of factors; checkers was a nice game to tackle; relatively not difficult at all (compared to Chess, and Go)
- In fact, it's been weakly solved (The game-theoretic value for the initial position has
been determined.) in 2007 Schaeffer and squad computed all moves and deduced that assuming perfect play from both sides creates a draw.
- not bothered with creating the best player, but to see whether it was possible to create a decent bot that didn't rely on it at all.

- So using some knowledge I've picked up from last year's submodule, I want to find out whether it's possible to use that to train a neural network. Could this be used to create a checkers playing AI?

# Motivation

- Lots of factors:
- AI Search had a competitive summative last year; everyone was racing to create AI's that could make good tours
- One thing that I remembered the most was genetic algorithms 
- Also keen on machine learning 
- I also like board games

It was a winning combination of factors that led me to choose this project.

# Related Work

Of course, this is not a wildly new topic; people have been exercising this problem since the advent of machine learning (starting with Samuel in the late fifties.)

# Approach

If it all goes right, this would be the plan that will be used to create an AI playing Agent.
- We first start by devising a method that can evaluate a state of a checkerboard.
- Then we devise a method that can choose the best move from a given position
- We then create a population of these checkerboard evaluators
- We make them play each other
- We choose the best ones and create mechanisms to deduce better evaluators from them

# Evaluating Checkerboards

So how will we go about evaluating checkerboards?
- there are a variety of methods, you could create your own heuristics that determine how good a position is, but that would be human intervention; and the AI would theoretically be as good as the heuristics you use.

A common approach is to use Neural networks. For those who don't know what neural networks are, it's a metaphor of how the human brain operates;

the vertices here represent the neurons of a human brain, and the edges represent synapses. The values of the vertices propagate to other edges, until they reach the output layer. The idea is that this could be used to model fairly sophisticated relationships between an input and output.

Ideally, we could use the positions of the pieces on the checkerboard as an input to a neural network, and hopefully it can evaluate the probability of one side winning.

# Activation function

Our activation function of choice is a tanh function; it's symmetrical around the origin and is perfect at mapping the zero-sumness of the game; where if one side is winning to some extent, the other side is losing by that same extent. 

# Choosing Moves

One issue with draughts is the potential to calculate 
There are about 5x10^20 possible moves that can be deduced from the beginning of the game.

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