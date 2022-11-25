#!/bin/bash
PROGRAM=add

for i in 1000 10000 100000 1000000
do
    ./cmake_build/basic $i
done

for i in 1000 10000 100000 1000000
do
    ./cmake_build/${PROGRAM} $i
done
