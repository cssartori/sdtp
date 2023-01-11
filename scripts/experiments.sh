#!/bin/bash

timestamp() {
  date +"%Y-%m-%d :: %H:%M:%S"
}

## Script to run all necessary experiments

echo "Starting Experiments at "$(timestamp)

BASE="../data"
mkdir $BASE

./execute.sh "time" $BASE
./execute.sh "cache" $BASE

echo "Ending Experiments at "$(timestamp)
echo "DONE"
