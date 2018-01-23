# Introduce

- introduce yourself

# Outline

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

# Crossover
Two offsprings would be created from a pair of parents, with each offspring being the reciprocal crossover of each other. The weights of both parents (now each treated as a 1D array of coefficents), are divided contingent on the number of weights and biases for a given layer. Each layer should be treated separately to reduce the potential dependency on a purely randomly generated neural network. 

This will need more experimentation however. There are a lot of studies on different approaches to crossover mechanisms.

# Mutations

Weight and biases of an agent's neural network will increment by a random value that is created using the following formula, where $WeightP$ is the current weight, $K$ represents the number of weights and biases in the neural network, and $m$ representing a random floating point in the range of [-1,1]:

$$ WeightN = WeightP + \frac{m}{\sqrt{2 * \sqrt{K} }}$$

The weights, as explained earlier will have a soft maximum of [-1, 1]. This  would consequently mean that the mutation is not controlled, and dependent on the number of weights in the system; The more weights in the network implies a less significant mutation.