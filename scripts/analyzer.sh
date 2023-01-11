#!/bin/bash

timestamp() {
  date +"%Y-%m-%d :: %H:%M:%S"
}

## Script used to generate data files and graphs from experiment results

echo "Starting analysis at "$(timestamp)

## generate summarized and aggregated data files
./aggregate.sh
mkdir "../data/time/graphs/"
mkdir "../data/cache/graphs/"
## plot everything
Rscript plotter.R

echo "Ending analysis at "$(timestamp)
echo "DONE"
