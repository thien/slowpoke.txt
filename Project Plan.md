<!-- No more than two pages -->
# Project Plan

Thien Nguyen,
Supervisor: Stefan Dantchev
Department of Computer Science, Durham University

## Description
Machine Learning techniques have been applied to the game of Draughts since their beginnings. The current state of the art deep learning techniques have been successfully applied by Fogel and Chellapilla for their Blondie24 program, playing checkers to an ELO level of 1901. This involved using a single feed-forward CNN as an evaluator of the given state of the board. However, some would say that by using a 

## Preliminary Preparation

- Preparation consists of a variety of tasks:
Background reading around the topic will include the state of the art forms of CNN's, the game of draughts, genetic algorithms, and zero sum games in general. 
- In terms of the game of draughts, study the different heuristics that stronger players implement.
- Pre-emptive implementations of the Draught's game interface will need to be implemented. 

## Research Question
Can we produce a similarly performant draughts evaluator by using seperate classifiers for the different stages of the game?

## Deliverables

### Minimum
- Implement a CNN using TensorFlow
- Implement a Checkers Game Interface
- Implement a genetic algorithm with an evaluation function that consists of a round robin tournament against the population of CNN Evaluators.
- The resulting AI can be played against a human, or a random oppoment.

### Intermediate
- The resulting AI can However, this contradicts with how players singularly implement be played online.
- The AI can be 

### Advanced
- The resulting AI can play within the same ELO as previously 

## Milestones

