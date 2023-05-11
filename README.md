# SDTP

This repository provides open-source `C++` implementations of mathematical models and algorithms to solve *Simple Disjunctive Temporal Problems* (SDTPs). The repository further includes instances and results comparing all of the implementations to establish the most suitable approach in a wide range of scenarios.

## Methods

There are nine methods for solving SDTPs as described in [Sartori et al., 2023]:
1. BFDC: Bellman-Ford with Domain Check [Sartori et al., 2023];
2. RULT: Reduced Upper-Lower Tightening [Sartori et al., 2023];
3. ILP: Integer Linear Programming model [Sartori et al., 2023];
4. CP: Constraint Programming model [Sartori et al., 2023];
5. SCP: Simplified Constraint Programming model [Sartori et al., 2023];
6. CRA: Comin-Rizzi Algorithm, described in [Comin and Rizzi, 2018];   
7. KAB: Kumar's Algorithm for zero-one Extended STPs with Bellman-Ford APSP, described in [Kumar, 2004];
8. KAJ: Kumar's Algorithm for zero-one Extended STPs with Johnson's APSP, described in [Kumar, 2004];
9. ULT: Upper-Lower Tightening, described in [Schwalb and Dechter, 1997].


## Compiling the source code

The CMakeLists file in the source folder can be used to compile the code and generate three executables. IBM's `CPLEX` is used to solve both the ILP, CP and SCP models and therefore is a dependency when compiling the source code. Additionally, the `boost` library is used as a dependency for the command line options and other structures. Below is a list of the dependencies:

  * CMake version 3.10 or later;
  * GNU C++ compiler compatible with C++17;
  * IBM's CPLEX version 12.9 (other versions may work but one should be aware of possible changes to the API);
  * Boost library version 1.65 or later.

Once all dependencies have been installed, it sufficies to run the following command, assuming one is inside `sdtp/source/` and in a Linux system:

```
mkdir build
cd build
cmake ..
make
```

## Executables

The compilation will generate three executables inside `sdtp/source/build/`: `sdtp`, `stest` and `sperf`. They are described below. For all of them, one may also run `./<executable-name> -h` to obtain a helper text with instructions.

### sdtp

The basic executable to run *one* algorithm to solve *one* instance. One can run:
```
./sdtp -a <ALGORITHM> -i <INSTANCE> [--log <LOG>] [--verbose] [--time-limit <TIME>]
```
where, 
  * `<ALGORITHM>` is one string from: "BFDC", "RULT", "ILP", "CP", "SCP", "CRA", "KRAB", "KRAJ", "ULT";
  * `<INSTANCE>` is the path to the instance file to be solved (in JSON format);
  * `<LOG>` is an optional parameter with the name (path) of a file where the result of the execution (including the final solution) will be written in JSON format;
  * `--verbose` is an optional parameter that indicates whether information concerning the execution should be printed to `stdout`;
  * `<TIME>` is an integer (long) representing the maximum execution time of the algorithm in microseconds.
  
----
### stest

The second executable is `stest` which can be used to run all algorithms and produce a full log file. The format of these logs is the same as those in the folder `sdtp/data/time/`. Use `stest` as follows:
```
./tester -i <INSTANCE> [-a <ALGS>] [--log <LOG>] [--verbose] [--time-limit <TIME>] [--num-repetitions <REP>]
```
where,
  * `<INSTANCE>` is the path to the instance file to be solved;
  * `<ALGS>` is a string defining the algorithms to be executed in this test. Default is all algorithms.
  * `<LOG>` is an optional parameter with the name (path) of a file where the result of the tests will be written in JSON format. The result of all algorithms in `<ALGS>` are stored in the log file but the final solutions are not (to reduce the output size). To obtain a solution for an instance please use the `sdtp` executable above;
  * `--verbose` is an optional parameter that indicates whether information concerning the execution should be printed to `stdout`;
  * `<TIME>` is an integer (long) representing the maximum execution time for *each* algorithm in `<ALGS>`;
  * `<REP>` is an integer representing the number of times each algorithm should be executed to solve `<INSTANCE>`. Each repetition will generate one time measurement which may be stored in the `<LOG>` file.

----
### sperf

The third and last executable is `sperf` which can be used to run the algorithms and produce a full log file measuring cache information using the `perf` tool from the Linux kernel. The structure of the output log files are the same as those in the folder `sdtp/data/cache/`. Use `sperf` via:
```
./sperf -i <INSTANCE> [-a <ALGS>] [--log <LOG>] [--verbose] [--time-limit <TIME>]
```
where,
  * `<INSTANCE>` is the path to the instance file to be solved;
  * `<ALGS>` is a string defining the algorithms to be executed in this test. Default is all algorithms.
  * `<LOG>` is an optional parameter with the name (path) of a file where the result of the tests will be written in JSON format. The result of all algorithms in `<ALGS>` are stored in the log file but the final solutions are not (to reduce the output size). To obtain a solution for an instance please use the `sdtp` executable above;
  * `--verbose` is an optional parameter that indicates whether information concerning the execution should be printed to `stdout`;
  * `<TIME>` is an integer (long) representing the maximum execution time for *each* algorithm in `<ALGS>`.


## Citation

When using the implementations and instances from this repository in (scienftific) publications, please consider citing

```
@article{sartori-smet-vanden-berghe-2023,
	title = "Models and algorithms for simple disjunctive temporal problems",
	author = "Carlo S. Sartori and Pieter Smet and Greet {Vanden Berghe}",
	journal = "Technical report at KU Leuven",
	year = "2023",
	url = \url{https://arxiv.org/abs/2302.02644}
}
```

## References

[Schwalb and Dechter, 1997]: Schwalb, E., and Dechter, R. (1997). Processing disjunctions in temporal constraint networks. *Artificial Intelligence*. Available at [10.1016/S0004-3702(97)00009-X](https://doi.org/10.1016/S0004-3702(97)00009-X).

[Kumar, 2004]: Kumar, T. S. (2004). A polynomial-time algorithm for simple temporal problems with piecewise constant domain preference functions. In *AAAI* (pp. 67-72).

[Comin and Rizzi, 2018]: Comin, C., and Rizzi, R. (2018). On Restricted Disjunctive Temporal Problems: Faster Algorithms and Tractability Frontier. In *25th International Symposium on Temporal Representation  and Reasoning (TIME 2018)*. Available at [10.4230/LIPIcs.TIME.2018.10](http://dx.doi.org/10.4230/LIPIcs.TIME.2018.10).

[Sartori et al., 2023]: Sartori, C. S., Smet, P., and Vanden Berghe, G. (2023). Models and algorithms for simple disjunctive temporal problems. *Technical report at KU Leuven*. Available at [arxiv.org/abs/2302.02644](https://arxiv.org/abs/2302.02644). 
