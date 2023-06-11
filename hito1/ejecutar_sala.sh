#!/bin/bash

cd ./lib/sala
gcc sala.c -c
cd ../
ar -crs libsala.a sala/sala.o
cd ../fuentes
gcc multihilos.c -o multihilos -lsala -L../lib -lpthread

