# CacheAwareMatrixTransposition

## Table of Contents
- [Introduction](#introduction)
- [Build and Run](#build-and-run)
- [Possible Output](#possible-output)
- [How does this work](#how-does-this-work)

## Introduction
The purpose of this project is to implement and test a cache-aware matrix transposition algorithm and compare it's performance against a normal matrix transposition algorithm.

## Build and Run
To clone and run this project, you'll need [Git](https://git-scm.com) and [CMake](https://cmake.org/) installed on your computer. From your command line:

```bash
# Clone this repository
$ git clone https://github.com/Hayk10002/CacheAwareMatrixTransposition

# Go into the repository
$ cd CacheAwareMatrixTransposition

# Generate the build files
$ cmake -DCMAKE_BUILD_TYPE=Release -S . -B build

# Build the project
$ cmake --build build --config Release

# Then, run the executable generated in the `build` directory with the matrix size and iteration count to run the test.
$ your/path/to/exe/main.exe {matrix_size} {iteration_count}
# example - .../main.exe 2048 100
```

## Possible Output
(for matrix size 2048 and iteration count 100)

```
Process started on Core ID: 4
L1 Cache Size for Core 4: 48 KB
Cache-line size: 64
L1 Cache Associativity for Core 4: 12-way
Calculated optimal block size: 16
Transposition with    blocks: 1868ms
Transposition without blocks: 6275ms
```

## How does this work
The project measures the performance of a cache-aware merge sort algorithm by considering the L1 cache size and associativity. The algorithm calculates an optimal block size to do the transposition based on them, because stride access when the stride is power of two, can be really slow due to cache associativity.

Benchmarks show that doing the transposition with blocks can have very positive effect on performance for sufficiently large matrices if the matrix size is or is divisible on a large power of two, but for other cases, doing the transposition in blocks should not affect performance.
