#!/bin/sh

g++ -std=c++17 -c FileHandler.cpp -O2
g++ -std=c++17 -c Filter.cpp -O2
g++ -std=c++17 -c main.cpp -O2
g++ -std=c++17 main.o FileHandler.o Filter.o -o sluiceBox -O2

rm *.o 
