# Literature Survey

### Title: Using Genetic Algorithms to evolve a Neural Network based Draughts Player
### Student: Thien Nguyen
### Academic Advisor: Stefan Dantchev
### Durham University

# Background

> Description: Draughts AI players are currently designed to play at a fixed ability. While it has produced very competitive and intelligent players, they require tweaks in order to improve its performance. By combining Neural Networks and Genetic Algorithms, this issue could possibly be solved by creating a player that can grow in ability over time.

## Environment
- 

## Themes
- Machine Learning
- Artificial Neural Networks
- Evolutionary Models
- Genetic Algorithms
- Draughts (Checkers)

## Definition of Terms

**Genetic Algorithm** - An evolutionary method that solves optimisation problems. This is based on Darwin's theorem of perpetually evolving populations of solutions.

**Neural Network** - A computational model based on the operations of "interconnected processing elements, which process information by their dynamic state response to external inputs." [1]

**Draughts** - 

For the sake of clarity these rules will be enforced:
- Take Piece.
- Forced Promotion

---

Pages 2–4
# Important issues of identified themes from results of reading.

**Evolutionary approach to the game of checkers**
_M. Kusiak, K. Waledzik, J. Mandziuk, Warsaw U. of Technology_

This paper mentions 25 heuristics that that can be used together to create a linear combination. We could potentially use these heuristics in unison to create the evaluation function.

These heuristics can be grouped depending on the state of the game. For instance, some heuristics would be more useful during the early-mid game, whereas other heuristics would be only useful for the end game. The paper suggests that this performs the strongest out of their options.

> Not surprisingly, the best of all tested heuristics is E3Ph − a nonlinear 3phase heuristic. This result conﬁrms the common knowledge in game research thatit is advantageous to divide the entire game into phases anddevelopseparate heuristics for each part of the game. Speciﬁcally it was also observed that game phases need to partition board positions space into disjoint sets. Otherwise the genetic process may have diﬃculties in assigning coeﬃcients for the features shared by two or more game phases. 

The population will consist of coefficients of these heuristics.

>  All coeﬃcients of the heuristics were represented as a vector of real numbers whereby each number denoted a single gene (one coeﬃcient). In case of nonlinear heuristics, the conditions that nonlinear components consisted of were not modiﬁed by the process of evolution.

Selection is managed by tournaments. winners of two tournaments are coupled and crossbred.

Crossover is managed by a random partition of its coefficients, with random assignments from each parent. The offspring would replace the weakest specimen in the population. 

Mutation occurs here are simple math operations on the coefficients.

The Fitness Function becomes a little tricky here.


> One of the diﬃculties encountered while designing a genetic algorithm was deﬁning ﬁtness function for the heuristics. In order to solve this problem the general idea presented in [9] was followed. The game was partitioned into several disjoint stages according to the number of moves already performed. During the ﬁrst phase of the algorithm a heuristic that would be able to assess correctly situations close to the end of the game was to be obtained. In order to achieve this, alpha-beta algorithm with no heuristic was used to assess a number of randomly generated positions close to the leaves of the game tree. All positions beyond the depth of alpha-beta algorithm were considered a draw. 
> 
> Subsequently, each specimen assessed the same positions and its ﬁtness was calculated according to the formulae $$n/\sum (h_i - a_i )^2$$, where$$$$$$n$$ denotes the number of test situations, $$h_i-$$ assessment of the $$i$$-th test situation by the heuristic specimen and $$a_i-$$ by the alpha-beta algorithm.
> 
> Once the initial stage had ended, worst ﬁtted fraction of the population was replaced by new random specimens. New board situations closer to the root of the game tree were generated and they were assessed by the alpha-beta algorithm with the ﬁttest specimen of the previous phase used as its heuristic function. The process continued until the root of the game tree was reached. In other words, the evaluation function of HG was evolved step-by-step starting from positions achieved far from the initial position and moving backwards.
> 
> In each phase a constant fraction of all the test boards came from the stage of the game closest to the beginning (i.e. the stage considered most recently). In the ﬁrst step positions obtained in between 82 and 86 moves were considered.
> 
> This interval was determined based on preliminary tests calculating the average number of moves necessary to ﬁnish a game performing random moves. The diﬀerence in depths between subsequent phases was set to 6. The number of positions considered in each phase ranged from 3,000 to 4,500.

**Apply genetic algorithm to the learning phase of a neural network**
_S. Perez, UC Irvine_

This paper discusses comparisons between the possible approaches that would improve the quality of weights on the neural network: Back Propagation and Genetic Algorithms. The comparisons would be measured using a Balance Scale.

The test data used was split into two sets. One set is used to feed into the neural network as training data, with its performance tested. The set is then used as test data for comparison.
Hidden layers are necessary for the comparison in order to make the comparison fair.


> As expected, the GA approach gives better results than the back-propagation method with almost all the iterations used (using 6% and 50% as mutation and crossover probab
> ilities).  Only when the number of iterations is very small, the back-propagation method gives better precision performance.

The paper suggests possible downfalls to a genetic algorithm approach.

> What would have happened if the margin of the weights was not known?  The search space for the genetic algorithm would be much larger that the one that we were supposed to search, and the solution not only will converge slowly, but also with less precision.

**Evolving Neural Networks to Play Checkers without Relying on Expert Knowledge**
_Kumar Chellapilla, UC San Diego; David B. Fogel, Natural Selection, Inc._

Fogel's paper describes a method that has solved what we are attempting to do. 
- Try a different approach to implementing it.

The paper describes a perspective on an approach that can be used to solve the problem described. It consists of a min-max algorithm with alpha-beta pruning. The 

It uses an 'evolutionary algorithm' (which is assumed to be a genetic algorithm), with the neural network as the population. The individual weights of the neural networks were measured against each other using a tournament, where each neural network plays against each other in a simulated game of checkers. The best neural networks stay on.

They used 250 generations, of 150 games per generation, with a population of 15 neural networks.


The bot 

# Proposed Direction of the Project (Page 5)

The main concern is to try and differ the project away from the method described by Chellapilla & Fogel, whilst attempting to achieve the same objective. 

The approach that would be taken consists of again,

the genetic algorithm 

While it is our primary focus to build an effective 

This is determined by the performance of these parts of the system:

- Genetic Crossover Algorithm
- Genetic Mutation Algorithm
- Effectiveness of the Neural Network

Fortunately 

# Conclusion (Page 6)

# References

[1] "Neural Network Primer: Part I" by Maureen Caudill, AI Expert, Feb. 1989













