#!/bin/bash

g++ -O2 -o Neurony_PHD ./src/*.cpp ./neurony/*.cpp ./neurony/config/*.cpp ./neurony/gl_mode/*.cpp ./neurony/network/*.cpp ./neurony/dynamics/*.cpp ./neurony/utils/*.cpp  ./neurony/visualisation/*.cpp ./neurony/statistics/*.cpp  ./neurony/statistics/spectrum/*.cpp ./neurony/statistics/path_length/*.cpp ./neurony/statistics/degree_corelation/*.cpp -lpthread -llapack -lblas
