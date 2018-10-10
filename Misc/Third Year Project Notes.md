Third Year Project Notes
THXX: Autodidactic Draughts AI

Maximum students: -

Subject: Machine Learning

Project can be for: -

Draughts AI players are currently designed to at a fixed ability. While it has produced very competitive and intelligent players, they require tweaks in order to improve its performance. By using Neural Networks and Genetic Algorithms, this issue could possibly be solved by by creating a player that can grow in ability over time.

Requirements: None.

Keywords: AI, Machine Learning, Genetic Algorithms

Academic background reading can be found in +Research Papers.

Anticipated Outcome: Build an autodidactic Checkers AI that can play to an advanced level.



----------



----------


Heuristics

Base Features

1. Number of Pawns
2. Number of Kings
3. Number of Safe Pawns
4. Number of Safe Kings
5. Number of Moveable Pawns (Not Capture)
6. Number of Moveable Kings (Not Capture)
7. Aggregated distance from pawns to promotion line
8. Number of un-occupied fields on promotion line

Layout Features

9. Number of Defender Pieces (i.e Number of Pawns and Kings situated in the two lowermost rows)
10. Number of attacking pawns (i.e Positioned in the three topmost rows)
11. Number of centrally positioned pawns
12. Number of centrally positioned kings
13. Number of pawns on the main diagonal
14. Number of kings on the main diagonal
15. Number of pawns on the double diagonal
16. Number of kings on the double diagonal
17. Number of loner pawns (a loner piece was defined as the one not adjacent to any other piece)
18. Number of loner kings
19. Number of Holes (empty squares adjacent to at least three pieces of the same color.)

Pattern Features
The Board will be represented by the white players point of view. At most one instance of each pattern can exist for each player in a given board position. These values can only take boolean values

20. White Pawns on the square 27,31,32 (The Triangle)
21. White Pawns on the square 26,30,31 (The Oreo)
22. White Pawns on the square 30,32 (The Bridge)
23. White Pawn on 32, Black Pawn on 28 (The Dog)
24. White Piece on 29 (Pawn in the Corner)
25. White King on 4 (King in the Corner)

$$LinearCombination = a_1 * param_1 + param_2 + param_3 + ... + param_j$$


![White Pieces are at the bottom.](https://d2mxuefqeaa7sj.cloudfront.net/s_1158E89CD15795C8792264A45BB012878E7EF1AD4FAA69EFCF0655962F46DE5F_1496676322351_board.png)


Using genetic algorithms and simulated annealing together?
https://rmflight.github.io/posts/2014/03/saga_blogPost.html
