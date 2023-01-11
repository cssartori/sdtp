
The instances are available via Mendeley Data: https://dx.doi.org/10.17632/jcdmf2wsr3.1

To setup the instance files, it suffices to download them from the link above and unzip them into this folder.
The structure of directories should be:

~/sdtp/instances/
    |- GRID/
    |- LATE/
    |- NEGCYCLE/
    |- RAND/
    |- SEQ/
    |- VL/
    |- VRP/


Note: this structure is similar to the one used in the 'data/' folder.


==========================================================================================================================

Instances format: we have opted to use JSON as the formatting of the instances because it is an easy format, with tools
widely available for processing them.

The structure of instance files is the following.

{
    "name": <string with the instance's name>,
    "num-variables": <integer with the number of variables or time-points in the instance>,
    "num-constraints": <integer with the number of type 1 constraints in the instance>,
    "status": <string indicating whether this instance is feasible>,
    "comment": <string with a general comment about the generation of this instance>,
    "variables": [ <a list where each element represents a variable with the following structure>
                 {
                    "id": <integer with the unique id of this variable>,
                    "domain-size": <the number of intervals in this variable's domain>,
                    "domains": [ <a list with all domains for this variable, each element with the following structure>
                               {
                                "l": <integer representing the lower bound of this interval>,
                                "u": <integer representing the upper bound of this interval>
                                }
                                ]
                  }
                  ]
    "constraints": [ <a list of strings representing all type 1 constraints, each string with the following structure>
                   "x y w" < which is equivalent to the constraint 'x - y <= w' . All three values are integers. x and y are ids of variables while w is a constant. >
                   ]
}







