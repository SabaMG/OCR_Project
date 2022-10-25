# xor

Proof of concept for the main neural network of the optical character recognition program. 

Mini neural network that is able to learn the function or exclusive (xor).

Weights and biases of the neurons can be exported after the training phase and loaded from a file.

Two modes are available: the normal mode to use the neural network and the training mode to train the network.

## Table of contents

- [Installation](#Installation)
- [Usage](#Usage)
- [Uninstallation](#Uninstallation)

## Installation

1. Compile the project in `/xor` with the command `make`.

2. Run the program with `./main`.


## Usage

* Global usage: `./main [OPTIONS...] [PARAMETERS...]`.

* Normal mode: `./main -l [input file]`.

* Training mode:
	- Run with default parameters: epochs = 10000, learning rate = 0.1: `./main -t`.
	- Save weights and biases: `./main -t -s [output file]`.
	- Specify the number of epoch:	`./main -t -e [number]`.
	- Specify the learning rate:	`./main -t -r [number]`.
	- Verbose mode (print the results of each epoch) `./main -t -v`.

## Uninstallation

Clean all object and executable files with `make clean`.
