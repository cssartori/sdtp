#!/bin/bash

timestamp() {
  date +"%Y-%m-%d :: %H:%M:%S"
}

## Scripts to execute experiment runs.

REP=10
THD=4
EXE=""
VLPAR=""

mode=""
if [ "$1" == "" ] ; then
	echo "Missing mode parameter."
	exit 0
else
	mode=$1
    echo $mode
    if [[ "$1" == "time" ]] ; then
        EXE="../build/stest"
        REP=10
        VLPAR="--num-repetitions 10"
    elif [[ "$1" == "cache" ]] ; then
        EXE="../build/sperf"
        REP=0
    else
	    echo "Unknown mode $1."
        exit 0
    fi
fi

base_path=""
if [ "$2" == "" ] ; then
	echo "Missing base path for results."
	exit 0
else
	base_path=$2
fi

BASE="$base_path/$mode"
mkdir $BASE

echo "Starting $mode executions at "$(timestamp)
echo "Using $THD threads"

rungroup(){
    g=$1
    mkdir "$BASE/$g"

    runbatch $g "nodes" "$BASE/$g"
    if [[ "$mode" == "time" ]] ; then
        runbatch $g "density" "$BASE/$g"
        runbatch $g "var-disjunctions" "$BASE/$g"
        runbatch $g "num-disjunctions" "$BASE/$g"
    fi
}

runbatch(){
    g=$1
    b=$2
    d=$3

    mkdir "$d/$b"
    mkdir "$d/$b/feasible"
    mkdir "$d/$b/infeasible"

    ./runner.sh "../instances/$g/$b/feasible" "$d/$b/feasible" $THD $EXE $REP
    ./runner.sh "../instances/$g/$b/infeasible" "$d/$b/infeasible" $THD $EXE $REP
}

## ==================================================================
## =============================== SP ===============================
## ==================================================================

rungroup "RAND"
rungroup "GRID"
rungroup "SEQ"
rungroup "LATE"

## ==================================================================
## ============================== VRP ===============================
## ==================================================================

mkdir "$BASE/VRP"

mkdir "$BASE/VRP/feasible"
./runner.sh "../instances/VRP/feasible/" "$BASE/VRP/feasible" $THD $EXE $REP
mkdir "$BASE/VRP/infeasible"
./runner.sh "../instances/VRP/infeasible/" "$BASE/VRP/infeasible" $THD $EXE $REP

## ==================================================================
## ============================== NC ================================
## ==================================================================

mkdir "$BASE/NEGCYCLE"

mkdir "$BASE/NEGCYCLE/NC02"
./runner.sh "../instances/NEGCYCLE/NC02/" "$BASE/NEGCYCLE/NC02" $THD $EXE $REP
mkdir "$BASE/NEGCYCLE/NC03"
./runner.sh "../instances/NEGCYCLE/NC03/" "$BASE/NEGCYCLE/NC03" $THD $EXE $REP
mkdir "$BASE/NEGCYCLE/NC04"
./runner.sh "../instances/NEGCYCLE/NC04/" "$BASE/NEGCYCLE/NC04" $THD $EXE $REP
mkdir "$BASE/NEGCYCLE/NC05"
./runner.sh "../instances/NEGCYCLE/NC05/" "$BASE/NEGCYCLE/NC05" $THD $EXE $REP

## ==================================================================
## ============================== VL ================================
## ==================================================================

mkdir "$BASE/VL"
$EXE -a "{BFDC,RULT,CRA}" --log "$BASE"/VL/vl-1.json -i ../instances/VL/vl-1.json --verbose --time-limit 360000000 $VLPAR
$EXE -a "{BFDC,RULT,CRA}" --log "$BASE"/VL/vl-2.json -i ../instances/VL/vl-2.json --verbose --time-limit 360000000 $VLPAR
$EXE -a "{BFDC,RULT,CRA}" --log "$BASE"/VL/vl-3.json -i ../instances/VL/vl-3.json --verbose --time-limit 360000000 $VLPAR
$EXE -a "{BFDC,RULT,CRA}" --log "$BASE"/VL/vl-4.json -i ../instances/VL/vl-4.json --verbose --time-limit 360000000 $VLPAR
$EXE -a "{BFDC,RULT,CRA}" --log "$BASE"/VL/vl-5.json -i ../instances/VL/vl-5.json --verbose --time-limit 360000000 $VLPAR

echo "Ending $mode executions at "$(timestamp)
echo "DONE -> $mode"
