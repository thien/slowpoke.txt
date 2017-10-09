Literature Survey

Evolutionary approach to the game of checkers
M. Kusiak, K. Waledzik, J. Mandziuk, Warsaw U. of Technology

https://www.dropbox.com/s/5wzlp61pqwt30mf/ICANNGA07-1.pdf?dl=0

This paper mentions 25 heuristics that that can be used together to create a linear combination. We could potentially use these heuristics in unison to create the evaluation function.

These heuristics can be grouped depending on the state of the game. For instance, some heuristics would be more useful during the early-mid game, whereas other heuristics would be only useful for the end game. The paper suggests that this performs the strongest out of their options.


> Not surprisingly, the best of all tested heuristics is E3Ph − a nonlinear 3phase heuristic. This result conﬁrms the common knowledge in game research that it is advantageous to divide the entire game into phases and develop separate heuristics for each part of the game. Specifically it was also observed that game phases need to partition board positions space into disjoint sets. Otherwise the genetic process may have difficulties in assigning coefficients for the features shared by two or more game phases. 

The population will consist of coefficients of these heuristics.


>  All coefficients of the heuristics were represented as a vector of real numbers whereby each number denoted a single gene (one coefficient). In case of nonlinear heuristics, the conditions that nonlinear components consisted of were not modified by the process of evolution.

Selection is managed by tournaments. winners of two tournaments are coupled and crossbred.

Crossover is managed by a random partition of its coefficients, with random assignments from each parent. The offspring would replace the weakest specimen in the population. 

Mutation occurs here are simple math operations on the coefficients.

The Fitness Function becomes a little tricky here.


> One of the diﬃculties encountered while designing a genetic algorithm was deﬁning ﬁtness function for the heuristics. In order to solve this problem the general idea presented in [9] was followed. The game was partitioned into several disjoint stages according to the number of moves already performed. During the ﬁrst phase of the algorithm a heuristic that would be able to assess correctly situations close to the end of the game was to be obtained. In order to achieve this, alpha-beta algorithm with no heuristic was used to assess a number of randomly generated positions close to the leaves of the game tree. All positions beyond the depth of alpha-beta algorithm were considered a draw. 
> 
> Subsequently, each specimen assessed the same positions and its ﬁtness was calculated according to the formula $$n/\sum (h_i - a_i )^2$$, where$$$$$$n$$ denotes the number of test situations, $$h_i-$$ assessment of the $$i$$-th test situation by the heuristic specimen and $$a_i-$$ by the alpha-beta algorithm.
> 
> Once the initial stage had ended, worst ﬁtted fraction of the population was replaced by new random specimens. New board situations closer to the root of the game tree were generated and they were assessed by the alpha-beta algorithm with the ﬁttest specimen of the previous phase used as its heuristic function. The process continued until the root of the game tree was reached. In other words, the evaluation function of HG was evolved step-by-step starting from positions achieved far from the initial position and moving backwards.
> 
> In each phase a constant fraction of all the test boards came from the stage of the game closest to the beginning (i.e. the stage considered most recently). In the ﬁrst step positions obtained in between 82 and 86 moves were considered.
> 
> This interval was determined based on preliminary tests calculating the average number of moves necessary to ﬁnish a game performing random moves. The diﬀerence in depths between subsequent phases was set to 6. The number of positions considered in each phase ranged from 3,000 to 4,500.


----------

Apply genetic algorithm to the learning phase of a neural network
S. Perez, UC Irvine


https://www.dropbox.com/s/zz63cv82lkgr8ft/sperez1_GANN.pdf?dl=0


This paper discusses comparisons between the possible approaches that would improve the quality of weights on the neural network: Back Propagation and Genetic Algorithms. The comparisons would be measured using a Balance Scale.

The test data used was split into two sets. One set is used to feed into the neural network as training data, with its performance tested. The set is then used as test data for comparison.
Hidden layers are necessary for the comparison in order to make the comparison fair.


> As expected, the GA approach gives better results than the back-propagation method with almost all the iterations used (using 6% and 50% as mutation and crossover probab
> ilities).  Only when the number of iterations is very small, the back-propagation method gives better precision performance.

The paper suggests possible downfalls to a genetic algorithm approach.


> What would have happened if the margin of the weights was not known?  The search space for the genetic algorithm would be much larger that the one that we were supposed to search, and the solution not only will converge slowly, but also with less precision.


Evolving Neural Networks to Play Checkers without Relying on Expert Knowledge
Kumar Chellapilla, UC San Diego; David B. Fogel, Natural Selection, Inc.


https://www.dropbox.com/s/4gkzt28emsipprd/TNNKChellapillaAndDBFogelText.pdf?dl=0


Evolutionary algorithms in theory and practice
Fogel, David B. • 1997


https://www.dropbox.com/s/h5t5xcr8a9ntug9/Fogel-1997-Complexity.pdf?dl=0



Evolving a checkers player without relying on human experience
David B. Fogel
intelligence, 01 June 2000, Vol.11(2), pp.20-27


https://www.dropbox.com/s/914bao2e3uq4qaj/p20-fogel.pdf?dl=0



https://www.dropbox.com/s/e6qa7gg7ula40vv/1703.01513.pdf?dl=0


Genetic CNN
Lingxi Xie, Alan Yuille
Center for Imaging Science, The Johns Hopkins University, Baltimore, MD, USA


https://www.dropbox.com/s/cadxzt4f35c0snn/is%20good%20defense%20the%20best%20offence.pdf?dl=0


Is A Good Offensive Always the Best Defense?
J. QUETZALCOATL TOLEDO-MARIN, ROGELIO DIAZ-MENDEZ, MARCELO DEL CASTILLO MUSSOT, Universidad Nacional Auto ́noma de M ́exico,

https://rmflight.github.io/posts/2014/03/saga_blogPost.html
