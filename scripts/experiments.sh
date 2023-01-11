#!/bin/bash

timestamp() {
  date +"%Y-%m-%d :: %H:%M:%S"
}

## Script to run all necessary experiments

echo "Starting Experiments at "$(timestamp)

BASE="../data"
mkdir $BASE

./execute.sh "time" $BASE

##cache experiments require super-user privilege
./execute.sh "cache" $BASE

echo "Ending Experiments at "$(timestamp)
echo "DONE"
