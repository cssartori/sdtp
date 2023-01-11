#!/bin/bash

timestamp() {
  date +"%Y-%m-%d :: %H:%M:%S"
}

## Runs a batch of experiments

inst_path=""
if [ "$1" == "" ] ; then
	echo "Missing instances path."
	exit 0
else
	inst_path=$1
fi

result_folder=""
if [ "$2" == "" ] ; then
	echo "Missing result folder."
	exit 0
else
	result_folder=$2
fi

num_threads=""
if [ "$3" == "" ] ; then
	echo "Missing number of threads."
	exit 0
else
	num_threads=$3
fi

EXE=""
if [ "$4" == "" ] ; then
	echo "Missing executable path."
	exit 0
else
    EXE=$4
fi

REP=""
if [ "$5" == "" ] ; then
	echo "Missing num. repetitions."
	exit 0
else
    REP=$5
fi


par=""
size_par=6
parrep="--num-repetitions $REP "
if (( REP == 0 )) ; then
    size_par=4
    parrep=""
fi

for filename in "$inst_path"/*
do
	## basename returns the last portion of a unix path
	fn=$(basename "$filename")
	## get the instance name only
	inst_name=$(echo $fn | cut -d'.' -f 1)
    log_name=$result_folder"/$inst_name.json"
	par=$par" -i $filename --log $log_name $parrep"
done

echo "Starting $num_threads parallel runs: "$(timestamp)
## echo $par
parallel --no-run-if-empty -j $num_threads -n $size_par $EXE ::: $par
echo "Done with all parallel runs: "$(timestamp)

echo "DONE"
