# Scripts

To reproduce the results in `sdtp/data/` one can run the scripts in this folder. First, all executables (`sdtp`, `stest` and `sperf`) should have been compiled and located in `sdtp/source/build`. 
If this is not the case, please consider following the steps described **here** to compile the source code.

In addition to the executables, one must ensure that the **instances** are all located in their respective folder. Note that due to space limitations, instances have not been made available in this repository but instead in a third-party one.

Once all executables and instances have been properly placed, one can run experiments by:
```
./experiments.sh
```

**Note**: in order to measure cache using `perf` one needs to be a superuser (`sudo`). Hence, if possible, it is advisable to run the above command after obtaining the superuser permissions. Alternatively, one can disable cache experiments by simply commenting the associated line in the script file.

After the experiments are done, the folder `sdtp/data/` should have been populated with files from the executions. Then, graphs can be generated for analysis using:
```
./analysis.sh
```

This script requires both Python 3.6+ and R to be installed. Moreover, for R one needs to have `ggplot2` and other libraries detailed inside the `plotter.R` script (a special font is also needed).


