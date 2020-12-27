#!/bin/bash
today=`date '+%Hh%M'`;
filename="./output/valgrind-$today";
valgrind --leak-check=full --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file="$filename" ./debug