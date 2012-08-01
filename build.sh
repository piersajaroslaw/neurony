#!/usr/bin/sh

g++ ./src/*.cpp ./neurony/*.cpp ./neurony/config/*.cpp ./neurony/gl_mode/*.cpp ./neurony/network/*.cpp ./neurony/statistics/*.cpp  ./neurony/statistics/spectrum/*.cpp ./neurony/statistics/path_length/*.cpp -lpthread
