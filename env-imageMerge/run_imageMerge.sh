#!/bin/bash

dataLsit=data.lst
resultPath=/home/data/result_imageMerge_video_1

mkdir $resultPath
./imageMerge $dataLsit $resultPath 30000
