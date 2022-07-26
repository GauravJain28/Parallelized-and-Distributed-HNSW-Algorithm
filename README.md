# Parallelized-and-Distributed-HNSW-Algorithm
This repo contains the implementation of Parallelized and Distributed HNSW based prediction algorithm using OpenMP and OpenMPI. 

Solution to Assignment 3 of the course COL380- Introduction to Parallel and Distributed Programming offered in Second (Holi) Semester 2021-22.

## HNSW
HNSW stands for Hierarchical Navigable Small World graphs. HNSW is designed to reduce the prediction complexity of One-vs-All (OVA) method from O(L) to O(logL). This reduction in complexity is achieved by learning a hierarchical graph structure. More details [here](./A3_PS.pdf).

## How to run the Code?
Clone the git repo and run the following commands:
```
cd Code
```
This command opens the Code folder.
```
./compile.sh
```
This command compiles all the required files.
```
./DataSetup.sh ./data/path/hnsw/given ./data/path/hnsw/converted
```
This command is used to convert raw pre-trained HNSW data.
```
./HNSWpred.sh ./data/path/hnsw/converted K users.bin user_prediction.txt
```
This command is used to predict top K relevant items in descending order and store them in user_prediction.txt file.

