#!/bin/bash

HISTOGRAMS=$1
OUTPUT=$2


python plot.py $HISTOGRAMS $OUTPUT

# Example usage
# ./plot.sh outfile_pp2jjj.root plots
