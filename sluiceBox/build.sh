#!/bin/sh

g++ -std=c++17 -c FileHandler.cpp -lstdc++fs -O2
g++ -std=c++17 -c Filter.cpp -lstdc++fs -O2
g++ -std=c++17 -c main.cpp -lstdc++fs -O2
g++ -std=c++17 main.o FileHandler.o Filter.o -o sluiceBox -lstdc++fs -O2

rm *.o 