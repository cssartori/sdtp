#!/bin/bash

timestamp() {
  date +"%Y-%m-%d :: %H:%M:%S"
}

## Script to aggregate all data in summary files for easier post-processing and graph generation


echo "Starting aggregation at "$(timestamp)

genSummaryFiles(){
    base=$1
    mode=$2

    path="$base/$mode/"

    addp=""

    if [ $(basename $base) == "GRID" ] ; then
        addp="--grid"
    fi

    ## full
    python grouper.py -d "$path" -o "$base/sum_$mode.txt" -t "$mode" $addp -m time

    ## feasible
    python grouper.py -d "$path/feasible/" -o "$base/sum_feas_$mode.txt" -t "$mode" $addp -m time
    ## infeasible
    python grouper.py -d "$path/infeasible/" -o "$base/sum_infeas_$mode.txt" -t "$mode" $addp -m time
}

genGroupFiles(){
    base=$1

    genSummaryFiles "$base" "nodes"
    genSummaryFiles "$base" "density"
    genSummaryFiles "$base" "num-disjunctions"
    genSummaryFiles "$base" "var-disjunctions"
}

DATADIR="../data/time"

genGroupFiles "$DATADIR/RAND"
genGroupFiles "$DATADIR/GRID"
genGroupFiles "$DATADIR/SEQ"
genGroupFiles "$DATADIR/LATE"

python grouper.py -d "$DATADIR/VRP/" -o "$DATADIR/VRP/sum_vrp.txt" -t "nodes" -m time
python grouper.py -d "$DATADIR/VRP/feasible/" -o "$DATADIR/VRP/sum_feas_vrp.txt" -t "nodes" -m time
python grouper.py -d "$DATADIR/VRP/infeasible/" -o "$DATADIR/VRP/sum_infeas_vrp.txt" -t "nodes" -m time

python grouper.py -d "$DATADIR/NEGCYCLE/NC02" -o "$DATADIR/NEGCYCLE/sum_nc02.txt" -t "nodes" -m time
python grouper.py -d "$DATADIR/NEGCYCLE/NC03" -o "$DATADIR/NEGCYCLE/sum_nc03.txt" -t "nodes" -m time
python grouper.py -d "$DATADIR/NEGCYCLE/NC04" -o "$DATADIR/NEGCYCLE/sum_nc04.txt" -t "nodes" -m time
python grouper.py -d "$DATADIR/NEGCYCLE/NC05" -o "$DATADIR/NEGCYCLE/sum_nc05.txt" -t "nodes" -m time

python grouper.py -d "$DATADIR/VL/" -o "$DATADIR/VL/sum_vl.txt" -t "nodes" -m time

python grouper.py -d "../data/cache/VL/" -o "../data/cache/sum_cache.txt" -t "nodes" -m cache

echo "Ending aggregation at "$(timestamp)
echo "DONE"
