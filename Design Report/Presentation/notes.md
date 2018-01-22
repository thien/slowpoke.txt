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

the vertices here represent the neurons of a human brain, and the edges represent synapses. The idea is that this could be used to calculate 

It's not very sexy in code.