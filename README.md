ILP-gatech
==========

A project to compute the Instruction-level parallelism in alpha instruction set. Project done in simplescsalar

Programming Assignment 1 Report (ECE 6100)
Shubhojit Chattopadhyay

email: ssc3@gatech.edu

Study of limits of Instruction Level parallelism

The aim of this assignment was to modify an execution based process simulator at the functional level and to study the limits caused by dependencies on parallelism. The simulator used was simplescalar-3.0 (sim-safe alpha version). The simulation was run on SPEC2000 benchmark programs. For simulation purposes, all instructions were assumed to have unit latencies. 

Following the implementation done in [1] and [2], the values of RAW, WAR, WAW, MRAW, MWAR and MWAW were calculated and issue cycle for each instruction was found out. Tracking of Registers and their contents was made easy by the use of macro enabled definition of each instruction of the instruction set which was easily stored in variables. For tracking memory dependencies, use of a smart data structure was necessary. There are many ways of implementing the tracking data structure, some of them include linked lists, binary search trees, skip lists, hash tables etc. Although hash tables seem to be the best technique for huge memory blocks (2^32 addresses in this case), binary search and insert trees was used in the assignment. The obvious drawback observed here was the speed of simulations decreased, typically due to the constant search and insert procedure employed by binary tree, with some simulations running for up to 7 hours. To increase the speed of execution, a balanced search tree was later tried out but couldn't be implemented by deadline.

Due to the high execution times, the benchmark programs for machine model 2 were run for both 50 Million and 1 Billion instruction cycles whereas in machine model 1 and 3, they were run for only 50 Million instructions (except for eon, ammp and mgrid which were also run for 1 Billion instructions). The resultant output is hence not perfectly accurate and the simulation possibly does not reflect the extent of parallelism achievable with 1 Billion instructions. However, the results are still closely consistent with observations made in [1] and [2].

Three different machine models were studied and the observations of the simulation are as follows:

1) Machine Model 1: The ILP values for the benchmark programs range from 1.5 to 4.5, averaging at 2.4. This is consistent with the observations made in [1] and [2] since register, memory as well as control dependencies are all to be considered in this model. (Figure 1).
2) Machine Model 2: Since control dependencies are not a factor any longer in this machine model, the ILP increases by a factor of 1.8 to 2.0 over machine model 1 calculations, averaging at 4.56. (Figure 1)
3) Machine Model 3: It was observed that removal of register dependencies increases the ILP by almost an order or more. However, there is still more scope for increase in ILP with increase in no. of instructions (i.e. with simulation of 1 Billion instead of 50 million instructions). For a larger window size, the ILP is expected to increase, especially for fp operation intensive programs. Average value of ILP here came around  20.15 (Figure 1)

An interesting observation was that for machine model 2, the ILP was approximately the same for both 50 Million Cycles and 1 Billion cycles. However, in machine model 3, it was observed that as the size of the program increases, the ILP also increases i.e. the ILP due to absence of register dependencies increases with the length of the window size.  This was verified only for 3 benchmark programs namely eon, ammp and mgrid. This confirms the observations made in [1] that most register dependency parallelism occurs over great distances.

A general observation made was that increase in ILP in programs containing more floating point  operations due to register renaming are typically greater than that in Integer operation based programs.(mgrid, fma and swim in this experiment particularly show a high increase of ILP in machine model 3) (Figure 3)

In conclusion, the study of limits of parallelism led to similar observations as obtained in [1] and [2]. It was observed that an unbalanced binary tree is not an efficient way of tracking memory dependencies. Use of a balanced tree or some other data structure like hash tables is recommended. A comparative study of all three machine models leads to the conclusion that for a large number of instructions, a very high level of parallelism can be exploited, at least theoretically, by removal of dependencies using perfect branch prediction and perfect register renaming. This suggests that achieving close to perfect branch prediction and close to perfect register renaming, in the practical world,  can considerably increase ILP.


References:

[1] M. Postiff, D. Greene, G. Tyson and T. Mudge, The Limits of Instruction Level Parallelism in SPEC 95 Applications, INTERACT-3, 1999

[2] Hsien-Hsin Lee, Youfeng Wu and Gary Tyson, Quantifying Instruction Level Parallelism Limits on an EPIC Architecture, ISPASS, 2000

[3] David Wall, WRL Research report, Limits of Instruction Level Parallelism, DEC Tech Report, 1993

[4] J. Hennessey and D. Patterson, Computer Architecture: A Quantitative Approach, The 4th edition, Morgan Kaufmann. 2006. ISBN 0-12-370490-1.

