# 44. Bundeswettbewerb Informatik
This repository contains my submissions for the second round of the **44. Bundeswettbewerb Informatik (BwInf)**. 

The first round was completed as a group project alongside an amazing team. Our Round 1 submission can be found in a separate repository: [derBohrman/BwInf (Round 1)](https://github.com/derBohrman/BwInf/tree/main/441). 

For the second round, I solved **Problem 2** and **Problem 3**. In addition to the standard tasks, I implemented an additional Bitmask solution for the third problem. All documentation is written in German, detailing my algorithmic ideas and implementation choices.

* [Original Problem Sheet (PDF)](https://bwinf.de/fileadmin/wettbewerbe/bundeswettbewerb/44/2_Runde/Aufgaben442.pdf)
* [Official Test Cases (.zip)](https://bwinf.de/fileadmin/wettbewerbe/bundeswettbewerb/44/2_Runde/44_2.zip)

---

## Problem 2: Gießroboter
### Problem
Given a specific set of trees, defined by their X- and Y-coordinates on a two dimensional plane, the task is to find the minimum number of robots required to water all trees. The robots are limited by the maximum length of their routes (they must return before running out of battery). A robot's powerstation can be placed anywhere on the two dimensional plane, including at tree coordinates. In addition to that, each powerstation can only belong to one robot, preventing shared powerstations.

### My Solution
The problem heavily resembles the **Capacitated Vehicle Routing Problem** ([CVRP](https://en.wikipedia.org/wiki/Vehicle_routing_problem)), where trees represent customers and robots represent trucks. The goal is to minimize the number of "trucks" required to supply all "customers", making this a NP-hard optimization problem.

The solution utilizes the **Adaptive Large Neighborhood Search** (ALNS) algorithm. While ALNS is typically used to solve classic VRP variants, modifying its heuristic behaviors and adding primary and secondary goals allows you to solve this specific optimization task. Detailed test cases and generated results are included in the documentation.

---

## Problem 3: Lieferkette
### Problem
Given a bidirectional weighted connected graph where nodes represent either starting, ending or manufacturing steps (with each group executing different production tasks), the task is to find a valid route within a specific distance constraint. This route must start at the initial node, pass through every manufacturing group in a strict, predefined sequence, and reach the ending node. Furthermore, the route must remain viable within given time constraints even if **at most one node becomes defective** and fails to perform its manufacturing step.
### My Solution
The problem is solved by combining **Dial's Algorithm** with **Dynamic Programming** (DP). By analyzing the problem constraints, you can find out that the maximum edge weight is 20 across all test cases. This allows the use of Dial's Algorithm, which runs in $O(E + V \cdot W)$ time (where $W$ is the maximum edge weight), offering a faster alternative to standard Dijkstra (which runs in $O((V + E) \log V)$ ).

The DP state logic works backwards from the target, by answering the following questions:
1. *How long is my shortest path to the end node from here?* (Solved for all nodes using Dial's algorithm).
2. *How long is the shortest path if the current node becomes defective?*
3. *Without knowing in advance which node will fail, what is the lowest worst-case path length?* (Calculated by combining questions 1 and 2)

Once this mini-max evaluation is completed for the starting node, a route with the best worst-case performance is guaranteed.

For more information, feel free to read through the documentation I have provided for the problem. Inside, you will be able to find mathematical proof about the strategy functioning as well as a detailed description regarding how to solve the problem and how I implemented the solution.

---

## Problem 3: "Lieferkette" but with Bitmask
### Modification
Modifying the original constraints to allow manufacturing steps to be executed in **any order** significantly expands the space of valid routes. This variation transforms the task into a NP-hard problem resembling the Traveling Salesman Problem (TSP), though validating any given route remains possible in polynomial time.
### My Solution
Because the sequential transitions are removed and new DP-transitions are possible, the solution requires a structural rework. The algorithm precalculates all-pairs shortest paths to generate a bidirectional weighted complete graph. The DP state space is expanded using a **Bitmask** to represent which manufacturing steps have already been completed and which are remaining. The mini-max question remains the same: calculating the best path for the worst-case scenario of a node failure. Evaluating this for the starting node with all bits set to completed gives the solution.

---

## Author & License

**Copyright (C) 2026 Tao Zheng**

*Some Rights Reserved.*

All source code and documentation in this repository are licensed under the [GPL 3.0](https://choosealicense.com/licenses/gpl-3.0/). 

**DISCLAIMER: PROVIDED WITHOUT ANY WARRANTY.**

**There is no official guarantee for the absolute correctness of these solutions.** For official solution hints and scoring guidelines, please visit https://bwinf.de/bundeswettbewerb/aufgabenarchiv/. 