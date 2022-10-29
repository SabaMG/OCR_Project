# EPITA - S3 - OCR

OCR *(Optical Character Recognition)* software solving a sudoku grid.

## Table of contents

- [Grid Recognition](#GridRecognition)
- [Sudoku Solver](#SudokuSolver)
- [XOR Neural Network](#XORNeuralNetwork)

## Grid Recognition

Program able to recognize the sudoku grid and cut its 81 boxes into 81 sub-pictures in order for the neural network to process digit per digit.

It requests 2 pictures to process: the original picture of the sudoku and the same picture but with the sobel/Canny edge filter applied.

The sub-pictures of the grid are stored in the `/boxes` directory. They are nammed after `box_ij.jpg` with `i` and `j` their coordinates (the upper left box of the sudoku has the 0,0 coordinates).


### Installation

1. Compile the project in `/GridReco` with the command `make`.

2. Run the program with `./main linesEdges.jpg lines.jpg`.


### Usage

* Global usage: `./main linesEdges.jpg lines.jpg`.

### Uninstallation

Clean all object and executable files with `make clean`.


## Sudoku Solver
The sudoku solver uses a backtracking algorithm to test all possible possibilities 

until the solution is found. 

It takes as input a file containing an incomplete sudoku grid, 

and returns a new ".result" file containing the resolved grid.


### Instalation

1. Compile the project in `./solver` with command `make`.
2. Run the program with `./solver` 

### Usage
* Global usage: `./solver grid_00`
  - Here `grid_00` is the file containing the incomplete sudoku grid.
  - Return `grid_00.result` the file containing the solved grid.

### Uninstallation

Clean all object and executable files and .result file with `make clean`.

## XOR Neural Network

Proof of concept for the main neural network of the optical character recognition program. 

Mini neural network that is able to learn the function or exclusive (xor).

Weights and biases of the neurons can be exported after the training phase and loaded from a file.

Two modes are available: the normal mode to use the neural network and the training mode to train the network.


### Installation

1. Compile the project in `/xor` with the command `make`.

2. Run the program with `./main`.


### Usage

* Global usage: `./main [OPTIONS...] [PARAMETERS...]`.

* Normal mode: `./main -l [input file]`.

* Training mode:
	- Run with default parameters: epochs = 10000, learning rate = 0.1: `./main -t`.
	- Save weights and biases: `./main -t -s [output file]`.
	- Specify the number of epoch:	`./main -t -e [number]`.
	- Specify the learning rate:	`./main -t -r [number]`.
	- Verbose mode (print the results of each epoch) `./main -t -v`.

### Uninstallation

Clean all object and executable files with `make clean`.
